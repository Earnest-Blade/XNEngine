namespace xne.tools.assets
{
    public struct Uniform
    {
        public int Attribute;
        public int Format;
        public string Name;
    }

    public struct SubShader
    {
        public int Type;
        public string Name;
    }
    
    public class Shader : IRaw
    {
        public string Name { get; set; }
        public string? Raw { get; set; }
        public string? Path { get; set; }
        
        public SubShader[] Shaders;
        public Uniform[] Uniforms;

        public static Shader CreateFromFile(File file, string name)
        {
            string? raw = null;
            
            if (file.Exists)
            {
                using (StreamReader reader = new StreamReader(file.Stream))
                {
                    raw = reader.ReadToEnd();
                }
            }
            
            return new Shader()
            {
                Name = name,
                Raw = raw,
                Path = file.Path,
                Shaders = new SubShader[1],
                Uniforms = new Uniform[1]
            };
        }
    }
}