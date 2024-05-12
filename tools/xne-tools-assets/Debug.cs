namespace xne.tools.assets
{
    public static class Debug
    {
        public delegate string? AskStringDelegate(string? message);
        public delegate File AskFileDelegate(string? message, FileMode mode);
        
        public static AskStringDelegate AskString;
        public static AskFileDelegate AskFile;

        public static string? AskStringDefault(string? message)
        {
            if(!string.IsNullOrEmpty(message))
                Console.Write(message);
            
            return Console.ReadLine();
        }

        public static File AskFileDefault(string? message, FileMode mode)
        {
            if(!string.IsNullOrEmpty(message))
                Console.Write(message);

            string path = Console.ReadLine();
            if (string.IsNullOrEmpty(path))
            {
                Console.WriteLine("previously entered path is empty, please, insert a valid one.");
                return AskFileDefault(message, mode);
            }

            return new File()
            {
                Path = path,
                Stream = System.IO.File.Exists(path) ?  System.IO.File.Open(path, mode) : null
            };
        }

        static Debug()
        {
            AskString = AskStringDefault;
            AskFile = AskFileDefault;
        }
    }
}