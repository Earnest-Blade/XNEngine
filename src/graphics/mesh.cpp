#include "mesh.h"

#define XNE_CORE_FILE
#define XNE_CORE_STRING
#include "core/core.h"

#include <zlib.h>
#include <json-c/json.h>

#include <assert.h>
#include <cstring>
#include <GL/glew.h>

#define XNE_BUFFER_SIZE 255
#define XNE_DEFAULT_ALIGN_SIZE 32

#pragma region mesh

void xne_create_mesh(xne_Mesh_t* mesh, xne_MeshDesc_t desc) {
    assert(mesh);
    assert(desc.vertices_count > 0);
    assert(desc.elements_count > 0);
    assert(desc.vertices);
    assert(desc.elements);
    mesh->vertices = {};
    mesh->elements = {};
    mesh->attrib_count = 0;
    mesh->target = 0;
    mesh->primitive = desc.primitive;

    glGenVertexArrays(1, &mesh->target);
    glBindVertexArray(mesh->target);

    glGenBuffers(1, &mesh->vertices.buffer);
    mesh->vertices.target = XNE_BUFFER_TARGET_ARRAY;
    mesh->vertices.acces = XNE_BUFFER_READ_WRITE;
    mesh->vertices.usage = XNE_BUFFER_STATIC_DRAW;
    
    /* find attrib count */
    xne_VertexAlignDesc_t* align = (xne_VertexAlignDesc_t*) &desc.vertex_align[0];
    while (align->attrib != XNE_VERTEX_ATTRIB_END){        
        align = (xne_VertexAlignDesc_t*) &desc.vertex_align[++mesh->attrib_count];
    }

    mesh->vertices.memory.ptr = NULL;
    mesh->vertices.memory.elemsize = desc.vertex_align[mesh->attrib_count].offset;
    mesh->vertices.memory.size = mesh->vertices.memory.elemsize * desc.vertices_count;

    glBindBuffer(mesh->vertices.target, mesh->vertices.buffer);
    glBufferData(mesh->vertices.target, mesh->vertices.memory.size, desc.vertices, mesh->vertices.usage);

    /* attrib */
    for (size_t i = 0; i < mesh->attrib_count; i++)
    {
        glEnableVertexAttribArray(i);

        int size = 
            desc.vertex_align[i + 1].offset / XNE_GET_SIZE((xne_Types_t)desc.vertex_align[i + 1].format) -
            desc.vertex_align[i].offset / XNE_GET_SIZE((xne_Types_t)desc.vertex_align[i].format);

        glVertexAttribPointer(i, size, desc.vertex_align[i].format, GL_FALSE, mesh->vertices.memory.elemsize, (void*)desc.vertex_align[i].offset);
    }

    /* create element buffer */
    glGenBuffers(1, &mesh->elements.buffer);
    mesh->elements.target = XNE_BUFFER_TARGET_ELEMENT_ARRAY;
    mesh->elements.acces = XNE_BUFFER_READ_WRITE;
    mesh->elements.usage = XNE_BUFFER_STATIC_DRAW;
    mesh->elements.memory.ptr = NULL;
    mesh->elements.memory.elemsize = sizeof(uint32_t);
    mesh->elements.memory.size = sizeof(uint32_t) * desc.elements_count;

    glBindBuffer(mesh->elements.target, mesh->elements.buffer);
    glBufferData(mesh->elements.target, sizeof(uint32_t) * desc.elements_count, desc.elements, mesh->elements.usage);

    for (size_t i = 0; i < mesh->attrib_count; i++)
    {
        glDisableVertexAttribArray(i);
    }

    glBindBuffer(mesh->elements.target, 0);
    glBindBuffer(mesh->elements.target, 0);
    glBindVertexArray(0);    
}

void xne_draw_mesh(xne_Mesh_t* mesh){
    glBindVertexArray(mesh->target);
    glBindBuffer(mesh->vertices.target, mesh->vertices.buffer);
    glBindBuffer(mesh->elements.target, mesh->elements.buffer);

    for (size_t i = 0; i < mesh->attrib_count; i++)
        glEnableVertexAttribArray(i);
    
    glDrawElements(mesh->primitive, mesh->elements.memory.size / mesh->elements.memory.elemsize, XNE_UNSIGNED_INT, NULL);

    for (size_t i = 0; i < mesh->attrib_count; i++)
        glDisableVertexAttribArray(i);
    
    glBindBuffer(mesh->elements.target, 0);
    glBindBuffer(mesh->vertices.target, 0);
    glBindVertexArray(0);
}

void xne_destroy_mesh(xne_Mesh_t* mesh){
    glDeleteBuffers(1, &mesh->elements.buffer);
    glDeleteBuffers(1, &mesh->vertices.buffer);
    glDeleteVertexArrays(1, &mesh->target);
}

#pragma endregion

#pragma region model 

/*
    Decompress a model file readed into a char array by using zlib.
    @param __v0 pointer to the char array to change.
    @param __size pointer to the size of the char array to decompress. The size will be update with the new char array size.
*/
/*static void xne__model_file_inflate(char** __v0, size_t* __size){
    z_stream zstream;
    memset(&zstream, 0, sizeof(z_stream));
    zstream.zalloc = Z_NULL;
    zstream.zfree = Z_NULL;
    zstream.opaque = Z_NULL;
    
    if(inflateInit(&zstream) != Z_OK){
        fprintf(stderr, "Failed to initialize zlib!");
    }

    int ret;
    char* buffer = (char*) malloc(XNE_BUFFER_SIZE);
    size_t bufsize = 0;

    do
    {
        if(bufsize % XNE_BUFFER_SIZE == 0){
            buffer = (char*) realloc(buffer, bufsize + XNE_BUFFER_SIZE);
            if(!buffer){ /* error while reallocate 
                fprintf(stderr, "Error while reallocate buffer when uncompressing model!");
                inflateEnd(&zstream);
                free(buffer);
                return;
            }
        }

        zstream.avail_in = *__size - zstream.total_in;
        zstream.next_in = (Bytef*)*__v0 + zstream.total_in;
        
        zstream.avail_out = XNE_BUFFER_SIZE;
        zstream.next_out = (Bytef*)(buffer + bufsize);

        ret = inflate(&zstream, Z_NO_FLUSH);
        bufsize += XNE_BUFFER_SIZE - zstream.avail_out;
    } while (ret != Z_STREAM_END);
    
    inflateEnd(&zstream);

    *__v0 = (char*) realloc(*__v0, bufsize + 1);
    strncpy(*__v0, buffer, bufsize);
    *__size = bufsize;

    free(buffer);
}

static void xne__create_model_node(json_object* node, xne_Model_t::xne__ModelNode* dest){
    assert(dest);
    
    dest->cntChild = json_object_array_length(json_object_object_get(node, "Childs"));
    dest->cntMesh = json_object_array_length(json_object_object_get(node, "Meshes"));
    dest->childs = (xne_Model_t::xne__ModelNode**) malloc(sizeof(xne_Model_t::xne__ModelNode*) * dest->cntChild);
    dest->meshes = (uint32_t*) malloc(sizeof(uint32_t) * dest->cntMesh);
    dest->material = json_object_get_int(json_object_object_get(node, "Material"));

    dest->name = (char*) malloc(json_object_get_string_len(json_object_object_get(node, "Name")) + 1);
    strcpy(dest->name, json_object_get_string(json_object_object_get(node, "Name")));

    const json_object* childs = json_object_object_get(node, "Childs");
    const json_object* meshes = json_object_object_get(node, "Meshes");
    json_object* jobj;

    for (size_t i = 0; i < dest->cntChild; i++)
    {
        jobj = json_object_array_get_idx(childs, i);
        dest->childs[i] = (xne_Model_t::xne__ModelNode*) malloc(sizeof(xne_Model_t::xne__ModelNode));

        xne__create_model_node(jobj, dest->childs[i]);
    }

    for (size_t i = 0; i < dest->cntMesh; i++)
    {
        jobj = json_object_array_get_idx(meshes, i);
        dest->meshes[i] = json_object_get_uint64(jobj);
    }
}

static void xne__draw_model_node(xne_Model_t* model, xne_Model_t::xne__ModelNode* node){
    for (size_t i = 0; i < node->cntChild; i++)
    {
        xne__draw_model_node(model, node->childs[i]);
    }

    fprintf(stdout, "material %i\n", node->material);
    xne_material_enable(model->materials[node->material]);
    
    for (size_t i = 0; i < node->cntMesh; i++)
    {
        xne_draw_mesh(model->meshes[node->meshes[i]]);
    }

    xne_material_disable(NULL);
}

static void xne__destroy_model_node(xne_Model_t::xne__ModelNode* node){
    for (size_t i = 0; i < node->cntChild; i++)
    {
        xne__destroy_model_node(node->childs[i]);
    }
    
    free(node->childs);
    free(node->meshes);
    free(node->name);
}

static void xne__get_json_vector4(json_object* object, float* dest){
    dest[0] = (float)json_object_get_double(json_object_object_get(object, "X"));
    dest[1] = (float)json_object_get_double(json_object_object_get(object, "Y"));
    dest[2] = (float)json_object_get_double(json_object_object_get(object, "Z"));
    dest[3] = (float)json_object_get_double(json_object_object_get(object, "W"));
}

void xne_create_modelf(xne_Model_t* model, FILE* file){
    assert(model);
    assert(file);

    char header = xne_freadw8(file);

    size_t fsize = xne_fsize(file);
    char* fstr = (char*) malloc(fsize * sizeof(char));
    size_t fsize0 = fread(fstr, sizeof(char), fsize, file);
    fstr[fsize0] = '\0';

    const xne_VertexAlignDesc_t defaultVertAlign[] = {
        { XNE_VERTEX_POSITION, XNE_FLOAT, offsetof(xne_Vertex_t, position) },
        { XNE_VERTEX_TEXCOORD, XNE_FLOAT, offsetof(xne_Vertex_t, uv) },
        { XNE_VERTEX_NORMAL, XNE_FLOAT, offsetof(xne_Vertex_t, normal)},
        { XNE_VERTEX_ALIGN_END(XNE_DEFAULT_ALIGN_SIZE) }
    };

    /* Uncompress if necessary
    if(header){
        xne__model_file_inflate(&fstr, &fsize);
        fstr[fsize] = '\0';
    }

    /* parse JSON file
    json_object* root = json_tokener_parse(fstr);
    assert(root);

    json_object* scene = json_object_object_get(json_object_object_get(root, "asset"), "Scene");
    json_object* mesh_array = json_object_object_get(scene, "Meshes");
    model->cntMesh = 0;
    model->meshes = NULL;

    if(json_object_get_type(mesh_array) != json_type_null){
        model->cntMesh = json_object_array_length(mesh_array);
        model->meshes = (xne_Mesh_t**) calloc(model->cntMesh, sizeof(xne_Mesh_t*));

        json_object* mesh, *jvertices, *jindices;

        for (size_t i = 0; i < model->cntMesh; i++)
        {
            mesh = json_object_array_get_idx(mesh_array, i);
            assert(mesh);

            int vcount = json_object_array_length(json_object_object_get(mesh, "Vertices"));
            int ecount = json_object_array_length(json_object_object_get(mesh, "Indices"));

            jvertices = json_object_object_get(mesh, "Vertices");
            jindices = json_object_object_get(mesh, "Indices");

            float* vertices = (float*) malloc(vcount * sizeof(float));
            for (size_t y = 0; y < vcount; y++)
            {
                vertices[y] = json_object_get_double(json_object_array_get_idx(jvertices, y));
            }

            uint32_t* elements = (uint32_t*) malloc(ecount * sizeof(uint32_t));
            for (size_t y = 0; y < ecount; y++)
            {
                elements[y] = json_object_get_uint64(json_object_array_get_idx(jindices, y));
            }

            xne_MeshDesc_t desc = {};
            desc.vertices = vertices;
            desc.vertexAlign = defaultVertAlign;
            desc.elements = elements;
            desc.cntVertices = vcount / (XNE_DEFAULT_ALIGN_SIZE / 4);
            desc.cntElement = ecount;
            desc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;

            model->meshes[i] = (xne_Mesh_t*) malloc(sizeof(xne_Mesh_t));
            xne_create_mesh(model->meshes[i], desc);

            free(vertices);
            free(elements);
        }
    }
    else {
        fprintf(stderr, "File does not have any meshes!\n");
    }

    json_object* texture_array = json_object_object_get(scene, "Textures");
    model->cntTextures = 0;
    model->textures = NULL;
    
    if(json_object_get_type(texture_array) != json_type_null){
        
        json_object* texture;
        model->cntTextures = json_object_array_length(texture_array);
        model->textures = (xne_Texture_t**) calloc(model->cntTextures, sizeof(xne_Texture_t*));

        for (size_t i = 0; i < model->cntTextures; i++)
        {
            texture = json_object_array_get_idx(texture_array, i);
            assert(texture);

            model->textures[i] = (xne_Texture_t*) malloc(sizeof(xne_Texture_t));
            xne_create_texture(model->textures[i], json_object_get_string(json_object_object_get(texture, "Path")), 
                XNE_TEXTURE_FILTER_NEAREST, XNE_TEXTURE_WRAP_REPEAT);
        }
    }
    else {
        fprintf(stderr, "File does not have any textures!\n");
    }

    json_object* material_array = json_object_object_get(scene, "Materials");
    model->cntMaterials = 0;
    model->materials = NULL;

    if(json_object_get_type(material_array) != json_type_null){
        json_object* material, *shader, *shaders, *uniforms;
        model->cntMaterials = json_object_array_length(material_array);
        model->materials = (xne_Material_t**) calloc(model->cntMaterials, sizeof(xne_Material_t*));

        for (size_t i = 0; i < model->cntMaterials; i++)
        {
            material = json_object_array_get_idx(material_array, i);
            shader = json_object_object_get(material, "Shader");
            model->materials[i] = (xne_Material_t*) malloc(sizeof(xne_Material_t));
            memset(model->materials[i], 0, sizeof(xne_Material_t));

            shaders = json_object_object_get(shader, "Shaders");
            xne_ShaderDesc_t* shader_descs = (xne_ShaderDesc_t*) calloc(json_object_array_length(shaders), sizeof(xne_ShaderDesc));
            for (size_t i = 0; i < json_object_array_length(shaders); i++)
            {
                json_object* subshader = json_object_array_get_idx(shaders, i);
                shader_descs[i].type = json_object_get_int(json_object_object_get(subshader, "Type"));
                shader_descs[i].name = json_object_get_string(json_object_object_get(subshader, "Name"));
            }
            
            xne_create_shader(&model->materials[i]->shader, json_object_get_string(json_object_object_get(shader, "Path")), shader_descs);
            free(shader_descs);

            uniforms = json_object_object_get(shader, "Uniforms");
            xne_ShaderUniformDesc_t* uniform_descs = (xne_ShaderUniformDesc_t*) calloc(json_object_array_length(uniforms), sizeof(xne_ShaderUniformDesc_t));
            for (size_t i = 0; i < json_object_array_length(uniforms); i++)
            {
                json_object* subuniform = json_object_array_get_idx(uniforms, i);
                uniform_descs[i].attrib = json_object_get_int(json_object_object_get(subuniform, "Attribute"));
                uniform_descs[i].format = (xne_UniformType_t) json_object_get_int(json_object_object_get(subuniform, "Format"));
                uniform_descs[i].name = json_object_get_string(json_object_object_get(subuniform, "Name"));
            }

            xne_shader_link_uniforms(&model->materials[i]->shader, uniform_descs);
            free(uniform_descs);

            xne__get_json_vector4(json_object_object_get(material, "AmbientColor"), model->materials[i]->ambient_color);
            xne__get_json_vector4(json_object_object_get(material, "DiffuseColor"), model->materials[i]->diffuse_color);

            model->materials[i]->ambient = model->textures[json_object_get_int(json_object_object_get(material, "AmbientTexture"))];
            xne_link_texture(model->materials[i]->ambient, 
                json_object_get_string(json_object_object_get(json_object_array_get_idx(texture_array, json_object_get_int(json_object_object_get(material, "AmbientTexture"))), "Name")), 
                model->materials[i]->shader.target
            );

            model->materials[i]->diffuse = model->textures[json_object_get_int(json_object_object_get(material, "DiffuseTexture"))];
            xne_link_texture(model->materials[i]->diffuse, 
                json_object_get_string(json_object_object_get(json_object_array_get_idx(texture_array, json_object_get_int(json_object_object_get(material, "DiffuseTexture"))), "Name")), 
                model->materials[i]->shader.target
            );
            fprintf(stdout, "Diffuse index: %i, Ambient index: %i\n", model->materials[i]->diffuse->texture_location, model->materials[i]->ambient->texture_location);
        }
    }

    model->root = {};
    xne__create_model_node(json_object_object_get(scene, "Root"), &model->root);

    json_object_put(root);
    free(fstr);
}

void xne_draw_model(xne_Model_t* model){
    // TODO: make rendering relative to node's tree.
    xne__draw_model_node(model, &model->root);
}

void xne_destroy_model(xne_Model_t* model){
    for (size_t i = 0; i < model->cntMaterials; i++)
    {
        xne_destroy_material(model->materials[i]);
        free(model->materials[i]);
    }
    
    for (size_t i = 0; i < model->cntTextures; i++)
    {
        xne_destroy_texture(model->textures[i]);
        free(model->textures[i]);
    }

    for (size_t i = 0; i < model->cntMesh; i++)
    {
        xne_destroy_mesh(model->meshes[i]);
        free(model->meshes[i]);
    }

    free(model->materials);
    free(model->textures);
    free(model->meshes);

    xne__destroy_model_node(&model->root);
}*/

/*xne_Model::xne_Model(FILE* file){
    assert(file);

    char header = xne_freadw8(file); 
    
    size_t fsize = xne_fsize(file);
    char* fstr = (char*) malloc(fsize * sizeof(char));
    size_t fsize0 = fread(fstr, sizeof(char), fsize, file);
    fstr[fsize0] = '\0';

    const xne_VertexAlignDesc_t defaultVertAlign[] = {
        { XNE_VERTEX_POSITION, XNE_FLOAT, offsetof(xne_Vertex_t, position) },
        { XNE_VERTEX_TEXCOORD, XNE_FLOAT, offsetof(xne_Vertex_t, uv) },
        { XNE_VERTEX_NORMAL, XNE_FLOAT, offsetof(xne_Vertex_t, normal)},
        { XNE_VERTEX_ALIGN_END(32) }
    };

    if(header){
        xne__ModelInflate(&fstr, &fsize);
        fstr[fsize] = '\0';
    }

    nlohmann::json asset = nlohmann::json::parse(fstr)["asset"];

    nlohmann::json scene = asset["Scene"];

    m_MeshCount = scene["MeshCount"].template get<int>();
    m_Meshes = (xne_Mesh**) calloc(m_MeshCount, sizeof(xne_Mesh*));

    nlohmann::json meshes = scene["Meshes"];
    for(size_t y = 0; y < m_MeshCount; y++){
        
        nlohmann::json mesh = scene["Meshes"].at(y);

        int verticesCount = (mesh)["VerticesCount"].template get<int>();
        int indicesCount = (mesh)["IndicesCount"].template get<int>();

        float* vertices = (float*) malloc(verticesCount * sizeof(float));
        for (size_t i = 0; i < verticesCount; i++)
            vertices[i] = (mesh)["Vertices"].at(i).template get<float>();
        
        uint32_t* indices = (uint32_t*) malloc(indicesCount * sizeof(uint32_t));
        for (size_t i = 0; i < indicesCount; i++)
            indices[i] = (mesh)["Indices"].at(i).template get<uint32_t>();
        
        xne_MeshDesc desc = {};
        desc.vertices = vertices;
        desc.vertexAlign = defaultVertAlign;
        desc.indices = indices;
        desc.vcount = verticesCount / 8;
        desc.icount = indicesCount;
        desc.primitive = XNE_MESH_PRIMITIVE_TRIANGLE;

        m_Meshes[y] = (xne_Mesh_t*) malloc(sizeof(xne_Mesh_t));
        xne_CreateMesh(m_Meshes[y], desc);

        free(vertices);
        free(indices);
    }

    nlohmann::json root = scene["Root"];
    m_Root = {};
    //xne__ProcessModelNode(root, &m_Root);

    free(fstr);
}

void xne_Model::Draw(){
    for (size_t i = 0; i < m_MeshCount; i++)
    {
        xne_DrawMesh(m_Meshes[i]);
    }
}

xne_Model::~xne_Model(){
    for (size_t i = 0; i < m_MeshCount; i++)
    {
        if(!m_Meshes[i]) continue;
        xne_DestroyMesh(m_Meshes[i]);
    }

    free(m_Meshes);
}*/

#pragma endregion