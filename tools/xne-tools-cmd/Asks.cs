using xne.tools.assets;

namespace xne.tools.cmd;

public static class Asks
{
    internal static bool AskBool(string message)
    {
        string value = Debug.AskString(message).ToLower();
        return value[0] switch
        {
            'y' => true,
            'n' => false,
            _ => throw new ArgumentException()
        };
    }

    internal static int AskInt(string message)
    {
        return int.Parse(Debug.AskString(message));
    }
}