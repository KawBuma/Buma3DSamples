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

#define BUMA_ASSERT(x) assert(x)

#define BMR_IS_FAILED(x) (x >= buma3d::BMRESULT_FAILED) 
#define BMR_IS_SUCCEEDED(x) (x < buma3d::BMRESULT_FAILED)
#define BMR_ASSERT_IF_FAILED(x) BUMA_ASSERT((x) && #x) 
#define BMR_RET_IF_FAILED(x) if (x >= buma3d::BMRESULT_FAILED) { BUMA_ASSERT(false && #x); return false; }
#define BUMA_RET_IF_FAILED(x) if (!(x)) { BUMA_ASSERT(false && #x); return false; }

#include "B3DInit.h"

#define NOMINMAX
//#include "DeviceResources.h"
#include "StepTimer.h"

#include "Utils.h"
#include "DeviceResources.h"
#include "SwapChain.h"

#include "LazyDelegate.h"
#include "Logger.h"
#include "Application.h"
#include "Platform.h"
#include "Window.h"

#include "VariableSizeAllocationsManager.h"
#include "ResourceHeapAllocator.h"

#include "Framework.h"

#include "GamepadInputs.h"
#include "MouseInput.h"
#include "KeyboardInput.h"
#pragma comment(lib, "Inputs.lib")

#include "GLTFSDK/GLTF.h"

#include "DirectXColors.h"
#include "DirectXCollision.h"
#include "DirectXMath.h"
#include "SimpleMath.h"


#include "LoggerWindows.h"
#include "WindowWindows.h"
#include "PlatformWindows.h"

#include "ShaderLoader.h"

#include "ImmediateContext.h"


#endif //PCH_H
