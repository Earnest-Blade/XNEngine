namespace xne.assp
{
    static class Program
    {
        private static readonly string VERSION = "0.0.0";
        
        [STAThread]
        static void Main(string[] args)
        {
            Console.Clear();
            Console.WriteLine($"Welcome to XNE Model Convertor v{VERSION}");
            Console.WriteLine($"Working Directory: '${Environment.CurrentDirectory}'");

            if (args.Length == 0)
            {
                Console.Write("Enter file to convert: ");
                string? inputf = GetUserInput("Input File");
            
                Console.Write("Enter output file: ");
                string? outputf = GetUserInput("Output File");

                ConvertModel(inputf, outputf);
            }
            
            Console.WriteLine("Press any key to quit.");
            Console.ReadKey();
        }

        private static void HelpCommand()
        {
            
        }

        private static void ConvertModel(string input, string output)
        {
            Console.Write("Do you want to compress the result? [Y]es/[N]o ");
            bool compress = GetUserInput("Compress File").ToLower()[0] == 'y';

            bool format = false;
            if (!compress)
            {
                Console.Write("Do you want to have a correctly format (might be heavier) [Y]es/[N]o ");
                format = GetUserInput("Format output").ToLower()[0] == 'y';
            }
            
            Console.Write("Do you want to optimize meshes? [Y]es/[N]o ");
            bool optimize = GetUserInput("Optimize Meshes").ToLower()[0] == 'y';
            
            DateTimeOffset offset = DateTimeOffset.Now;
            
            Scene? root = null;
            using (Importer imprt = new Importer(new FileStream(input, FileMode.Open)))
            {
                ImportDesc settings = new ImportDesc();
                settings.Name = "root";
                settings.Triangulate = true;
                settings.Optimize = optimize;
                
                root = imprt.Import(settings);
                
                Console.WriteLine("");
                Console.WriteLine($"Generated {imprt.GetVerticesCount()} vertices!");
                Console.WriteLine($"Generated {imprt.GetMeshCount()} meshes!");
            }

            using (Exporter exptr = new Exporter(output))
            {
                ExportDesc settings = new ExportDesc();
                settings.Name = input;
                settings.Version = VERSION;
                settings.Scene = root;
                settings.Format = format;
                settings.Compress = compress;
                
                exptr.Export(settings);
                Console.WriteLine($"Wrote {exptr.GetStreamSize()} characters!");
            }
            
            Console.WriteLine($"\nThe conversion took {(DateTimeOffset.Now - offset).TotalMilliseconds}ms !");
        }

        private static string GetUserInput(string name)
        {
            string? user = Console.ReadLine()?.Trim(' ');
            if (string.IsNullOrWhiteSpace(user)) throw new ArgumentNullException(nameof(user), $"'{name}' is Empty or null!");
            return user;
        }
    }
}
    