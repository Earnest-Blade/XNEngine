using System;
using System.IO;
using System.Windows;
using xne.tools.assets;

namespace xne.tools.editor;

public partial class ExportWindow : Window
{
    public string Path;
    
    public ExportWindow(string path)
    {
        InitializeComponent();
        AssetContext.GetInstance().CreateExporter(path);
        
        Path = path;
    }

    private void _ExportButtonClick(object sender, RoutedEventArgs e)
    {
        ExportDesc exportDesc = new ExportDesc();
        exportDesc.Name = System.IO.Path.GetFileName(Path);
        exportDesc.Version = AssetContext.VERSION;
        exportDesc.Scene = AssetContext.GetInstance().Importer.Scene;
        
        exportDesc.Format = _FormatCheckBox.IsChecked.Value;

        if (_NoCompressionButton.IsChecked.Value) exportDesc.Compress = 0;
        else if (_DefaultCompressionButton.IsChecked.Value) exportDesc.Compress = 1;
        else exportDesc.Compress = 2;
        Console.WriteLine(exportDesc.Compress);
        
        AssetContext.GetInstance().Exporter.Export(exportDesc);
        
        Close();
    }
}