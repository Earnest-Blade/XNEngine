using System.Numerics;
using Assimp;

namespace xne.tools.assets
{
    public struct Material
    {
        public string Name;
        public Shader? Shader;
        
        public Vector4 AmbientColor;
        public Vector4 DiffuseColor;

        public int AmbientTexture;
        public int DiffuseTexture;
    }
}