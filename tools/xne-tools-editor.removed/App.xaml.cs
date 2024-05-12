using System;
using System.Collections.Generic;
using System.Configuration;
using System.Data;
using System.Linq;
using System.Threading.Tasks;
using System.Windows;
using xne.tools.assets;

namespace xne.tools.editor
{
    /// <summary>
    /// Interaction logic for App.xaml
    /// </summary>
    public partial class App : Application
    {
        private void ApplicationStartup(object sender, StartupEventArgs e)
        {
            MainWindow window = new MainWindow();
            AssetContext.GetInstance();

            window.Show();
        }
    }
}