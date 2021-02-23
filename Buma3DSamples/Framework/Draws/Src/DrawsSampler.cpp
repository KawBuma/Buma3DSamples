#include "pch.h"
#include "DrawsSampler.h"

namespace buma
{
namespace draws
{


namespace /*anonymous*/
{

buma3d::TEXTURE_SAMPLE_MODE ConvertFilterMode(SAMPLER_FILTER_MODE _mode)
{
    switch (_mode)
    {
    case buma::draws::SAMPLER_FILTER_MODE_POINT  : return buma3d::TEXTURE_SAMPLE_MODE_POINT;
    case buma::draws::SAMPLER_FILTER_MODE_LINEAR : return buma3d::TEXTURE_SAMPLE_MODE_LINEAR;

    default:
        return buma3d::TEXTURE_SAMPLE_MODE(-1);
    }
}

buma3d::TEXTURE_ADDRESS_MODE ConvertWrapMode(SAMPLER_WRAP_MODE _mode)
{
    switch (_mode)
    {
    case buma::draws::SAMPLER_WRAP_MODE_WRAP    : return buma3d::TEXTURE_ADDRESS_MODE_WRAP; 
    case buma::draws::SAMPLER_WRAP_MODE_CLAMP   : return buma3d::TEXTURE_ADDRESS_MODE_CLAMP;
    case buma::draws::SAMPLER_WRAP_MODE_BOARDER : return buma3d::TEXTURE_ADDRESS_MODE_BORDER;

    default:
        return buma3d::TEXTURE_ADDRESS_MODE(-1);
    }
}

buma3d::BORDER_COLOR ConvertBoarderType(SAMPLER_BOARDER_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::SAMPLER_BOARDER_TYPE_TRANSPARENT_BLACK_FLOAT : return buma3d::BORDER_COLOR_TRANSPARENT_BLACK_FLOAT;
    case buma::draws::SAMPLER_BOARDER_TYPE_TRANSPARENT_BLACK_INT   : return buma3d::BORDER_COLOR_TRANSPARENT_BLACK_INT;
    case buma::draws::SAMPLER_BOARDER_TYPE_OPAQUE_BLACK_FLOAT      : return buma3d::BORDER_COLOR_OPAQUE_BLACK_FLOAT;
    case buma::draws::SAMPLER_BOARDER_TYPE_OPAQUE_BLACK_INT        : return buma3d::BORDER_COLOR_OPAQUE_BLACK_INT;
    case buma::draws::SAMPLER_BOARDER_TYPE_OPAQUE_WHITE_FLOAT      : return buma3d::BORDER_COLOR_OPAQUE_WHITE_FLOAT;
    case buma::draws::SAMPLER_BOARDER_TYPE_OPAQUE_WHITE_INT        : return buma3d::BORDER_COLOR_OPAQUE_WHITE_INT;

    default:
        return buma3d::BORDER_COLOR(-1);
    }
}

}//namespace /*anonymous*/


DrawsSampler::DrawsSampler()
    : ref_count         { 1 }
    , ins               {}
    , min_filter        {}
    , mag_filter        {}
    , wrap_mode         {}
    , boarder_type      {}
    , max_anisotropy    {}
    , sampler           {}
{

}

DrawsSampler::~DrawsSampler()
{

}

bool DrawsSampler::Init(DrawsInstance* _ins, const SAMPLER_CREATE_DESC& _desc)
{
    ins = _ins;

    min_filter      = _desc.min_filter;
    mag_filter      = _desc.mag_filter;
    wrap_mode       = _desc.wrap_mode;
    boarder_type    = _desc.boarder_type;
    max_anisotropy  = _desc.max_anisotropy;
    if (!CreateSampler())
        return false;

    return true;
}
bool DrawsSampler::CreateSampler()
{
    buma3d::SAMPLER_DESC sd{};
    sd.filter.mode                       = buma3d::SAMPLER_FILTER_MODE_ANISOTROPHIC;
    sd.filter.reduction_mode             = buma3d::SAMPLER_FILTER_REDUCTION_MODE_STANDARD;
    sd.filter.max_anisotropy             = std::min(max_anisotropy, (uint32_t)ins->GetDR()->GetDeviceAdapterLimits().max_sampler_anisotropy);
    sd.filter.comparison_func            = buma3d::COMPARISON_FUNC_NEVER;
    if ((sd.texture.address.u            = ConvertWrapMode(wrap_mode)) == -1) return false;
    if ((sd.texture.address.v            = ConvertWrapMode(wrap_mode)) == -1) return false;
    if ((sd.texture.address.w            = ConvertWrapMode(wrap_mode)) == -1) return false;
    if ((sd.texture.sample.minification  = ConvertFilterMode(min_filter)) == -1) return false;
    if ((sd.texture.sample.magnification = ConvertFilterMode(mag_filter)) == -1) return false;
    if ((sd.texture.sample.mip           = ConvertFilterMode(min_filter)) == -1) return false;
    sd.mip_lod.min                       = 0.f;
    sd.mip_lod.max                       = FLT_MAX;
    sd.mip_lod.bias                      = 0.f;
    if ((sd.border_color                 = ConvertBoarderType(boarder_type)) == -1) return false;

    auto bmr = ins->GetDR()->GetDevice()->CreateSampler(sd, &sampler);
    if (util::IsFailed(bmr))
        return false;

    return true;
}

bool DrawsSampler::Create(DrawsInstance* _ins, const SAMPLER_CREATE_DESC& _desc, IDrawsSampler** _dst)
{
    auto p = new DrawsSampler();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

void DrawsSampler::AddRef()
{
    ++ref_count;
}

uint32_t DrawsSampler::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}

void DrawsSampler::SetName(const char* _name)
{
    sampler->SetName(_name);
}

const char* DrawsSampler::GetName()
{
    return sampler->GetName();
}


}// namespace buma
}// namespace draws
