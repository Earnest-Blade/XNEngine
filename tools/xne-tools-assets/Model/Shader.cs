namespace xne.tools.assets
{
    public struct Uniform
    {
        public int Attribute;
        public int Format;
        public string Name;

        public Uniform(int attribute, int format, string name)
        {
            Attribute = attribute;
            Format = format;
            Name = name;
        }
    }

    public struct SubShader
    {
        public int Type;
        public string Name;

        public SubShader(int type, string name)
        {
            Type = type;
            Name = name;
        }
    }
    
    public class Shader : IRaw
    {
        public string Name { get; set; }
        public string? Raw { get; set; }
        public string? Path { get; set; }
        
        public SubShader[] Shaders;
        public Uniform[] Uniforms;

        public Shader(string name, string? raw, string? path)
        {
            Name = name;
            Raw = raw;
            Path = path;
            Shaders = new[]
            {
                new SubShader(35633, "_VERTEX_"),
                new SubShader(35632, "_FRAGMENT_"),
                new SubShader(0, "")
            };
            Uniforms = new[]
            {
                new Uniform(1, 5, "projection"),
                new Uniform(1, 5, "transform"),
                new Uniform(0, 0, "")
            };
        }
        
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
            
            return new Shader(name, raw, file.Path);
        }
    }
}