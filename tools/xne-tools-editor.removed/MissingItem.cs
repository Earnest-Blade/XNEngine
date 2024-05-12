using System;
using System.IO;
using System.Reflection;
using System.Text;
using xne.tools.assets;

namespace xne.tools.editor
{
    public unsafe class MissingItem
    {
        public string Name => _name;
        public string Value => "Open File";
        
        private string _name;
        private IRaw _value;

        public MissingItem(string name, IRaw dest)
        {
            _name = name;
            _value = dest;
        }

        public void OpenFile(string path)
        {
            _value.Raw = File.ReadAllText(path);
            Console.WriteLine(_value.Raw);
        }
    }
}