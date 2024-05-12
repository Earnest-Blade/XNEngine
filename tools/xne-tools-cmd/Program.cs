using xne.tools.assets;

public class Program
{
    public static void Main(string[] args)
    {
        Console.WriteLine($"working dir : '{Directory.GetCurrentDirectory()}'");
        
        string ifile, efile;
        
        if (args.Length <= 1 || string.IsNullOrEmpty(args[0]) || string.IsNullOrEmpty(args[1]))
        {
            ifile = Debug.AskString("import file : ");
            efile = Debug.AskString("export file : ");
        }
        else
        {
            ifile = args[0];
            efile = args[1];
        }

        ModelImportDesc importDesc = new ModelImportDesc();
        importDesc.Name = Debug.AskString("enter model name : ");
        importDesc.JoinVertices = AskBool("join vertices ? [Y]es or [N]o ? ");
        importDesc.Triangulate = AskBool("triangulate mesh ? [Y]es or [N]o ? ");
        importDesc.Optimize = AskBool("optimize mesh ? [Y]es or [N]o ? ");

        ModelExportDesc exportDesc = new ModelExportDesc();
        exportDesc.Name = importDesc.Name;
        exportDesc.Formatting = AskBool("format export json ? [Y]es or [N]o ? ");
        exportDesc.CompressLevel = (CompressLevel)AskInt("compression level (0: no compression, 1: default, 2: best) : ");
        exportDesc.Version = AssetContext.VERSION;

        using (ModelStream stream = new ModelStream(exportDesc, importDesc))
        {
            stream.Import(ifile);
            stream.Export(efile);
        }

        Console.WriteLine($"successfully converted {ifile}!");
        Console.ReadKey();
    }

    private static bool AskBool(string message)
    {
        string value = Debug.AskString(message).ToLower();
        return value[0] switch
        {
            'y' => true,
            'n' => false,
            _ => throw new ArgumentException()
        };
    }

    private static int AskInt(string message)
    {
        return int.Parse(Debug.AskString(message));
    }
}