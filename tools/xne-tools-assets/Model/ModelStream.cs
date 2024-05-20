using System.Numerics;
using System.Text;
using Assimp;
using Newtonsoft.Json;

namespace xne.tools.assets
{
    public struct ModelExportDesc
    {
        public string? Name;
        public string Version;
        public CompressLevel CompressLevel;
        public bool Formatting;
    }

    public struct ModelImportDesc
    {
        public string? Name;
        public bool Triangulate, Optimize, JoinVertices;
    }
    
    public class ModelStream : AssetStream<Model>
    {
        private const int VERTICES_SIZE = 8;
        
        public ModelExportDesc? ExportDesc;
        public ModelImportDesc? ImportDesc;

        private List<Mesh>? _meshes;
        private List<Texture>? _textures;
        private List<Material>? _materials;

        private Model.Node _root;

        public ModelStream(ModelExportDesc? exportDesc, ModelImportDesc? importDesc) : base()
        {
            ExportDesc = exportDesc;
            ImportDesc = importDesc;
        }

        public override void Export(string file)
        {
            ArgumentException.ThrowIfNullOrEmpty(file);
            if (!ExportDesc.HasValue)
            {
                throw new ArgumentNullException(nameof(ExportDesc));
            }

            _outStrategy = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write, FileShare.Delete);

            _asset = new Asset<Model>(ExportDesc.Value.Name, ExportDesc.Value.Version, Value);

            JsonSerializerSettings settings = new JsonSerializerSettings();
            settings.Formatting = ExportDesc.Value.Formatting ? Formatting.Indented : Formatting.None;

            string buffer = JsonConvert.SerializeObject(new { Asset = _asset }, settings);

            _outStrategy.Position = 0;
            
            byte[] bytes = Encoding.UTF8.GetBytes(buffer);
            if (ExportDesc.Value.CompressLevel != CompressLevel.NO_COMPRESSION)
            {
                bytes = Compress.CompressBytes(Encoding.UTF8.GetBytes(buffer), ExportDesc.Value.CompressLevel);
            }
            
            _outStrategy.WriteByte((byte)((ExportDesc.Value.CompressLevel > 0) ? 1 : 0));
            _outStrategy.Write(bytes, 0, bytes.Length);
            
            _outStrategy.Dispose();
            _outStrategy = null;
        }

        public override string QuickExport()
        {
            _asset = new Asset<Model>(ExportDesc.Value.Name, ExportDesc.Value.Version, Value);

            JsonSerializerSettings settings = new JsonSerializerSettings();
            settings.Formatting = Formatting.Indented;

            return JsonConvert.SerializeObject(new { Asset = _asset }, settings);
        }

        public override void Import(string file)
        {
            ArgumentException.ThrowIfNullOrEmpty(file);
            if (!ImportDesc.HasValue)
            {
                throw new ArgumentNullException(nameof(ImportDesc));
            }
            
            _inStrategy = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.Read);
            _meshes = new List<Mesh>();
            _textures = new List<Texture>();
            _materials = new List<Material>();
            
            AssimpContext context = new AssimpContext();

            PostProcessSteps pps = PostProcessSteps.FlipUVs;
            if (ImportDesc.Value.JoinVertices) pps |= PostProcessSteps.JoinIdenticalVertices;
            if (ImportDesc.Value.Triangulate) pps |= PostProcessSteps.Triangulate;
            if (ImportDesc.Value.Optimize) pps |= PostProcessSteps.OptimizeMeshes | PostProcessSteps.OptimizeGraph;

            Assimp.Scene scene = context.ImportFileFromStream(_inStrategy);
            if (scene.SceneFlags.HasFlag(SceneFlags.Incomplete))
            {
                throw new SystemException("failed to import model!");
            }

            _root = ProcessNode(scene.RootNode, scene);
            ProcessMaterials(scene.Materials.ToArray());

            Value = new Model()
            {
                Name = ImportDesc.Value.Name,
                Root = _root,
                Materials = _materials.ToArray(),
                Meshes = _meshes.ToArray(),
                Textures = _textures.ToArray()
            };
            
            _inStrategy.Dispose();
            _inStrategy = null;
            context.Dispose();
        }

        private Model.Node ProcessNode(Assimp.Node node, Assimp.Scene scene)
        {
            List<Model.Node> nodes = new List<Model.Node>();
            UInt32? mesh = null;
            UInt32? material = null;

            if (node.MeshCount > 0 && node.MeshIndices.Count > 0)
            {
                Assimp.Mesh mesh0 = scene.Meshes[node.MeshIndices[0]];
                _meshes.Add(ProcessMesh(mesh0));
                
                mesh = (uint?)(_meshes.Count - 1);
                material = (uint?)(mesh0.MaterialIndex);
            }

            for (int i = 0; i < node.ChildCount; i++)
            {
                nodes.Add(ProcessNode(node.Children[i], scene));
            }

            return new Model.Node()
            {
                Name = node.Name,
                Childs = nodes.ToArray(),
                Transform = new Transform(node.Transform),
                Material = material,
                Mesh = mesh
            };
        }

        private Mesh ProcessMesh(Assimp.Mesh mesh)
        {
            List<float> vertices = new List<float>();
            List<UInt32> indices = new List<uint>();

            for (int i = 0; i < mesh.VertexCount; i++)
            {
                float[] vert = new float[VERTICES_SIZE];
                vert[0] = mesh.Vertices[i].X;
                vert[1] = mesh.Vertices[i].Y;
                vert[2] = mesh.Vertices[i].Z;
                
                if (mesh.HasTextureCoords(0))
                {
                    vert[3] = mesh.TextureCoordinateChannels[0][i].X;
                    vert[4] = mesh.TextureCoordinateChannels[0][i].Y;
                }
                else
                {
                    vert[3] = 0.0f;
                    vert[4] = 0.0f;
                }

                vert[5] = mesh.Normals[i].X;
                vert[6] = mesh.Normals[i].Y;
                vert[7] = mesh.Normals[i].Z;
                
                vertices.AddRange(vert);
            }

            for (int i = 0; i < mesh.FaceCount; i++)
            {
                for (int j = 0; j < mesh.Faces[i].IndexCount; j++)
                {
                    indices.Add((uint)mesh.Faces[i].Indices[j]);
                }
            }

            return new Mesh()
            {
                Name = mesh.Name,
                Vertices = vertices.ToArray(),
                Elements = indices.ToArray()
            };
        }

        private void ProcessMaterials(Assimp.Material[] materials)
        {
            for(int i = 0; i < materials.Length; i++)
            {
                _materials.Add(new Material()
                {
                    Shader = Shader.CreateFromFile(
                        Debug.AskFile($"Shader{i} file path : ", FileMode.Open),
                        Debug.AskString($"Shader{i} name : ")!
                        ),
                    Name = materials[i].Name,
                    AmbientColor = new Vector4(materials[i].ColorAmbient.R, materials[i].ColorAmbient.G, materials[i].ColorAmbient.B, materials[i].ColorAmbient.A),
                    DiffuseColor = new Vector4(materials[i].ColorDiffuse.R, materials[i].ColorDiffuse.G, materials[i].ColorDiffuse.B, materials[i].ColorDiffuse.A),
                    AmbientTexture = ProcessTexture(materials[i].TextureAmbient),
                    DiffuseTexture = ProcessTexture(materials[i].TextureDiffuse)
                });
            }
        }
        
        private int ProcessTexture(TextureSlot slot)
        {
            int index = _textures.FindIndex(tex => tex.Path == slot.FilePath);
            if (index != -1) return index;
            
            _textures.Add(new Texture(slot.FilePath, Debug.AskString($"name to texture at '{slot.FilePath}' : ")));
            return _textures.Count - 1;
        }

    }
}