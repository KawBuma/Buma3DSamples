#pragma once

#define B3D_BUILD_D3D12

#ifdef B3D_BUILD_D3D12
constexpr bool B3D_IS_D3D12  = true;
constexpr bool B3D_IS_VULKAN = false;

#include <d3d12.h>
#include <dxgi1_6.h>
#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib,"dxguid.lib")

#ifdef _DEBUG
#pragma comment(lib,"Buma3D_D3D12_Debug.lib")
#else
#pragma comment(lib,"Buma3D_D3D12_Release.lib")
#endif

#elif defined B3D_BUILD_VULKAN
constexpr bool B3D_IS_D3D12  = false;
constexpr bool B3D_IS_VULKAN = true;

#pragma comment(lib,"vulkan-1.lib")

#ifdef _DEBUG
#pragma comment(lib,"Buma3D_Vulkan_Debug.lib")
#else
#pragma comment(lib,"Buma3D_Vulkan_Release.lib")
#endif

#else
#error B3D_BUILD_* is not specified 
#endif


#include "GLTFSDK/GLTF.h"

#include "DirectXColors.h"
#include "DirectXCollision.h"
#include "DirectXMath.h"
#include "SimpleMath.h"



inline std::string GetUUIDString(const uint8_t _uuid[16])
{
#define B3DFW std::setfill('0') << std::setw(2) 
    std::stringstream ss;
    ss << std::hex 
        << B3DFW << (uint32_t)_uuid[0]  << B3DFW << (uint32_t)_uuid[1] << B3DFW << (uint32_t)_uuid[2] << B3DFW << (uint32_t)_uuid[3] << "-"
        << B3DFW << (uint32_t)_uuid[4]  << B3DFW << (uint32_t)_uuid[5] << '-'
        << B3DFW << (uint32_t)_uuid[6]  << B3DFW << (uint32_t)_uuid[7] << '-'
        << B3DFW << (uint32_t)_uuid[8]  << B3DFW << (uint32_t)_uuid[9] << '-'
        << B3DFW << (uint32_t)_uuid[10] << B3DFW << (uint32_t)_uuid[11] << B3DFW << (uint32_t)_uuid[12] << B3DFW << (uint32_t)_uuid[13] << B3DFW << (uint32_t)_uuid[14] << B3DFW << (uint32_t)_uuid[15]
        << std::dec;
    return ss.str();
#undef B3DFW
}

inline std::string GetLUIDString(const uint8_t _luid[8])
{
#define B3DFW std::setfill('0') << std::setw(2) 
    std::stringstream ss;
    ss << std::hex
        << "Low: "    << B3DFW << (uint32_t)_luid[0] << B3DFW << (uint32_t)_luid[1] << B3DFW << (uint32_t)_luid[2] << B3DFW << (uint32_t)_luid[3]
        << ", High: " << B3DFW << (uint32_t)_luid[4] << B3DFW << (uint32_t)_luid[5] << B3DFW << (uint32_t)_luid[6] << B3DFW << (uint32_t)_luid[7]
        << std::dec;
    return ss.str();
#undef B3DFW
}

