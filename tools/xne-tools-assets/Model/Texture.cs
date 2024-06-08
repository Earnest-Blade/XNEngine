namespace xne.tools.assets
{
    public struct Texture : IEquatable<Texture>, IRaw
    {
        public string Name { get; set; }
        public string Raw { get; set; }
        public int Wrap, Filter;
        
        public string? Path => _path;

        private string _path;

        public Texture(string path, string name)
        {
            _path = path;
            
            Name = name;
            Wrap = 10497;
            Filter = 9729;
        }

        public bool Equals(Texture other)
        {
            return _path == other._path;
        }

        public override bool Equals(object? obj)
        {
            return obj is Texture other && Equals(other);
        }

        public override int GetHashCode()
        {
            return _path.GetHashCode();
        }
    }
}