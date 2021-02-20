#pragma once

namespace Microsoft
{
namespace glTF
{
class GLTFResourceReader;
class Document;
class IStreamReader;
struct Node;
}
}


namespace buma
{
namespace scne
{


class ScenesLoaderGltf
{
public:
    ScenesLoaderGltf();
    ~ScenesLoaderGltf();

    bool Load(const char* _filename, IScene* _scene, uint32_t _scene_index);
    bool Init(const char* _filename);
    void CreateResReaderGlb(std::shared_ptr<Microsoft::glTF::IStreamReader>& _stream_reader, std::filesystem::path& _pathfile, std::string& _manifest);
    void CreateResReaderGltf(std::shared_ptr<Microsoft::glTF::IStreamReader>& _stream_reader, std::filesystem::path& _pathfile, std::string& _manifest);
    bool DeserializeDocument(const std::string& _manifest);

private:
    void CreateCameras();
    void CreateTextures();
    void CreateSamplers();
    void CreateMaterials();
    void CreateMeshes();
    bool CreateScene(IScene* _scene, uint32_t _scene_index);
    void TraverseNodes(INode* _dst, const Microsoft::glTF::Node& _n);


private:
    IScenes*                                                scenes;
    IScene*                                                 scene;
    std::shared_ptr<Microsoft::glTF::GLTFResourceReader>    res_reader;
    std::shared_ptr<Microsoft::glTF::Document>              document;

    std::vector<ICameraComponent*>      cameras;
    std::vector<ITextureComponent*>     textures;
    std::vector<ISamplerComponent*>     samplers;
    std::vector<IMaterialComponent*>    materials;
    std::vector<IMeshComponent*>        meshes;

};


}// namespace scne
}// namespace buma
