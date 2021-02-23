#pragma once

namespace buma
{
namespace scne
{

class IndexBufferComponent : public IIndexBufferComponent, public ScenesObjectImpl
{
protected:
    IndexBufferComponent(Scenes* _scenes);
    ~IndexBufferComponent();

public:
    static IndexBufferComponent* Create(Scenes* _scenes);

    uint32_t                Release()                           override { return ScenesObjectImpl::Release(); }
    const char*             GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                    SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                    OnDestroy()                         override;
    SCENES_OBJECT_TYPE      GetType()                     const override;
    void*                   As(SCENES_OBJECT_TYPE _type)        override;

    void                    SetIndexLayout(IIndexLayoutComponent* _index_layout) override;
    void                    SetBufferView(IBufferViewComponent* _buffer_view) override;

    IIndexLayoutComponent*  GetIndexLayout() override;
    IBufferViewComponent*   GetBufferView() override;

private:
    ScopedRef<IndexLayoutComponent>     index_layout;
    ScopedRef<BufferViewComponent>      buffer_view;

};


}// namespace scne
}// namespace buma
