using System.Text;
using xne.assets;

public class Program
{
    public static void Main(string[] args)
    {
        var arguments = ParseArguments(args);

        if (arguments.Count == 0)
        {
            PrintHelp();
            return;
        }

        if (arguments.ContainsKey("--skip-asks"))
        {
            Debug.AskFile = (message, mode) => new xne.assets.File() { Path = "" };
            Debug.AskString = message => "";
        }
        
        Console.WriteLine("creating a new model!");
        DateTime stime = DateTime.Now;
        
        ModelExportProperties exportProperties = new ModelExportProperties();
        ModelImportProperties importProperties = new ModelImportProperties();
        string fin = "", fout = "";

        if (arguments.TryGetValue("--finput", out string? in0)) fin = in0;
        if (arguments.TryGetValue("--foutput", out string? out0)) fout = out0;
        
        if(arguments.TryGetValue("--compression-level", out string? value)) {
            exportProperties.CompressionLevel = int.Parse(value);
        }
        
        if (arguments.ContainsKey("--triangulate")) importProperties.Triangulate = true;
        if (arguments.ContainsKey("--optimize")) importProperties.Optimize = true;
        if (arguments.ContainsKey("--join-vertices")) importProperties.JoinVertices = true;
        if (arguments.ContainsKey("--generate-normals")) importProperties.GenerateNormals = true;
        if (arguments.ContainsKey("--make-readable")) exportProperties.IsReadable = true;

        Console.WriteLine(fout);
        
        Asset<Defs.Model> model_context = Asset<Defs.Model>.CreateModel(fin, exportProperties, importProperties);
        model_context.Export(fout);
        model_context.Dispose();

        Console.WriteLine($"export finished in {(DateTime.Now - stime).Seconds}sec");
        
        return;
    }

    static Dictionary<string, string> ParseArguments(string[] args)
    {
        var arguments = new Dictionary<string, string>();

        foreach (var arg in args)
        {
            string[] parts = arg.Split('=');

            if (parts.Length == 2)
            {
                arguments[parts[0]] = parts[1];
            }
            
            else
            {
                arguments[arg] = null;
            }
        }

        return arguments;
    }

    static void PrintHelp()
    {
        Console.WriteLine("--help: Displays help for each argument.");
        Console.WriteLine("--skip-asks: Skip user input prompts.");
        Console.WriteLine("--finput: Path to the input file. Usage: --finput=file_path");
        Console.WriteLine("--foutput: Path to the output file. Usage: --foutput=file_path");
        Console.WriteLine("--compression-level: Compression level (integer). Usage: --compression-level=value");
        Console.WriteLine("--triangulate: Triangulate the imported model.");
        Console.WriteLine("--optimize: Optimize the imported model.");
        Console.WriteLine("--join-vertices: Join vertices of the imported model.");
        Console.WriteLine("--generate-normals: Generate normals for the imported model.");
        Console.WriteLine("--make-readable: Make the exported model readable.");
        Console.ReadKey();

    }
}