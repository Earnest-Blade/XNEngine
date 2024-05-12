using Microsoft.Win32;

using System;
using System.Windows;
using System.Windows.Controls;

using xne.tools.assets;

namespace xne.tools.editor
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();
            
            ImporterDesc importerDesc = new ImporterDesc();
            importerDesc.Name = "model";
            importerDesc.Join = true;
            importerDesc.Optimize = true;
            importerDesc.Triangulate = true;

            AssetContext.GetInstance().CreateImporter("E:/Code/XNEngine/tools/cube.dae");
            AssetContext.GetInstance().Importer.Import(importerDesc);
            
            _UpdateClick(null, null);
        }

        private void _NewButtonClick(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Multiselect = false;
            ofd.InitialDirectory = AssetContext.GetInstance().CurrentDirectory;

            if(ofd.ShowDialog() == true)
            {
                NewProjectWindow importWindow = new NewProjectWindow(ofd.FileName);
                importWindow.ShowDialog();
                
                _UpdateClick(sender, e);
            }
        }

        private void _OpenButtonClick(object sender, RoutedEventArgs e)
        {
            OpenFileDialog ofd = new OpenFileDialog();
            ofd.Multiselect = false;
            ofd.InitialDirectory = AssetContext.GetInstance().CurrentDirectory;

            if(ofd.ShowDialog() == true)
            {
                try
                {
                    Importer.LoadFromFile(ofd.FileName);
                }
                catch (Exception exception)
                {
                    MessageBox.Show($"Cannot open {ofd.FileName}!");
                    return;
                }
                
                _UpdateClick(sender, e);
            }
        }
        
        private void _ExportButtonClick(object sender, RoutedEventArgs e)
        {
            SaveFileDialog sfd = new SaveFileDialog();
            if (sfd.ShowDialog() == true)
            {
                ExportWindow exportWindow = new ExportWindow(sfd.FileName);
                exportWindow.ShowDialog();
            }
        }

        private void _ExitButtonClick(object sender, RoutedEventArgs e)
        {
            Environment.Exit(0);
        }

        private void _UpdateClick(object sender, RoutedEventArgs e)
        {
            if (!AssetContext.GetInstance().HasImporter) return;
            if (AssetContext.GetInstance().Importer.Scene == null) return;
            
            Scene target = AssetContext.GetInstance().Importer.Scene;
            _AssetControllerViewer.Push(target);
            
            string json = Exporter.DirectExportAsJson(target);
            _JsonTextBox.Text = json;

            _AssetControllerViewer.Update(target);
            //_JsonHierarchy.Load(AssetContext.GetInstance().Importer.Scene);
        }
    }
}