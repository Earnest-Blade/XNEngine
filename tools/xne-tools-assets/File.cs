namespace xne.tools.assets;

public struct File
{
    public string Path;
    public FileStream? Stream;

    public bool Exists => System.IO.File.Exists(Path);
}