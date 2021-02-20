#include "pch.h"
#include "Scenes.h"
#include "ScenesLoaderGltf.h"

#include "GLTFSDK/GLTF.h"
#include "GLTFSDK/GLTFResourceReader.h"
#include "GLTFSDK/GLBResourceReader.h"
#include "GLTFSDK/Deserialize.h"
#include "GLTFSDK/MeshPrimitiveUtils.h"
#pragma comment (lib, "GLTFSDK.lib")

namespace fsys = std::filesystem;

namespace gltf    = Microsoft::glTF;
namespace meshutl = Microsoft::glTF::MeshPrimitiveUtils;

namespace buma
{
namespace scne
{

namespace /*anonymous*/
{

class StreamReader : public Microsoft::glTF::IStreamReader
{
public:
    StreamReader(std::filesystem::path _path_base)
        : path_base(std::move(_path_base))
    {
        assert(path_base.has_root_path());
    }

    // Resolves the relative URIs of any external resources declared in the glTF manifest
    std::shared_ptr<std::istream> GetInputStream(const std::string& _filename) const
    {
        // 有効なストリームを構築するには：
        // 1.   filename引数はUTF-8としてエンコードされるため
        //      filesystem::u8pathを使用してパスインスタンスを正しく構築します
        // 
        // 2.   path_baseと指定されたファイル名パスを連結して絶対パスを生成します
        //      filesystem::operator/ は必要に応じてプラットフォームの優先ディレクトリ区切り文字を使用します
        //    
        // 3.   常にバイナリモードでファイルストリームを開きます
        //      glTF SDKはテキストエンコーディングの問題を処理します
        auto stream_path = path_base / fsys::u8path(_filename);
        auto stream = std::make_shared<std::ifstream>(stream_path, std::ios_base::binary);

        // ストリームにエラーがなく、I/O操作の準備ができているかどうかを確認します
        if (stream && (*stream))
            return stream;
        else
            throw std::runtime_error("Unable to create a valid input stream for uri: " + _filename);
    }

private:
    std::filesystem::path path_base;

};

bool ValidateFilePath(std::filesystem::path& path)
{
    if (path.is_relative())
    {
        auto path_current = fsys::current_path();

        // コマンドライン引数を現在のパスに追加して相対パスを絶対パスに変換します
        path_current /= path;
        path_current.swap(path);
    }

    if (!path.has_filename())
    {
        assert(false && "Command line argument path has no filename");
        return false;
    }

    if (!path.has_extension())
    {
        assert(false && "Command line argument path has no filename extension");
        return false;
    }

    return true;
}

template<typename T, typename U>
T& GetFromKey(std::vector<T>& _x, const gltf::IndexedContainer<U>& _c, const std::string& _key)
{
    return _x[_c.GetIndex(_key)];
}

}// namespace /*anonymous*/

ScenesLoaderGltf::ScenesLoaderGltf()
    : scenes        {}
    , scene         {}
    , res_reader    {}
    , document      {}
{

}

ScenesLoaderGltf::~ScenesLoaderGltf()
{
    for (auto& i : cameras)     i->Release();
    for (auto& i : textures)    i->Release();
    for (auto& i : samplers)    i->Release();
    for (auto& i : materials)   i->Release();
    for (auto& i : meshes)      i->Release();
}

bool ScenesLoaderGltf::Load(const char* _filename, IScene* _scene, uint32_t _scene_index)
{
    scenes  = _scene->GetScenes();
    scene   = _scene;

    if (!Init(_filename))
        return false;

    auto&& d = *document;

    CreateCameras();
    CreateTextures();
    CreateSamplers();
    CreateMaterials();
    CreateMeshes();

    return CreateScene(_scene, _scene_index);
}

void ScenesLoaderGltf::CreateCameras()
{
    auto&& d = *document;
    cameras.reserve(d.cameras.Size());
    for (auto& i : d.cameras.Elements())
    {
        auto&& cam = cameras.emplace_back(scenes->CreateCameraComponent());
        cam->SetName(i.name.c_str());

        switch (i.projection->GetProjectionType())
        {
        case gltf::ProjectionType::PROJECTION_PERSPECTIVE:
        {
            auto&& p = i.GetPerspective();
            cam->SetAspectRatio(p.zfar.HasValue() ? p.zfar.Get() : glm::radians(90.f));
            cam->SetFovY(p.yfov);
            cam->SetNearZ(p.znear);
            cam->SetFarZ(p.zfar.HasValue() ? p.zfar.Get() : 1.f);
            break;
        }

        case gltf::ProjectionType::PROJECTION_ORTHOGRAPHIC:
        {
            //auto&& o = i.GetOrthographic();
            //o.xmag;
            //o.ymag;
            //o.zfar;
            //o.znear;
            break;
        }

        default:
            assert(false && __FUNCTION__);
            break;
        }
    }
}

void ScenesLoaderGltf::CreateTextures()
{
    auto&& d = *document;
    textures.reserve(d.images.Size());
    for (auto& i : d.images.Elements())
    {
        auto&& tex = textures.emplace_back(scenes->CreateTextureComponent());
        tex->SetName(i.name.c_str());

        assert(!i.uri.empty());
        tex->SetFilename(i.uri.c_str());
    }
}

void ScenesLoaderGltf::CreateSamplers()
{
    auto&& d = *document;
    samplers.reserve(d.samplers.Size());
    for (auto& i : d.samplers.Elements())
    {
        auto&& sam = samplers.emplace_back(scenes->CreateSamplerComponent());
        sam->SetName(i.name.c_str());

        switch (i.wrapS)
        {
        case gltf::Wrap_REPEAT          : sam->SetSamplerWrapMode(SAMPLER_WRAP_MODE_WRAP);  break;
        case gltf::Wrap_CLAMP_TO_EDGE   : sam->SetSamplerWrapMode(SAMPLER_WRAP_MODE_CLAMP); break;
        case gltf::Wrap_MIRRORED_REPEAT : sam->SetSamplerWrapMode(SAMPLER_WRAP_MODE_WRAP);  break;

        default:
            assert(false && __FUNCTION__);
            break;
        }

        if (i.magFilter.HasValue())
        {
            switch (i.magFilter.Get())
            {
            case gltf::MagFilter_NEAREST : sam->SetSamplerFilterMode(SAMPLER_FILTER_MODE_POINT);  break;
            case gltf::MagFilter_LINEAR  : sam->SetSamplerFilterMode(SAMPLER_FILTER_MODE_LINEAR); break;

            default:
                assert(false && __FUNCTION__);
                break;
            }
        }
        else
        {
            sam->SetSamplerFilterMode(SAMPLER_FILTER_MODE_LINEAR);
        }
    }
}

void ScenesLoaderGltf::CreateMaterials()
{
    auto&& d = *document;
    materials.reserve(d.materials.Size());
    for (auto& i : d.materials.Elements())
    {
        auto&& mat = materials.emplace_back(scenes->CreateMaterialComponent());
        mat->SetName(i.name.c_str());

        switch (i.alphaMode)
        {
            case gltf::ALPHA_UNKNOWN : mat->SetAlphaMode(MATERIAL_ALPHA_MODE_OPAQUE);   break;
            case gltf::ALPHA_OPAQUE  : mat->SetAlphaMode(MATERIAL_ALPHA_MODE_OPAQUE);   break;
            case gltf::ALPHA_BLEND   : mat->SetAlphaMode(MATERIAL_ALPHA_MODE_BLEND);    break;
            case gltf::ALPHA_MASK    : mat->SetAlphaMode(MATERIAL_ALPHA_MODE_MASK);     break;

        default:
            assert(false && __FUNCTION__);
            break;
        }
        mat->SetAlphaCutoff(i.alphaCutoff);
        mat->SetTwoSidedEnabled(i.doubleSided);

        /*
        * base_color
        *   float4 base_color_factor 
        * 
        * metallic_roughness
        *   float metallic_factor 
        *   float roughness_factor 
        * 
        * normal
        *   float normal_scale 
        * 
        * occlusion
        *   float occlusion_strength 
        * 
        * emissive
        *   float3 emissive_factor 
        */

        // base_color
        {
            auto base_color = scenes->CreateMaterialPropertyComponent();
            base_color->SetName("base_color");

            auto base_color_factor = scenes->CreateValueComponent();
            base_color_factor->SetName("base_color_factor");
            base_color_factor->SetValueType(VALUE_TYPE_FLOAT32);
            base_color_factor->Resize(sizeof(i.metallicRoughness.baseColorFactor));
            base_color_factor->SetData(0, sizeof(i.metallicRoughness.baseColorFactor), &i.metallicRoughness.baseColorFactor);
            base_color->AddValueComponent(base_color_factor);

            auto&& ct = d.textures[i.metallicRoughness.baseColorTexture.textureId];
            base_color->SetTextureComponent(GetFromKey(textures, d.images, ct.imageId));
            base_color->SetSamplerComponent(GetFromKey(samplers, d.samplers, ct.samplerId));

            mat->AddCustomProperties(base_color);
        }
        // metallic_roughness
        {
            auto metallic_roughness = scenes->CreateMaterialPropertyComponent();
            metallic_roughness->SetName("metallic_roughness");

            auto metallic_factor = scenes->CreateValueComponent();
            metallic_factor->SetName("metallic_factor");
            metallic_factor->SetValueType(VALUE_TYPE_FLOAT32);
            metallic_factor->Resize(sizeof(i.metallicRoughness.metallicFactor));
            metallic_factor->SetData(0, sizeof(i.metallicRoughness.metallicFactor), &i.metallicRoughness.metallicFactor);
            metallic_roughness->AddValueComponent(metallic_factor);

            auto roughness_factor = scenes->CreateValueComponent();
            roughness_factor->SetName("roughness_factor");
            roughness_factor->SetValueType(VALUE_TYPE_FLOAT32);
            roughness_factor->Resize(sizeof(i.metallicRoughness.roughnessFactor));
            roughness_factor->SetData(0, sizeof(i.metallicRoughness.roughnessFactor), &i.metallicRoughness.roughnessFactor);
            metallic_roughness->AddValueComponent(roughness_factor);

            auto&& mrt = d.textures[i.metallicRoughness.metallicRoughnessTexture.textureId];
            metallic_roughness->SetTextureComponent(GetFromKey(textures, d.images, mrt.imageId));
            metallic_roughness->SetSamplerComponent(GetFromKey(samplers, d.samplers, mrt.samplerId));

            mat->AddCustomProperties(metallic_roughness);
        }
        // normal
        {
            auto normal = scenes->CreateMaterialPropertyComponent();
            normal->SetName("normal");

            auto normal_scale = scenes->CreateValueComponent();
            normal_scale->SetName("normal_scale");
            normal_scale->SetValueType(VALUE_TYPE_FLOAT32);
            normal_scale->Resize(sizeof(i.normalTexture.scale));
            normal_scale->SetData(0, sizeof(i.normalTexture.scale), &i.normalTexture.scale);
            normal->AddValueComponent(normal_scale);

            auto&& nt = d.textures[i.normalTexture.textureId];
            normal->SetTextureComponent(GetFromKey(textures, d.images, nt.imageId));
            normal->SetSamplerComponent(GetFromKey(samplers, d.samplers, nt.samplerId));

            mat->AddCustomProperties(normal);
        }
        // occlusion
        {
            auto occlusion = scenes->CreateMaterialPropertyComponent();
            occlusion->SetName("occlusion");

            auto occlusion_strength = scenes->CreateValueComponent();
            occlusion_strength->SetName("occlusion_strength");
            occlusion_strength->SetValueType(VALUE_TYPE_FLOAT32);
            occlusion_strength->Resize(sizeof(i.occlusionTexture.strength));
            occlusion_strength->SetData(0, sizeof(i.occlusionTexture.strength), &i.occlusionTexture.strength);
            occlusion->AddValueComponent(occlusion_strength);

            auto&& ot = d.textures[i.occlusionTexture.textureId];
            occlusion->SetTextureComponent(GetFromKey(textures, d.images, ot.imageId));
            occlusion->SetSamplerComponent(GetFromKey(samplers, d.samplers, ot.samplerId));

            mat->AddCustomProperties(occlusion);
        }
        // emissive
        {
            auto emissive = scenes->CreateMaterialPropertyComponent();
            emissive->SetName("emissive");

            auto emissive_factor = scenes->CreateValueComponent();
            emissive_factor->SetName("emissive_factor");
            emissive_factor->SetValueType(VALUE_TYPE_FLOAT32);
            emissive_factor->Resize (sizeof(i.emissiveFactor));
            emissive_factor->SetData(0, sizeof(i.emissiveFactor), &i.emissiveFactor);
            emissive->AddValueComponent(emissive_factor);

            auto&& et = d.textures[i.emissiveTexture.textureId];
            emissive->SetTextureComponent(GetFromKey(textures, d.images, et.imageId));
            emissive->SetSamplerComponent(GetFromKey(samplers, d.samplers, et.samplerId));

            mat->AddCustomProperties(emissive);
        }
    }
}

void ScenesLoaderGltf::CreateMeshes()
{
    struct ATTRIBUTE_DATA
    {
        IVertexElementComponent*    element;
        IVertexLayoutComponent*     layout;
        IBufferComponent*           buffer;
        size_t                      offset;

        ATTRIBUTE_DATA(IScenes* _scenes, DATA_FORMAT _format, uint32_t _component_count, const char* _attr_name)
        {
            element = _scenes->CreateVertexElementComponent();
            element->SetName((std::string("element_") + _attr_name).c_str());
            element->SetDataFormat(_format);
            element->SetComponentCount(_component_count);

            layout = _scenes->CreateVertexLayoutComponent();
            layout->SetName((std::string("layout_") + _attr_name).c_str());
            layout->AddVertexElement(element);

            buffer = _scenes->CreateBufferComponent();
            offset = 0;
        }
        ~ATTRIBUTE_DATA()
        {
            element->Release();
            layout ->Release();
            buffer ->Release();
        }
    };
    ATTRIBUTE_DATA POSITION     (scenes, DATA_FORMAT_FLOAT32, 3, "POSITION");
    ATTRIBUTE_DATA NORMAL       (scenes, DATA_FORMAT_FLOAT32, 3, "NORMAL");
    ATTRIBUTE_DATA TANGENT      (scenes, DATA_FORMAT_FLOAT32, 4, "TANGENT");
    ATTRIBUTE_DATA TEXCOORD_0   (scenes, DATA_FORMAT_FLOAT32, 2, "TEXCOORD_0");

    IBufferComponent*       index_buffer = scenes->CreateBufferComponent();;
    size_t                  index_offset = 0;
    IIndexLayoutComponent*  index_layout = scenes->CreateIndexLayoutComponent();
    index_layout->SetName((std::string("index layout")).c_str());
    index_layout->SetDataFormat(DATA_FORMAT_UINT32);


    auto&& d = *document;
    meshes.reserve(d.meshes.Size());
    std::string accessor_id;

    for (auto& i_mesh : d.meshes.Elements())
    {
        auto&& m = meshes.emplace_back(scenes->CreateMeshComponent());
        m->SetName(i_mesh.name.c_str());

        for (auto& i_prim : i_mesh.primitives)
        {
            auto sm = scenes->CreateSubmeshComponent();
            sm->SetName(i_mesh.name.c_str());
            sm->SetMaterial(GetFromKey(materials, d.materials, i_prim.materialId));

            // index
            {
                sm->SetIndexBufferStartOffset(0);

                auto inds = meshutl::GetTriangulatedIndices32(*document, *res_reader, i_prim);
                index_buffer->Resize(index_buffer->GetSize() + inds.size());
                index_buffer->SetData(index_offset, inds.size(), inds.data());

                auto bv = scenes->CreateBufferViewComponent();
                bv->SetName((std::string(index_buffer->GetName()) + "index buffer view").c_str());
                bv->SetBuffer(index_buffer);
                bv->SetOffset(index_offset);
                bv->SetSize(inds.size());

                index_offset += inds.size();

                auto ib = scenes->CreateIndexBufferComponent();
                ib->SetName((std::string(sm->GetName()) + " index buffer").c_str());
                ib->SetBufferView(bv);
                ib->SetIndexLayout(index_layout);
                sm->AddIndexBuffer(ib);
            }

            // vertex buffers
            using GetBufferDataT = std::vector<float>(*)(const gltf::Document&, const gltf::GLTFResourceReader&, const gltf::MeshPrimitive&);
            auto CreateVertex = [&d, &sm, this](GetBufferDataT GetBufferData, const gltf::MeshPrimitive& _prim, const char* _accessor, std::string& _accessor_id, ATTRIBUTE_DATA& _attr_data)
            {
                if (_prim.TryGetAttributeAccessorId(_accessor, _accessor_id))
                {
                    //auto pos = meshutl::GetPositions(d, *res_reader, _prim);
                    auto pos = GetBufferData(d, *res_reader, _prim);
                    _attr_data.buffer->Resize(_attr_data.buffer->GetSize() + pos.size());
                    _attr_data.buffer->SetData(_attr_data.offset, pos.size(), pos.data());

                    auto bv = scenes->CreateBufferViewComponent();
                    bv->SetName((std::string(_attr_data.buffer->GetName()) + _accessor + " buffer view").c_str());
                    bv->SetBuffer(_attr_data.buffer);
                    bv->SetOffset(_attr_data.offset);
                    bv->SetSize(pos.size());

                    _attr_data.offset += pos.size();

                    auto vb = scenes->CreateVertexBufferComponent();
                    vb->SetName((std::string(sm->GetName()) + _accessor + " vertex buffer").c_str());
                    vb->SetBufferView(bv);
                    vb->SetVertexLayout(_attr_data.layout);

                    sm->AddVertexBuffer(vb);
                }
            };
            CreateVertex(meshutl::GetPositions  , i_prim, gltf::ACCESSOR_POSITION  , accessor_id, POSITION);
            CreateVertex(meshutl::GetNormals    , i_prim, gltf::ACCESSOR_NORMAL    , accessor_id, NORMAL);
            CreateVertex(meshutl::GetTangents   , i_prim, gltf::ACCESSOR_TANGENT   , accessor_id, TANGENT);
            CreateVertex(meshutl::GetTexCoords_0, i_prim, gltf::ACCESSOR_TEXCOORD_0, accessor_id, TEXCOORD_0);

            m->AddSubmesh(sm);
        }
    }
}

bool ScenesLoaderGltf::CreateScene(IScene* _scene, uint32_t _scene_index)
{
    if (_scene_index >= document->scenes.Size())
        return false;

    auto&& d = *document;
    auto&& s = d.scenes.Get(_scene_index);

    _scene->SetName(s.name.c_str());

    auto dst_root = _scene->GetRoot();
    for (auto& i : s.nodes)
    {
        auto dst_child = dst_root->AddChild();
        TraverseNodes(dst_child, d.nodes[i]);
    }

    return true;
}

void ScenesLoaderGltf::TraverseNodes(INode* _dst, const gltf::Node& _n)
{
    auto&& d = *document;
    _dst->SetName(_n.name.c_str());

    // transform
    {
        glm::mat4 transform{ 1 };
        switch (_n.GetTransformationType())
        {
        case gltf::TransformationType::TRANSFORMATION_IDENTITY:
            break;
        case gltf::TransformationType::TRANSFORMATION_MATRIX:
            for (uint32_t i = 0; i < 4; i++)
            {
                auto v = &_n.matrix.values[i * 4];
                auto&& dst = transform[i];
                dst.x = v[0];
                dst.y = v[1];
                dst.z = v[2];
                dst.w = v[3];
            }
            break;
        case gltf::TransformationType::TRANSFORMATION_TRS:
            transform =
                glm::translate(glm::vec3(_n.translation.x, _n.translation.y, _n.translation.z)) *
                glm::mat4(glm::quat(_n.rotation.x, _n.rotation.y, _n.rotation.z, _n.rotation.w)) *
                glm::scale(glm::vec3(_n.scale.x, _n.scale.y, _n.scale.z));
            break;
        default:
            assert(false);
            break;
        }
        auto t = scenes->CreateTransformComponent();
        t->SetTransform(transform);
        _dst->AddComponent(t);
    }

    // camera
    if (!_n.cameraId.empty())
    {
        _dst->AddComponent(GetFromKey(cameras, d.cameras, _n.cameraId));
    }

    // mesh
    if (!_n.meshId.empty())
    {
        _dst->AddComponent(GetFromKey(meshes, d.meshes, _n.meshId));
    }

    // children
    for (auto& i : _n.children)
    {
        auto&& cn = d.nodes[i];
        auto dst_child = _dst->AddChild();
        TraverseNodes(dst_child, cn);
    }
}


#pragma region ScenesLoaderGltf::Init

bool ScenesLoaderGltf::Init(const char* _filename)
{
    fsys::path path = _filename;

    if (!ValidateFilePath(path))
        return false;

    // ファイル名なしの絶対パスをストリームリーダーに渡す
    std::shared_ptr<gltf::IStreamReader> stream_reader = std::make_shared<StreamReader>(path.parent_path());

    fsys::path pathfile     = path.filename();
    fsys::path pathfile_ext = pathfile.extension();

    std::string manifest;
    auto MakePathExt = [](const std::string& _ext) { return "." + _ext; };

    if (pathfile_ext == MakePathExt(gltf::GLTF_EXTENSION))
    {        
        CreateResReaderGltf(stream_reader, pathfile, manifest);
    } 
    else if (pathfile_ext == MakePathExt(gltf::GLB_EXTENSION))
    {
        CreateResReaderGlb(stream_reader, pathfile, manifest);
    }
    else
    {
        assert(false && "Command line argument path filename extension must be .gltf or .glb");
        return false;
    }

    if (!DeserializeDocument(manifest))
        return false;

    return true;
}

void ScenesLoaderGltf::CreateResReaderGlb(std::shared_ptr<Microsoft::glTF::IStreamReader>& _stream_reader, std::filesystem::path& _pathfile, std::string& _manifest)
{
    // ファイルの拡張子が .glbの場合 glTF::GLBResourceReaderを作成します
    // このクラスはGLTFResourceReaderから派生し、GLBコンテナーのJSONチャンクからマニフェストを読み取り、
    // バイナリチャンクからリソースデータを読み取るためのサポートを追加します

    auto glbstream = _stream_reader->GetInputStream(_pathfile.u8string()); // UTF-8でエンコードされたファイル名をGetInputStringに渡します
    auto glbres_reader = std::make_shared<gltf::GLBResourceReader>(std::move(_stream_reader), std::move(glbstream));

    // JSONチャンクからマニフェストを取得します
    _manifest = glbres_reader->GetJson();

    res_reader = std::move(glbres_reader);
}

void ScenesLoaderGltf::CreateResReaderGltf(std::shared_ptr<Microsoft::glTF::IStreamReader>& _stream_reader, std::filesystem::path& pathfile, std::string& _manifest)
{
    // ファイルの拡張子が .gltfの場合 glTF::GLTFResourceReaderを作成します

    auto gltfstream = _stream_reader->GetInputStream(pathfile.u8string()); // UTF-8でエンコードされたファイル名をGetInputStringに渡します
    auto gltfres_reader = std::make_shared<gltf::GLTFResourceReader>(std::move(_stream_reader));

    // std::stringstreamを使用してglTFファイルの内容を文字列に読み取ります
    std::stringstream manifest_stream;
    manifest_stream << gltfstream->rdbuf();
    _manifest = manifest_stream.str();

    res_reader = std::move(gltfres_reader);
}

bool ScenesLoaderGltf::DeserializeDocument(const std::string& _manifest)
{
    try
    {
        document = std::make_shared<gltf::Document>(gltf::Deserialize(_manifest));
    }
    catch (const gltf::GLTFException& ex)
    {
        std::string ss;

        ss = "Microsoft::glTF::Deserialize failed: ";
        ss += ex.what();
        assert(false && "Microsoft::glTF::Deserialize failed");
        return false;
    }

    return true;
}

#pragma endregion ScenesLoaderGltf::Init


bool LoadSceneFromGltf(const char* _filename, IScene* _scene, uint32_t _scene_index)
{
    ScenesLoaderGltf loader;
    return loader.Load(_filename, _scene, _scene_index);
}


}// namespace scne
}// namespace buma
