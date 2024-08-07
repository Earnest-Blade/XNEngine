import bpy, os

def xne_get_static_blender_project_name() -> str:
    return bpy.path.basename(bpy.context.blend_data.filepath).split('.')[0]

def xne_get_static_blender_project_dir() -> str:
    return os.path.dirname(bpy.context.blend_data.filepath) + "/"

class xne_export_ui_menu_prop(bpy.types.PropertyGroup):
    cli: bpy.props.StringProperty(
        name="", 
        description="Path To xne-cli.exe",
        default="//Source/extern/xnengine/tools/xne-cli/bin/Release/net7.0/xne-cli.exe",
        maxlen=1024,
        subtype="FILE_PATH"
    )

    path: bpy.props.StringProperty(
        name="", 
        description="Path To Directory",
        default="./",
        maxlen=1024,
        subtype="DIR_PATH"
    )

    compression_level: bpy.props.IntProperty(
        name="",
        description="Compression level (integer)",
        default = 0
    )
    
    triangulate: bpy.props.BoolProperty(
        name="",
        description="Triangulate the imported model",
        default = True
    )

    optimize: bpy.props.BoolProperty(
        name="",
        description="Optimize the imported model",
        default = True
    )

    join_vertices: bpy.props.BoolProperty(
        name="",
        description="Join vertices of the imported model",
        default = True
    )

    generate_normals: bpy.props.BoolProperty(
        name="",
        description="Generate normals for the imported model",
        default = False
    )

    make_readable: bpy.props.BoolProperty(
        name="",
        description="Make the exported model readable",
        default = True
    )

class xne_export_ui_menu_operator_button(bpy.types.Operator):
    bl_idname = "object.simple_operator"
    bl_label = "Export"

    @classmethod
    def poll(cls, context):
        obj = context.active_object
        return (obj is not None and obj.type == 'MESH')
    
    def execute(self, context):
        scene = bpy.context.scene

        cli_path = xne_get_static_blender_project_dir() + scene.xne_prop.cli
        fbx_path = scene.xne_prop.path + xne_get_static_blender_project_name()
        internal_fbx_path = fbx_path + ".fbx"
        xneo_path = scene.xne_prop.path + xne_get_static_blender_project_name() + '.xneo'
        compress_level = scene.xne_prop.compression_level

        bpy.ops.export_scene.fbx(
            filepath=internal_fbx_path
        )

        argv = f" --skip-asks --finput={internal_fbx_path} --foutput={xneo_path} --compression-level={compress_level}"
        if(scene.xne_prop.triangulate):
            argv += " --triangulate"
        if(scene.xne_prop.triangulate):
            argv += " --optimize"
        if(scene.xne_prop.join_vertices):
            argv += " --join-vertices"
        if(scene.xne_prop.generate_normals):
            argv += " --generate-normals"
        if(scene.xne_prop.make_readable):
            argv += " --make-readable"

        print(cli_path + argv)

        os.system(cli_path + argv)

        return {'FINISHED'}

class xne_export_ui_menu(bpy.types.Panel):
    bl_label = "XNEO Exporter"
    bl_idname = "SCENE_PT_layout"
    bl_space_type = "PROPERTIES"
    bl_region_type = "WINDOW"
    bl_context = "render"

    def draw(self, context):
        scene = context.scene
        layout = self.layout

        col = layout.column(align=True)
        col.prop(scene.xne_prop, "cli", text="CLI")

        col = layout.column(align=True)
        col.prop(scene.xne_prop, "path", text="Destination")

        col = layout.column(align=True)
        col.prop(scene.xne_prop, "triangulate", text="Triangulate")
        
        col = layout.column(align=True)
        col.prop(scene.xne_prop, "optimize", text="Optimize")
        
        col = layout.column(align=True)
        col.prop(scene.xne_prop, "join_vertices", text="Join Vertices")

        col = layout.column(align=True)
        col.prop(scene.xne_prop, "generate_normals", text="Generate Normals")

        col = layout.column(align=True)
        col.prop(scene.xne_prop, "make_readable", text="Format JSON")

        col = layout.column(align=True)
        col.prop(scene.xne_prop, "compression_level", text="Compression")

        col = layout.column(align=True)
        col.operator(xne_export_ui_menu_operator_button.bl_idname)

CLASSES = [
    xne_export_ui_menu_prop,
    xne_export_ui_menu_operator_button,
    xne_export_ui_menu
]

def register():
    for c in CLASSES:
        bpy.utils.register_class(c)

    bpy.types.Scene.xne_prop = bpy.props.PointerProperty(type=xne_export_ui_menu_prop)

def unregister():    
    for c in CLASSES:
        bpy.utils.unregister_class(c)

if __name__ == "__main__":
    register()