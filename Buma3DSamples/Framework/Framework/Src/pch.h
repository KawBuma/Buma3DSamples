#ifndef PCH_H
#define PCH_H

#include <memory> 
#include <array> 
#include <vector> 
#include <map> 
#include <unordered_map> 
#include <set> 
#include <unordered_set> 
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
#include <mutex>

#include <spdlog/spdlog.h>

#define BUMA_ASSERT(x) assert(x)

#define BMR_IS_FAILED(x) (x >= buma3d::BMRESULT_FAILED) 
#define BMR_IS_SUCCEEDED(x) (x < buma3d::BMRESULT_FAILED)
#define BMR_ASSERT_IF_FAILED(x) BUMA_ASSERT(BMR_IS_SUCCEEDED(x) && #x) 
#define BMR_RET_IF_FAILED(x) if (BMR_IS_FAILED(x)) { BUMA_ASSERT(false && #x); return false; }
#define BUMA_RET_IF_FAILED(x) if (!(x)) { BUMA_ASSERT(false && #x); return false; }

#define NOMINMAX

#include "Framework.h"

#include "Resource.h"
#include "ResourceImpl.h"
#include "ResourceBufferImpl.h"
#include "ResourceTextureImpl.h"

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

#endif //PCH_H
