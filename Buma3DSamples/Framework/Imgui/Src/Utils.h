#pragma once

#include "Framework.h"
#include "Util/Buma3DPtr.h"
#include "FormatUtils.h"

#include <utility>
#include <cassert>

#define BMR_RET_IF_FAILED(x) if (x >= buma3d::BMRESULT_FAILED) { assert(false && #x); return false; }
#define RET_IF_FAILED(x) if (!(x)) { assert(false && #x); return false; }


namespace buma
{
namespace gui
{

struct RENDER_RESOURCE
{
    using MYIMGUI_CREATE_FLAGS = uint32_t;
    MYIMGUI_CREATE_FLAGS                            flags;
    std::shared_ptr<DeviceResources>                dr;
    buma3d::util::Ptr<buma3d::IDevice>              device;
    buma3d::util::Ptr<buma3d::ICommandQueue>        queue;
    util::PipelineBarrierDesc                       barriers;
    util::SubmitDesc                                submit;
    buma3d::util::Ptr<buma3d::ISamplerView>         sampler;
    std::shared_ptr<buma::res::IResourceTexture>    font_texture;
    buma3d::util::Ptr<buma3d::IShaderResourceView>  font_srv;
    buma3d::util::Ptr<buma3d::IShaderModule>        vs;
    buma3d::util::Ptr<buma3d::IShaderModule>        ps;
    buma3d::util::Ptr<buma3d::IDescriptorSetLayout> sampler_layout;
    buma3d::util::Ptr<buma3d::IDescriptorSetLayout> texture_layout;
    buma3d::util::Ptr<buma3d::IPipelineLayout>      pipeline_layout;
    buma3d::util::Ptr<buma3d::IPipelineState>       pipeline_state_load;
    buma3d::util::Ptr<buma3d::IRenderPass>          render_pass_load;
};


inline void GetSurfaceInfo
(
      size_t                    _width
    , size_t                    _height
    , buma3d::RESOURCE_FORMAT   _fmt
    , size_t*                   _out_num_bytes
    , size_t*                   _out_row_bytes
    , size_t*                   _out_num_rows
)
{
    size_t num_bytes = 0;
    size_t row_bytes = 0;
    size_t num_rows = 0;

    bool is_bc = false;
    bool is_packed = false;
    bool is_planar = false;
    size_t bpe = 0;
    switch (_fmt)
    {
    case buma3d::RESOURCE_FORMAT_BC1_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC1_UNORM:
    case buma3d::RESOURCE_FORMAT_BC1_UNORM_SRGB:
    case buma3d::RESOURCE_FORMAT_BC4_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC4_UNORM:
    case buma3d::RESOURCE_FORMAT_BC4_SNORM:
        is_bc = true;
        bpe = 8;
        break;

    case buma3d::RESOURCE_FORMAT_BC2_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC2_UNORM:
    case buma3d::RESOURCE_FORMAT_BC2_UNORM_SRGB:
    case buma3d::RESOURCE_FORMAT_BC3_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC3_UNORM:
    case buma3d::RESOURCE_FORMAT_BC3_UNORM_SRGB:
    case buma3d::RESOURCE_FORMAT_BC5_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC5_UNORM:
    case buma3d::RESOURCE_FORMAT_BC5_SNORM:
    case buma3d::RESOURCE_FORMAT_BC6H_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC6H_UF16:
    case buma3d::RESOURCE_FORMAT_BC6H_SF16:
    case buma3d::RESOURCE_FORMAT_BC7_TYPELESS:
    case buma3d::RESOURCE_FORMAT_BC7_UNORM:
    case buma3d::RESOURCE_FORMAT_BC7_UNORM_SRGB:
        is_bc = true;
        bpe = 16;
        break;

    default:
        break;
    }

    if (is_bc)
    {
        size_t num_blocks_wide = 0;
        if (_width > 0)
        {
            num_blocks_wide = std::max<size_t>(1, (_width + 3) / 4);
        }
        size_t num_blocks_high = 0;
        if (_height > 0)
        {
            num_blocks_high = std::max<size_t>(1, (_height + 3) / 4);
        }
        row_bytes = num_blocks_wide * bpe;
        num_rows = num_blocks_high;
        num_bytes = row_bytes * num_blocks_high;
    }
    else if (is_packed)
    {
        row_bytes = ((_width + 1) >> 1) * bpe;
        num_rows = _height;
        num_bytes = row_bytes * _height;
    }
    else if (is_planar)
    {
        row_bytes = ((_width + 1) >> 1) * bpe;
        num_bytes = (row_bytes * _height) + ((row_bytes * _height + 1) >> 1);
        num_rows = _height + ((_height + 1) >> 1);
    }
    else
    {
        auto size = util::GetFormatSize(_fmt);
        size_t bpp = (size / util::CalcTexelsPerBlock(_fmt)) * 8;
        row_bytes = (_width * bpp + 7) / 8; // round up to nearest byte
        num_rows = _height;
        num_bytes = row_bytes * _height;
    }

    if (_out_num_bytes)
        *_out_num_bytes = num_bytes;

    if (_out_row_bytes)
        *_out_row_bytes = row_bytes;

    if (_out_num_rows)
        *_out_num_rows = num_rows;
}


}// namespace gui
}// namespace buma
