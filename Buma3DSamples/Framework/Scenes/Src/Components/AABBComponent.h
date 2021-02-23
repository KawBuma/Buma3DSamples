#pragma once

namespace buma
{
namespace scne
{

class AABBComponent : public IAABBComponent, public ScenesObjectImpl
{
protected:
    AABBComponent(Scenes* _scenes);
    ~AABBComponent();

public:
    static AABBComponent* Create(Scenes* _scenes);

    uint32_t           Release()                           override { return ScenesObjectImpl::Release(); }
    const char*        GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void               SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*           GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void               OnDestroy()                         override;
    SCENES_OBJECT_TYPE GetType()                     const override;
    void*              As(SCENES_OBJECT_TYPE _type)        override;

    void UpdateBounds(IMeshComponent* _mesh) override;
    void CalcPositionElementOffset(buma::scne::IVertexLayoutComponent* layout, bool& has_found, uint32_t& element_offset);
    void CalcBounds(buma::scne::IVertexLayoutComponent* layout, buma::scne::IVertexBufferComponent* vb, const uint32_t& element_offset);
    const glm::vec3& GetMinBounds() override;
    const glm::vec3& GetMaxBounds() override;

private:
    glm::vec3 min;
    glm::vec3 max;

};


}// namespace scne
}// namespace buma
