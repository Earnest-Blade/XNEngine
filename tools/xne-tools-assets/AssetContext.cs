

namespace xne.tools.assets
{
    public class AssetContext
    {
        public const string VERSION = "0.0.0";
        
        public readonly string CurrentDirectory;

        private static AssetContext? _instance = null;

        private AssetContext()
        {
            CurrentDirectory = Environment.GetFolderPath(Environment.SpecialFolder.MyDocuments);
        }

        
        public static AssetContext GetInstance() { 
            if (_instance == null) {
                _instance = new AssetContext(); 
            } 

            return _instance; 
        }

        public static bool HasContext() => _instance != null;
    }
}
