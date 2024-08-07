using static xne.assets.Defs;

namespace xne.assets;

public class Asset<T> : IDisposable
{
    public string Name;
    public string Version;
    public int Type;

    public T Value;

    private AStream<T> _stream;

    public void Export(string file)
    {
        _stream.Export(this, file);
    }

    public void Dispose()
    {
        _stream.Dispose();
    }

    public override string ToString()
    {
        return _stream.QuickExport(this);
    }

    public static Asset<Model> CreateModel(string file, ModelExportProperties export, ModelImportProperties import)
    {
        Asset<Model> asset = new Asset<Model>();
        asset.Name = file;
        asset.Version = XNE_ASSET_VERSION;
        asset.Type = XNE_OBJECT_MODEL;

        asset._stream = new MStream(export, import);
        asset._stream.Import(file);
        asset.Value = asset._stream.Value;

        return asset;
    }

}