namespace xne.tools.assets
{
    public abstract class AssetStream<T> : IDisposable, IEquatable<AssetStream<T>> where T : IAsset
    {
        protected const int DefaultBufferSize = 4096;

        public T Value { get; protected set; }

        protected Asset<T> _asset;
        protected FileStream? _inStrategy;
        protected FileStream? _outStrategy;

        protected AssetStream()
        {
            
        }
        
        public abstract void Export(string file);
        public abstract string QuickExport();
        
        public abstract void Import(string file);

        public void Dispose()
        {
            _inStrategy?.Dispose();
            _outStrategy?.Dispose();
        }

        public bool Equals(AssetStream<T>? other)
        {
            if (ReferenceEquals(null, other)) return false;
            if (ReferenceEquals(this, other)) return true;
            return Equals(_inStrategy, other._inStrategy) && Equals(_outStrategy, other._outStrategy);
        }

        public override bool Equals(object? obj)
        {
            if (ReferenceEquals(null, obj)) return false;
            if (ReferenceEquals(this, obj)) return true;
            if (obj.GetType() != this.GetType()) return false;
            return Equals((AssetStream<T>)obj);
        }

        public override int GetHashCode()
        {
            return HashCode.Combine(_inStrategy, _outStrategy);
        }
    }
}