#include "pch.h"
#include "DrawsTexture.h"

namespace buma
{
namespace draws
{

DrawsTexture::DrawsTexture()
    : ref_count     { 1 }
    , ins           {}
    , usage         {}
    , format        {}
    , width         {}
    , height        {}
    , depth         {}
    , array_size    {}
    , num_mips      {}
    , texture       {}
    , mip_slice     {}
    , array_slice   {}
    , data_size     {}
    , data          {}
{

}

DrawsTexture::~DrawsTexture()
{
    if (data)
    {
        delete[] data;
        data = nullptr;
    }
}

bool DrawsTexture::Create(DrawsInstance* _ins, const TEXTURE_CREATE_DESC& _desc, IDrawsTexture** _dst)
{
    auto p = new DrawsTexture();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

bool DrawsTexture::Init(DrawsInstance* _ins, const TEXTURE_CREATE_DESC& _desc)
{
    ins = _ins;

    return true;
}

void DrawsTexture::AddRef()
{
    ++ref_count;
}

uint32_t DrawsTexture::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}

void DrawsTexture::SetName(const char* _name)
{
    texture->SetName(_name);
}

const char* DrawsTexture::GetName()
{
    return texture->GetName();
}

bool DrawsTexture::PrepareData(uint32_t _mip_slice, uint32_t _array_slice)
{
    mip_slice     = _mip_slice;
    array_slice   = _array_slice;
    data_size     = 
    data          = new uint8_t[data_size];

    return true;
}

bool DrawsTexture::SetData(uint64_t _src_row_pitch, uint64_t _src_texture_height, size_t _src_size, const void* _src_data)
{

    return true;
}

bool DrawsTexture::SubmitData()
{
    if (!data)
        return false;

    return true;
}


}// namespace buma
}// namespace draws
