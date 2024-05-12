using System.Numerics;
using Assimp;

namespace xne.assp
{
    public class Importer : IDisposable
    {
        private const int VERTICES_SIZE = 8;
        
        private AssimpContext _context;
        private Stream _stream;
        
        private ImportDesc _desc;

        private List<Mesh> _meshes;
        private List<Texture> _textures;
        private List<Material> _materials;
        private Node? _root;

        public Importer(Stream stream)
        {
            _stream = stream;
            _context = new AssimpContext();
            _meshes = new List<Mesh>();
            _textures = new List<Texture>();
            _materials = new List<Material>();
            _root = null;
        }

        public Scene? Import(ImportDesc desc)
        {
            _desc = desc;
            
            PostProcessSteps pp = PostProcessSteps.FlipUVs | PostProcessSteps.JoinIdenticalVertices;
            if (desc.Triangulate) pp |= PostProcessSteps.Triangulate;
            if (desc.Optimize) pp |= (PostProcessSteps.OptimizeMeshes | PostProcessSteps.OptimizeGraph);

            Assimp.Scene scene = _context.ImportFileFromStream(_stream, pp);
            if(scene.SceneFlags.HasFlag(SceneFlags.Incomplete))
                Console.WriteLine("Scene is incomplete!");
            
            _root = ProcessNode(scene.RootNode, scene);
            foreach (Assimp.Material mat in scene.Materials) ProcessMaterial(mat);

            Scene scene0 = new Scene
            {
                Name = desc.Name,
                Root = _root,
                Meshes = _meshes.ToArray(),
                Textures = _textures.ToArray(),
                Materials = _materials.ToArray()
            };

            return scene0;
        }

        private Node ProcessNode(Assimp.Node node, Assimp.Scene scene)
        {
            List<Node> nodes = new List<Node>();
            List<UInt32> meshes = new List<uint>();

            int sr = _meshes.Count;
            for (int i = 0; i < node.MeshCount; i++)
            {
                Assimp.Mesh mesh = scene.Meshes[node.MeshIndices[i]];
                _meshes.Add(ProcessMesh(mesh, scene));
                meshes.Add((UInt32)(sr + i));
            }

            for (int i = 0; i < node.ChildCount; i++)
            {
                nodes.Add(ProcessNode(node.Children[i], scene));
            }
            
            return new Node
            {
                Name = node.Name,
                Childs = nodes.ToArray(),
                Meshes = meshes.ToArray()
            };
        }

        private Mesh ProcessMesh(Assimp.Mesh mesh, Assimp.Scene scene)
        {
            List<float> vertices = new List<float>();
            List<UInt32> indices = new List<UInt32>();

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
                Console.WriteLine($"Process face: {i}");

                for (int j = 0; j < mesh.Faces[i].IndexCount; j++)
                {
                    indices.Add((uint)mesh.Faces[i].Indices[j]);
                }
            }

            Mesh result = new Mesh
            {
                Name = mesh.Name,
                Vertices = vertices.ToArray(),
                Indices = indices.ToArray(),
                Material = mesh.MaterialIndex
            };
            return result;
        }

        private void ProcessMaterial(Assimp.Material mat)
        {
            Console.Write($"New Material named {mat.Name} Detected! Enter material's shader path (can be empty)");
            string? shaderPath = Console.ReadLine()?.Trim();
            
            Shader shader = new Shader();
            if (File.Exists(shaderPath))
            {
                shader.Raw = File.ReadAllText(shaderPath);
            }
            
            _materials.Add(new Material()
            {
                Shader = shader,
                AmbientColor = new Vector4(mat.ColorAmbient.R, mat.ColorAmbient.G, mat.ColorAmbient.B, mat.ColorAmbient.A),
                DiffuseColor = new Vector4(mat.ColorDiffuse.R, mat.ColorDiffuse.G, mat.ColorDiffuse.B, mat.ColorDiffuse.A),
                DiffuseTexture = ProcessTexture(mat.TextureDiffuse)
            });
        }

        private int ProcessTexture(TextureSlot slot)
        {
            for (int i = 0; i < _textures.Count; i++)
            {
                if (_textures[i].FilePath == slot.FilePath) return i;
            }

            _textures.Add(new Texture());

            _textures.Last().FilePath = slot.FilePath;
            if (File.Exists(slot.FilePath))
            {
                _textures.Last().Raw = File.ReadAllText(slot.FilePath);
            }
            
            return _textures.Count - 1;
        }

        public long GetVerticesCount()
        {
            long x = 0;
            foreach (Mesh mesh in _meshes)
            {
                x += mesh.Vertices.Length;
            }

            return x / VERTICES_SIZE;
        }

        public long GetMeshCount()
        {
            return _meshes.Count;
        }

        public void Dispose()
        {
            _context.Dispose();
            _stream.Dispose();
        }
    }
}