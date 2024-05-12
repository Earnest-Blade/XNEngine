using System.Collections.Generic;
using System.Linq;
using System.Windows.Controls;
using xne.tools.assets;

namespace xne.tools.editor
{
    public partial class AssetController : UserControl
    {
        public AssetController()
        {
            InitializeComponent();
            
            Update(null);
        }

        public void Update(Scene? scene)
        {
            if (scene == null) return;
            if (!AssetContext.GetInstance().HasImporter) return;
            
            _ShaderViewer.ItemsSource = scene.Materials.Select(shader => shader.Shader).ToList()!;
            _TexturesViewer.ItemsSource = scene.Textures;
        }

        public void Push(Scene? scene)
        {
            
        }
    }
}