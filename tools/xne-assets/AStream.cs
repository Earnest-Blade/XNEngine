namespace xne.assets;

public abstract class AStream<T> : IDisposable, IEquatable<AStream<T>>
{
    public T? Value { get; protected set; }

    protected FileStream? _inStrategy;
    protected FileStream? _outStrategy;

    protected AStream()
    {
        Value = default;
    }
        
    public abstract void Export(Asset<T> header, string file);
    public abstract string QuickExport(Asset<T> header);
        
    public abstract void Import(string file);

    public void Dispose()
    {
        _inStrategy?.Dispose();
        _outStrategy?.Dispose();
    }

    public bool Equals(AStream<T>? other)
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
        return Equals((AStream<T>)obj);
    }

    public override int GetHashCode()
    {
        return HashCode.Combine(_inStrategy, _outStrategy);
    }
}