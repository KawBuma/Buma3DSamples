#pragma once

namespace buma
{
namespace scne
{

class VertexLayoutComponent : public IVertexLayoutComponent, public ScenesObjectImpl
{
protected:
    VertexLayoutComponent(Scenes* _scenes);
    ~VertexLayoutComponent();

public:
    static VertexLayoutComponent* Create(Scenes* _scenes);

    uint32_t                    Release()                           override { return ScenesObjectImpl::Release(); }
    const char*                 GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                        SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                    GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                        OnDestroy()                         override;
    SCENES_OBJECT_TYPE          GetType()                     const override;
    void*                       As(SCENES_OBJECT_TYPE _type)        override;

    void                        AddVertexElement    (IVertexElementComponent* _vertex_element) override;
    void                        ReleaseVertexElement(IVertexElementComponent* _vertex_element) override;

    uint32_t                    GetLayoutsStride() override;
    uint32_t                    GetVertexElementsSize() override;
    IVertexElementComponent*    GetVertexElements(uint32_t _index) override;
    IVertexElementComponent*    GetVertexElements(const char* _name) override;

private:
    void UpdateLayoutsStride();

private:
    uint32_t                                        layouts_stride;
    std::vector<ScopedRef<VertexElementComponent>>  vertex_elements;

};


}// namespace scne
}// namespace buma
