using System.IO.Compression;
using System.Text;
using Newtonsoft.Json;
using zlib;

namespace xne.assp
{
    public class Exporter : IDisposable
    {
        private Stream _stream;

        public Exporter(string path)
        {
            if(File.Exists(path)) File.Delete(path);
            _stream = new FileStream(path, FileMode.CreateNew);
        }

        public void Export(ExportDesc desc)
        {
            Asset asset = new Asset(desc.Name, desc.Version, desc.Scene);

            JsonSerializerSettings settings = new JsonSerializerSettings();
            if (desc.Format) settings.Formatting = Formatting.Indented;
            
            string value = JsonConvert.SerializeObject(new
            {
                asset
            }, settings);
            byte[] bytes;

            _stream.Position = 0;

            if (desc.Compress)
            {
                CompressData(Encoding.UTF8.GetBytes(value), out bytes);
                _stream.WriteByte(1);
            }
            else
            {
                bytes = Encoding.UTF8.GetBytes(value);
                _stream.WriteByte(0);
            }
            
            _stream.Write(bytes, 0, bytes.Length);
        }
        
        public void Dispose()
        {
            _stream.Dispose();
        }
        
        private static void CompressData(byte[] inData, out byte[] outData)
        {
            using MemoryStream outMemoryStream = new MemoryStream();
            using ZOutputStream outZStream = new ZOutputStream(outMemoryStream, zlibConst.Z_BEST_COMPRESSION);
            using (Stream inMemoryStream = new MemoryStream(inData))
            {
                CopyStream(inMemoryStream, outZStream);
                outZStream.finish();
                outData = outMemoryStream.ToArray();
            }
        }
        
        public static void CopyStream(Stream input, Stream output)
        {
            byte[] buffer = new byte[2048];
            int len;
            while ((len = input.Read(buffer, 0, 2048)) > 0)
            {
                output.Write(buffer, 0, len);
            }
            output.Flush();
        }

        public long GetStreamSize()
        {
            long curr = _stream.Position;
            _stream.Seek(0, SeekOrigin.End);
            long size = _stream.Position;
            _stream.Seek(curr, SeekOrigin.Begin);
            return size;
        }
    }
}