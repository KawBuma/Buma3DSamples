#pragma once

namespace buma
{
namespace scne
{

class SubmeshComponent : public ISubmeshComponent, public ScenesObjectImpl
{
protected:
    SubmeshComponent(Scenes* _scenes);
    ~SubmeshComponent();

public:
    static SubmeshComponent* Create(Scenes* _scenes);

    uint32_t                Release()                           override { return ScenesObjectImpl::Release(); }
    const char*             GetName()                     const override { return ScenesObjectImpl::GetName(); }
    void                    SetName(const char* _name)          override { ScenesObjectImpl::SetName(_name); }
    IScenes*                GetScenes()                         override { return ScenesObjectImpl::GetScenes(); }
    void                    OnDestroy()                         override;
    SCENES_OBJECT_TYPE      GetType()                     const override;
    void*                   As(SCENES_OBJECT_TYPE _type)        override;

    void                    SetMaterial                 (IMaterialComponent* _material) override;
    void                    SetIndexBufferStartOffset   (uint32_t _offset) override;
    void                    AddVertexBuffer             (IVertexBufferComponent* _vertex_buffer) override;
    void                    AddIndexBuffer              (IIndexBufferComponent* _index_buffer) override;
    void                    ReleaseVertexBuffer         (IVertexBufferComponent* _vertex_buffer) override;
    void                    ReleaseIndexBuffer          (IIndexBufferComponent* _index_buffer) override;

    IMaterialComponent*     GetMaterial() override;
    uint32_t                GetIndexBufferStartOffset() override;
    uint32_t                GetVertexBuffersSize() override;
    uint32_t                GetIndexBuffersSize() override;
    IVertexBufferComponent* GetVertexBuffers(uint32_t _index) override;
    IIndexBufferComponent*  GetIndexBuffers(uint32_t _index) override;

private:
    ScopedRef<MaterialComponent>                    material;
    uint32_t                                        index_buffer_start_offset;
    std::vector<ScopedRef<VertexBufferComponent>>   vertex_buffers;
    std::vector<ScopedRef<IndexBufferComponent>>    index_buffers;

};


}// namespace scne
}// namespace buma
