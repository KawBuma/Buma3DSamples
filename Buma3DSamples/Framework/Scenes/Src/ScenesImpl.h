#pragma once

namespace buma
{
namespace scne
{

class Scene;

class Scenes : public IScenes
{
public:
    Scenes();
    ~Scenes();

    static Scenes* CreateScenes();
    static void DestroyScenes(IScenes* _scenes);

    uint32_t                    GetScenesSize() override;
    IScene*                     GetScenes(uint32_t _index) override;

    IScene*                     CreateScene() override;
    INode*                      CreateNode();
    IValueComponent*            CreateValueComponent() override;
    ITransformComponent*        CreateTransformComponent() override;
    IAABBComponent*             CreateAABBComponent() override;
    ICameraComponent*           CreateCameraComponent() override;
    ILightComponent*            CreateLightComponent() override;
    ILightPropertyComponent*    CreateLightPropertyComponent() override;
    IMaterialComponent*         CreateMaterialComponent() override;
    IMaterialPropertyComponent* CreateMaterialPropertyComponent() override;
    ITextureComponent*          CreateTextureComponent() override;
    ISamplerComponent*          CreateSamplerComponent() override;
    IBufferComponent*           CreateBufferComponent() override;
    IBufferViewComponent*       CreateBufferViewComponent() override;
    IIndexLayoutComponent*      CreateIndexLayoutComponent() override;
    IVertexLayoutComponent*     CreateVertexLayoutComponent() override;
    IVertexElementComponent*    CreateVertexElementComponent() override;
    IIndexBufferComponent*      CreateIndexBufferComponent() override;
    IVertexBufferComponent*     CreateVertexBufferComponent() override;
    ISubmeshComponent*          CreateSubmeshComponent() override;
    IMeshComponent*             CreateMeshComponent() override;

private:
    std::vector<ScopedRef<Scene>> scenes;

};


}// namespace scne
}// namespace buma
