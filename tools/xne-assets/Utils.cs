using zlib;

namespace xne.assets;

public static class Utils
{
    public const int XNE_BUFFER_SIZE = 2048;
    
    public const int XNE_NO_COMPRESSION = 0;
    public const int XNE_BASIC_COMPRESSION = 1;
    public const int XNE_BEST_COMPRESSION = 2;
    
    public static byte[] CompressBytes(byte[] value, int level)
    {
        int zlevel = level switch
        {
            XNE_NO_COMPRESSION => zlibConst.Z_NO_COMPRESSION,
            XNE_BASIC_COMPRESSION => zlibConst.Z_DEFAULT_COMPRESSION,
            XNE_BEST_COMPRESSION => zlibConst.Z_BEST_COMPRESSION,
            _ => zlibConst.Z_NO_COMPRESSION
        };
            
        using MemoryStream outStream = new MemoryStream();
        using ZOutputStream zStream = new ZOutputStream(outStream, zlevel);
        using (Stream inStream = new MemoryStream(value))
        {
            CopyStream(inStream, zStream);
            zStream.finish();

            return outStream.ToArray();
        }
    }

    private static void CopyStream(Stream a, Stream b)
    {
        byte[] buffer = new byte[XNE_BUFFER_SIZE];
        int len;
        while ((len = a.Read(buffer, 0, XNE_BUFFER_SIZE)) > 0)
        {
            b.Write(buffer, 0, len);
        }
            
        b.Flush();
    }
}