#pragma once
#include "Buma3D.h"

namespace buma3d
{
namespace hlp
{
namespace init
{

inline FENCE_DESC TimelineFenceDesc(uint64_t _initial_value = 0, FENCE_FLAGS _flags = FENCE_FLAG_NONE)
{
    return FENCE_DESC{ FENCE_TYPE_TIMELINE, _initial_value, _flags };
}

inline FENCE_DESC BinaryFenceDesc(uint64_t _initial_value = 0, FENCE_FLAGS _flags = FENCE_FLAG_NONE)
{
    return FENCE_DESC{ FENCE_TYPE_BINARY_GPU_TO_GPU, _initial_value, _flags };
}

inline FENCE_DESC BinaryCpuFenceDesc(uint64_t _initial_value = 0, FENCE_FLAGS _flags = FENCE_FLAG_NONE)
{
    return FENCE_DESC{ FENCE_TYPE_BINARY_GPU_TO_CPU, _initial_value, _flags };
}

inline SWAP_CHAIN_BUFFER_DESC SwapChainBufferDesc(uint32_t                      _width,
                                                  uint32_t                      _height,
                                                  uint32_t                      _count       = 3,
                                                  const TEXTURE_FORMAT_DESC&    _format_desc = { RESOURCE_FORMAT_UNKNOWN },
                                                  SWAP_CHAIN_BUFFER_FLAGS       _flags       = SWAP_CHAIN_BUFFER_FLAG_COPY_DST)
{
    return SWAP_CHAIN_BUFFER_DESC{ _width, _height, _count, _format_desc, _flags };
}

inline SWAP_CHAIN_DESC SwapChainDesc(ISurface*                      _surface,
                                     COLOR_SPACE                    _color_space,
                                     const SWAP_CHAIN_BUFFER_DESC&  _buffer,
                                     ICommandQueue*const *          _present_queues,
                                     uint32_t                       _num_present_queues = 1,
                                     ROTATION_MODE                  _pre_roration       = ROTATION_MODE_IDENTITY,
                                     SWAP_CHAIN_ALPHA_MODE          _alpha_mode         = SWAP_CHAIN_ALPHA_MODE_DEFAULT,
                                     SWAP_CHAIN_FLAGS               _flags              = SWAP_CHAIN_FLAG_ALLOW_DISCARD_AFTER_PRESENT)
{
    return SWAP_CHAIN_DESC{ _surface, _color_space, _pre_roration, _buffer, _alpha_mode, _flags, _num_present_queues, _present_queues };
}


inline constexpr BUFFER_USAGE_FLAGS BUF_COPYABLE_FLAGS  = BUFFER_USAGE_FLAG_COPY_DST | BUFFER_USAGE_FLAG_COPY_SRC;
inline constexpr BUFFER_USAGE_FLAGS BUF_CBV_FLAGS       = BUFFER_USAGE_FLAG_CONSTANT_BUFFER | BUF_COPYABLE_FLAGS;
inline constexpr BUFFER_USAGE_FLAGS BUF_SRV_FLAGS       = BUFFER_USAGE_FLAG_SHADER_RESOURCE_BUFFER | BUF_COPYABLE_FLAGS;
inline constexpr BUFFER_USAGE_FLAGS BUF_UAV_FLAGS       = BUFFER_USAGE_FLAG_UNORDERED_ACCESS_BUFFER | BUF_COPYABLE_FLAGS;

inline constexpr TEXTURE_USAGE_FLAGS TEX_COPYABLE_FLAGS    = TEXTURE_USAGE_FLAG_COPY_DST | TEXTURE_USAGE_FLAG_COPY_SRC;
inline constexpr TEXTURE_USAGE_FLAGS TEX_STATIC_SRV_FLAGS   = TEXTURE_USAGE_FLAG_SHADER_RESOURCE | TEXTURE_USAGE_FLAG_COPY_DST;
inline constexpr TEXTURE_USAGE_FLAGS TEX_COPYABLE_SRV_FLAGS = TEXTURE_USAGE_FLAG_SHADER_RESOURCE          | TEX_COPYABLE_FLAGS;
inline constexpr TEXTURE_USAGE_FLAGS TEX_UAV_FLAGS          = TEXTURE_USAGE_FLAG_UNORDERED_ACCESS         | TEX_COPYABLE_FLAGS;
inline constexpr TEXTURE_USAGE_FLAGS TEX_DSV_FLAGS          = TEXTURE_USAGE_FLAG_DEPTH_STENCIL_ATTACHMENT | TEX_COPYABLE_FLAGS;
inline constexpr TEXTURE_USAGE_FLAGS TEX_RTV_FLAGS          = TEXTURE_USAGE_FLAG_DEPTH_STENCIL_ATTACHMENT | TEX_COPYABLE_FLAGS;

inline RESOURCE_DESC BufferResourceDesc(uint64_t _size_in_bytes, BUFFER_USAGE_FLAGS _buffer_usage, RESOURCE_FLAGS _flags = RESOURCE_FLAG_NONE)
{
    return RESOURCE_DESC{ RESOURCE_DIMENSION_BUFFER, BUFFER_DESC{ _size_in_bytes, BUFFER_CREATE_FLAG_NONE, _buffer_usage }, _flags };
}

inline RESOURCE_DESC Tex2DResourceDesc(const EXTENT2D& _size, RESOURCE_FORMAT _format, TEXTURE_USAGE_FLAGS  _texture_usage = TEX_STATIC_SRV_FLAGS,
                                       uint32_t _mips = 1, uint32_t _array_size = 1, uint32_t _sample_cont = 1,
                                       TEXTURE_CREATE_FLAGS _texture_flags = TEXTURE_CREATE_FLAG_NONE, RESOURCE_FLAGS _flags = RESOURCE_FLAG_NONE)
{
    auto result = RESOURCE_DESC{ RESOURCE_DIMENSION_TEX2D, {}, _flags };
    result.texture = TEXTURE_DESC{ EXTENT3D{ _size.width, _size.height, 1 },
                                   _array_size,
                                   _mips,
                                   _sample_cont,
                                   TEXTURE_FORMAT_DESC{ _format, 0, nullptr },
                                   TEXTURE_LAYOUT_UNKNOWN,
                                   nullptr,
                                   _texture_flags,
                                   _texture_usage };

    return result;
}

inline COMMITTED_RESOURCE_DESC CommittedResourceDesc(  uint32_t             _heap_index
                                                     , RESOURCE_HEAP_FLAGS  _heap_flags
                                                     , const RESOURCE_DESC& _resource_desc
                                                     , NodeMask             _creation_node_mask     = B3D_DEFAULT_NODE_MASK
                                                     , NodeMask             _visible_node_mask      = B3D_DEFAULT_NODE_MASK
                                                     , uint32_t             _num_bind_node_masks    = 0
                                                     , const NodeMask*      _bind_node_masks        = nullptr)
{
    return COMMITTED_RESOURCE_DESC{ _heap_index, _heap_flags,
                                    _creation_node_mask, _visible_node_mask,
                                    _resource_desc,
                                    _num_bind_node_masks, _bind_node_masks };
}

inline SHADER_RESOURCE_VIEW_DESC ShaderResourceViewDescDescTex2D(
    RESOURCE_FORMAT             _format,
    TEXTURE_ASPECT_FLAGS        _aspect         = TEXTURE_ASPECT_FLAG_COLOR,
    uint32_t                    _mip_slice      = 0,
    uint32_t                    _mip_levels     = 1,
    uint32_t                    _array_slice    = 0,
    uint32_t                    _array_size     = 1,
    SHADER_RESOURCE_VIEW_FLAGS  _flags          = SHADER_RESOURCE_VIEW_FLAG_NONE)
{
    SHADER_RESOURCE_VIEW_DESC result = { VIEW_DESC{ VIEW_TYPE_SHADER_RESOURCE, _format, _array_size > 1 ? VIEW_DIMENSION_TEXTURE_2D_ARRAY : VIEW_DIMENSION_TEXTURE_2D } };
    result.texture = TEXTURE_VIEW{ COMPONENT_MAPPING{ COMPONENT_SWIZZLE_IDENTITY, COMPONENT_SWIZZLE_IDENTITY, COMPONENT_SWIZZLE_IDENTITY, COMPONENT_SWIZZLE_IDENTITY },
                                   SUBRESOURCE_RANGE{ SUBRESOURCE_OFFSET{ _aspect,_mip_slice ,_array_slice }, _array_size, _mip_levels } };
    result.flags = _flags;

    return result;
}

inline RENDER_TARGET_VIEW_DESC RenderTargetViewDescTex2D(
    RESOURCE_FORMAT             _format,
    TEXTURE_ASPECT_FLAGS        _aspect         = TEXTURE_ASPECT_FLAG_COLOR,
    uint32_t                    _mip_slice      = 0,
    uint32_t                    _mip_levels     = 1,
    uint32_t                    _array_slice    = 0,
    uint32_t                    _array_size     = 1,
    RENDER_TARGET_VIEW_FLAGS    _flags          = RENDER_TARGET_VIEW_FLAG_NONE
)
{
    return RENDER_TARGET_VIEW_DESC{ VIEW_DESC{ VIEW_TYPE_RENDER_TARGET, _format, _array_size > 1 ? VIEW_DIMENSION_TEXTURE_2D_ARRAY : VIEW_DIMENSION_TEXTURE_2D },
                                    TEXTURE_VIEW{ COMPONENT_MAPPING{ COMPONENT_SWIZZLE_IDENTITY, COMPONENT_SWIZZLE_IDENTITY, COMPONENT_SWIZZLE_IDENTITY, COMPONENT_SWIZZLE_IDENTITY },
                                    SUBRESOURCE_RANGE{ SUBRESOURCE_OFFSET{ _aspect,_mip_slice ,_array_slice }, _array_size, _mip_levels } }, _flags };
}


}// namespace init
}// namespace hlp
}// namespace buma3d
