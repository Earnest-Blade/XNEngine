namespace xne.tools.assets
{
    public class Asset<T> : IEquatable<Asset<T>> where T : IAsset
    {
        public string? Name;
        public string Version;

        public AssetType Type
        {
            get
            {
                return Value switch
                {
                    Model m => AssetType.Model,
                    _ => AssetType.None
                };
            }
        }
        public T Value;

        public Asset(string? name, string version, T value)
        {
            Name = name;
            Version = version;
            Value = value;
        }

        public bool Equals(Asset<T>? other)
        {
            return other != null && Value.Equals(other.Value);
        }
    }
}