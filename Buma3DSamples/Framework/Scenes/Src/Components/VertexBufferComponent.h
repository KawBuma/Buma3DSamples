#pragma once

namespace buma
{
namespace scne
{

class VertexBufferComponent : public IVertexBufferComponent, public ScenesObjectImpl
{
protected:
    VertexBufferComponent(Scenes* _scenes);
    ~VertexBufferComponent();

public:
    static VertexBufferComponent* Create(Scenes* _scenes);

    uint32_t                Release()                           override { return ScenesObjectImpl::Release(); }
    const char*             GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                    SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                    OnDestroy()                         override;
    SCENES_OBJECT_TYPE      GetType()                     const override;
    void*                   As(SCENES_OBJECT_TYPE _type)        override;

    void                    SetVertexLayout(IVertexLayoutComponent* _vertex_layout) override;
    void                    SetBufferView(IBufferViewComponent* _buffer_view) override;

    IVertexLayoutComponent* GetVertexLayout() override;
    IBufferViewComponent*   GetBufferView() override;

private:
    ScopedRef<VertexLayoutComponent>    vertex_layout;
    ScopedRef<BufferViewComponent>      buffer_view;

};


}// namespace scne
}// namespace buma
