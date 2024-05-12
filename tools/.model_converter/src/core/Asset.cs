namespace xne.assp
{
    public class Asset
    {
        public string Name;
        public string Version;
        public Scene Scene;

        public Asset(string name, string version, Scene scene)
        {
            Name = name;
            Version = version;
            Scene = scene;
        }
    }
}