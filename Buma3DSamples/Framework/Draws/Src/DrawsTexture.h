#pragma once

namespace buma
{
namespace draws
{

class DrawsTexture : public IDrawsTexture
{
protected:
    DrawsTexture();
    ~DrawsTexture();

    bool Init(DrawsInstance* _ins, const TEXTURE_CREATE_DESC& _desc);

public:
    static bool Create(DrawsInstance* _ins, const TEXTURE_CREATE_DESC& _desc, IDrawsTexture** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

public:
    void        SetName(const char* _name) override;
    const char* GetName() override;

    bool SetData(uint32_t _mip_slice, uint32_t _array_slice, uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data) override;

private:
    std::atomic_uint32_t                            ref_count;
    DrawsInstance*                                  ins;
    TEXTURE_USAGE                                   usage;
    TEXTURE_FORMAT                                  format;
    uint32_t                                        width;
    uint32_t                                        height;
    uint32_t                                        depth;
    uint32_t                                        array_size;
    uint32_t                                        num_mips;
    std::unique_ptr<buma::res::IResourceTexture>    texture;

};


}// namespace buma
}// namespace draws
