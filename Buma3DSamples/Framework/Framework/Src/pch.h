#ifndef PCH_H
#define PCH_H

#include <memory> 
#include <array> 
#include <vector> 
#include <map> 
#include <unordered_map> 
#include <list> 
#include <stack> 
#include <queue> 
#include <deque> 
#include <iomanip> 
#include <new>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <utility>
#include <sstream>
#include <iostream>
#include <fstream>
#include <algorithm>

#include <spdlog/spdlog.h>

#include "Buma3D.h"
#include "Util/Buma3DPtr.h"

#define NOMINMAX
//#include "DeviceResources.h"
#include "StepTimer.h"

#include "Utils.h"
#include "DeviceResources.h"

#include "Application.h"
#include "Platform.h"
#include "Window.h"


#include "Framework.h"

#include "GamepadInputs.h"
#include "MouseInput.h"
#include "KeyboardInput.h"
#pragma comment(lib, "Inputs.lib")


#ifdef B3D_BUILD_D3D12
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


#include "WindowWindows.h"
#include "PlatformWindows.h"


#endif //PCH_H
