#pragma once

#define B3D_BUILD_D3D12
//#define B3D_BUILD_VULKAN

#include "StepTimer.h"
#include "Utils.h"
#include "DeviceResources.h"
#include "Platform.h"
#include "Window.h"
#include "Application.h"

#include "PCInputs.h"

#ifdef B3D_BUILD_D3D12
namespace buma
{
constexpr bool B3D_IS_D3D12  = true;
constexpr bool B3D_IS_VULKAN = false;
}// namespace buma


#elif defined B3D_BUILD_VULKAN
namespace buma
{
constexpr bool B3D_IS_D3D12 = false;
constexpr bool B3D_IS_VULKAN = true;
}// namespace buma


#else
#error B3D_BUILD_* is not specified 
#endif


#include "DirectXColors.h"
#include "DirectXCollision.h"
#include "DirectXMath.h"
#include "SimpleMath.h"

