using System;
using System.IO;
using System.Windows;

using xne.tools.assets;

namespace xne.tools.editor;

public partial class NewProjectWindow : Window
{
    public Scene? Scene;
    public string Path;
    
    public NewProjectWindow(string path)
    {
        InitializeComponent();
        AssetContext.GetInstance().CreateImporter(path);
        
        Scene = null;
        Path = path;
    }

    private void _ProcessButton_OnClick(object sender, RoutedEventArgs e)
    {
        ImporterDesc importerDesc = new ImporterDesc();
        importerDesc.Name = _ModelName.Text;
        importerDesc.Join = _JoinVerticesCheckBox.IsChecked.Value;
        importerDesc.Optimize = _OptimizeCheckBox.IsChecked.Value;
        importerDesc.Triangulate = _TriangulateCheckBox.IsChecked.Value;

        try
        {
            Scene = AssetContext.GetInstance().Importer.Import(importerDesc);
        }
        catch (Exception exception)
        {
            MessageBox.Show(exception.Message);
        }
        
        Close();
    }
}