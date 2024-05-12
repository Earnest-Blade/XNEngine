namespace xne.assp
{
    // TODO
    public struct ShaderUniform
    {
        public int Attrib;
        public int Format;
        public string Name;
    }

    public struct SubShader
    {
        public int Type;
        public string Name;
    }
    
    public class Shader
    {
        public string Raw;
        public SubShader[] Shaders;
        public ShaderUniform[] Uniforms;
    }
}