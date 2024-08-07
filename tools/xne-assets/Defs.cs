using System.Numerics;

namespace xne.assets;

public static class Defs
{
    public const string XNE_ASSET_VERSION = "0.0.0";
    
    public const int XNE_VERTEX_SHADER = 0x8B31;
    public const int XNE_FRAGMENT_SHADER = 0x8B30;
    public const int XNE_GEOMETRY_SHADER = 0x8DD9;
    
    public const int XNE_UNIFORM_UNKNOWN = 0;
    public const int XNE_UNIFORM_FLOAT = 1;
    public const int XNE_UNIFORM_INT = 2;
    public const int XNE_UNIFORM_VEC2 = 3;
    public const int XNE_UNIFORM_VEC3 = 4;
    public const int XNE_UNIFORM_VEC4 = 5;
    public const int XNE_UNIFORM_MAT4 = 6;
    public const int XNE_UNIFORM_LIGHT = 7;
    public const int XNE_UNIFORM_MATERIAL = 8;
    
    public const int XNE_UNIFORM_ATTRIB_UNIFORM = 1;
    public const int XNE_UNIFORM_ATTRIB_STRUCT = 2;
    public const int XNE_UNIFORM_ATTRIB_ARRAY = 4;
    
    public const int XNE_TEXTURE_FILTER_NEAREST = 0x2600;
    public const int XNE_TEXTURE_FILTER_LINEAR = 0x2601;
    public const int XNE_TEXTURE_WRAP_REPEAT = 0x2901;
    public const int XNE_TEXTURE_WRAP_CLAMP_TO_EDGE = 0x812F;

    public const int XNE_OBJECT_NULL = 0x0000;
    public const int XNE_OBJECT_MODEL = 0x0001;
    public const int XNE_OBJECT_SPRITE = 0x0002;
    public const int XNE_OBJECT_SCENE = 0x0004;

    public struct Mesh
    {
        public string Name;
        public float[] Vertices;
        public UInt32[] Elements;
    }

    public struct Transform
    {
        public Vector3 Position, Scale;
        public Vector4 Rotation;

        public Transform()
        {
            Position = new Vector3();
            Scale = new Vector3();
            Rotation = new Vector4(0.0f, 0.0f, 0.0f, 0.0f);
        }

        public Transform(Vector3 position, Vector3 scale, Vector4 rotation)
        {
            Position = position;
            Scale = scale;
            Rotation = rotation;
        }
    }

    public struct Texture : IEquatable<Texture>
    {
        public string Path;
        public string Name;
        public int Wrap;
        public int Filter;

        public Texture(string path, string name)
        {
            Path = path;
            Name = name;
            Wrap = XNE_TEXTURE_WRAP_CLAMP_TO_EDGE;
            Filter = XNE_TEXTURE_FILTER_NEAREST;
        }

        public bool Equals(Texture other)
        {
            return Path == other.Path;
        }

        public override bool Equals(object? obj)
        {
            return obj is Texture other && Equals(other);
        }

        public override int GetHashCode()
        {
            return Path.GetHashCode();
        }
    }

    public class Shader : IEquatable<Shader>
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

        public string Name;
        public string Path;
        public SubShader[] Shaders;
        public Uniform[] Uniforms;

        public Shader(string name, string path)
        {
            Name = name;
            Path = path;
            Shaders = new[]
            {
                new SubShader(XNE_VERTEX_SHADER, "_VERTEX_"),
                new SubShader(XNE_FRAGMENT_SHADER, "_FRAGMENT_"),
                new SubShader(0, "")
            };

            Uniforms = new[]
            {
                new Uniform(XNE_UNIFORM_ATTRIB_UNIFORM, XNE_UNIFORM_MAT4, "projection"),
                new Uniform(XNE_UNIFORM_ATTRIB_UNIFORM, XNE_UNIFORM_MAT4, "transform")
            };
        }

        public bool Equals(Shader? other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Path == other.Path;
        }

        public override bool Equals(object? obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((Shader)obj);
        }

        public override int GetHashCode()
        {
            return Path.GetHashCode();
        }
    }

    public class Material
    {
        public string Name;
        public Shader Shader;
        
        public Vector4 AmbientColor;
        public Vector4 DiffuseColor;

        public int AmbientTexture;
        public int DiffuseTexture;
    }

    public class Model
    {
        public struct Node : ICloneable
        {
            public string Name;
            public Node[] Childs;
            public Transform Transform;
            
            public UInt32? Mesh;
            public UInt32? Material;
            
            public object Clone()
            {
                return MemberwiseClone();
            }
        }

        public string Name;
        public Node Root;
        public Mesh[] Meshes;
        public Texture[] Textures;
        public Material[] Materials;
    }
}