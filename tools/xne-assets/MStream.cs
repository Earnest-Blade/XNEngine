using System.Numerics;
using System.Text;
using Assimp;
using Newtonsoft.Json;

namespace xne.assets;

using static Defs;
using static Utils;

public struct ModelExportProperties
{
    public string Name;
    public int CompressionLevel;
    public bool IsReadable;

    public ModelExportProperties()
    {
        Name = string.Empty;
        CompressionLevel = XNE_NO_COMPRESSION;
        IsReadable = true;
    }
}

public struct ModelImportProperties
{
    public string Name;
    
    public bool Triangulate;
    public bool Optimize;
    public bool JoinVertices;
    public bool GenerateNormals;

    public ModelImportProperties()
    {
        Name = string.Empty;
        Triangulate = true;
        Optimize = true;
        JoinVertices = true;
        GenerateNormals = false;
    }
}

public class MStream : AStream<Model>
{
    public ModelExportProperties? ExportProperties;
    public ModelImportProperties? ImportProperties;

    private Queue<Mesh>? _meshes;
    private List<Texture> _textures;
    private Queue<Material> _materials;

    public MStream(ModelExportProperties exportProperties, ModelImportProperties importProperties) : base()
    {
        ExportProperties = exportProperties;
        ImportProperties = importProperties;
    }

    public override void Export(Asset<Model> header, string file)
    {
        ArgumentException.ThrowIfNullOrEmpty(file);
        if (!ExportProperties.HasValue)
        {
            throw new ArgumentNullException(nameof(ExportProperties));
        }

        _outStrategy = new FileStream(file, FileMode.OpenOrCreate, FileAccess.Write, FileShare.Delete);

        string buffer = QuickExport(header);

        _outStrategy.Position = 0;
        byte[] bytes = Encoding.UTF8.GetBytes(buffer);
        if (ExportProperties.Value.CompressionLevel != XNE_NO_COMPRESSION)
        {
            bytes = CompressBytes(Encoding.UTF8.GetBytes(buffer), ExportProperties.Value.CompressionLevel);
        }
            
        _outStrategy.WriteByte((byte)((ExportProperties.Value.CompressionLevel > 0) ? 1 : 0));
        _outStrategy.Write(bytes, 0, bytes.Length);
            
        _outStrategy.Dispose();
        _outStrategy = null;
    }

    public override string QuickExport(Asset<Model> header)
    {
        JsonSerializerSettings settings = new JsonSerializerSettings();
        settings.Formatting = ExportProperties.Value.IsReadable ? Formatting.Indented : Formatting.None;

        string buffer = JsonConvert.SerializeObject(new { Asset = header }, settings);
        return buffer;
    }

    public override void Import(string file)
    {
        ArgumentException.ThrowIfNullOrEmpty(file);
        if (!ImportProperties.HasValue)
        {
            throw new ArgumentNullException(nameof(ImportProperties));
        }

        _inStrategy = new FileStream(file, FileMode.Open, FileAccess.Read);
        _meshes = new Queue<Mesh>();
        _textures = new List<Texture>();
        _materials = new Queue<Material>();

        AssimpContext context = new AssimpContext();

        PostProcessSteps postProcessSteps = PostProcessSteps.FlipUVs;
        if (ImportProperties.Value.JoinVertices) postProcessSteps |= PostProcessSteps.JoinIdenticalVertices;
        if (ImportProperties.Value.Triangulate) postProcessSteps |= PostProcessSteps.Triangulate;
        if (ImportProperties.Value.Optimize) postProcessSteps |= PostProcessSteps.OptimizeMeshes | PostProcessSteps.OptimizeGraph;
        if (ImportProperties.Value.GenerateNormals) postProcessSteps |= PostProcessSteps.GenerateNormals;

        Assimp.Scene scene = context.ImportFileFromStream(_inStrategy);
        if (scene.SceneFlags.HasFlag(SceneFlags.Incomplete))
        {
            throw new SystemException($"failed to import model from '{file}'!");
        }

        Model.Node root = ProcessNode(scene.RootNode, scene);
        ProcessMaterials(scene.Materials.ToArray());

        Value = new Model()
        {
            Name = ImportProperties.Value.Name,
            Root = (Model.Node)root.Clone(),
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
                _meshes.Enqueue(ProcessMesh(mesh0));
                
                mesh = (uint?)(_meshes.Count - 1);
                material = (uint?)(mesh0.MaterialIndex);
            }

            for (int i = 0; i < node.ChildCount; i++)
            {
                nodes.Add(ProcessNode(node.Children[i], scene));
            }

            node.Transform.Decompose(out var scale, out var rotation, out var position);

            return new Model.Node()
            {
                Name = node.Name,
                Childs = nodes.ToArray(),
                Transform = new Transform(
                    new Vector3(position.X, position.Y, position.Z),
                    new Vector3(scale.X, scale.Y, scale.Z),
                    new Vector4(rotation.X, rotation.Y, rotation.Z, rotation.W)),
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
                float[] vert = new float[8];
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
                _materials.Enqueue(new Material()
                {
                    Shader = new Shader(
                        Debug.AskString($"Shader{i} name : "),
                        Debug.AskFile($"Shader{i} file path : ", FileMode.Open).Path
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
        
        if (string.IsNullOrWhiteSpace(slot.FilePath))
        {
            slot.FilePath = Debug.AskString($"cannot find texture{slot.TextureIndex}'s path, please enter a new path : ");
        }
        
        int index = _textures.FindIndex(tex => tex.Path == slot.FilePath);
        if (index != -1) return index;

        _textures.Add(new Texture(slot.FilePath, Debug.AskString($"please enter a name for the texture at '{slot.FilePath}' : ")));
        return _textures.Count - 1;
    }
}