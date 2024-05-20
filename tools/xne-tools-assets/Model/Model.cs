namespace xne.tools.assets
{
    public class Model : IAsset
    {
        public string? Name { get; set; }
        public Node Root;

        public Mesh[] Meshes;
        public Texture[] Textures;
        public Material[] Materials;
        
        public struct Node
        {
            public string Name;
            public Node[] Childs;
            public Transform Transform;
            
            public UInt32? Mesh;
            public UInt32? Material;
        }
    }
}