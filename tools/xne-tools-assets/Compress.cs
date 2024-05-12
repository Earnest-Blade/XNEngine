using zlib;

namespace xne.tools.assets
{
    public enum CompressLevel
    {
        NO_COMPRESSION,
        BASIC_COMPRESSION,
        BEST_COMPRESSION
    }
    
    public static class Compress
    {
        public static byte[] CompressBytes(byte[] value, CompressLevel level)
        {
            int zlevel = level switch
            {
                CompressLevel.NO_COMPRESSION => zlibConst.Z_NO_COMPRESSION,
                CompressLevel.BASIC_COMPRESSION => zlibConst.Z_DEFAULT_COMPRESSION,
                CompressLevel.BEST_COMPRESSION => zlibConst.Z_BEST_COMPRESSION,
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
            byte[] buffer = new byte[2048];
            int len;
            while ((len = a.Read(buffer, 0, 2048)) > 0)
            {
                b.Write(buffer, 0, len);
            }
            
            b.Flush();
        }
        
    }
    
}