#pragma once

namespace buma
{
namespace draws
{

class DrawsIndexBuffer : public IDrawsIndexBuffer
{
protected:
    DrawsIndexBuffer();
    ~DrawsIndexBuffer();

    bool Init(DrawsInstance* _ins, const INDEX_BUFFER_CREATE_DESC& _desc);

public:
    static bool Create(DrawsInstance* _ins, const INDEX_BUFFER_CREATE_DESC& _desc, IDrawsIndexBuffer** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

public:
    void        SetName(const char* _name) override;
    const char* GetName() override;

    bool SetData(size_t _dst_offset, size_t _src_size, const void* _src_data) override;

private:
    std::atomic_uint32_t                        ref_count;
    DrawsInstance*                              ins;

    INDEX_BUFFER_TYPE                           type;
    size_t                                      index_count;
    BUFFER_FLAG                                 flags;
    std::unique_ptr<buma::res::IResourceBuffer> buffer;

};


}// namespace buma
}// namespace draws
