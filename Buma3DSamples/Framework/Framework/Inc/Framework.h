#pragma once
#include <vector>
#include "Buma3D.h"
#include "Util/Buma3DPtr.h"
#include "B3DInit.h"
#include "LazyDelegate.h"
#include "StepTimer.h"
#include "Utils.h"
#include "Logger.h"
#include "DeviceResources.h"
#include "SwapChain.h"
#include "Platform.h"
#include "Window.h"
#include "Application.h"

#include "ShaderLoader.h"

namespace buma
{
PlatformBase* CreatePlatform();
void          DestroyPlatform(PlatformBase* _platform);
}

#include "PCInputs.h"

//#include "DirectXColors.h"
//#include "DirectXCollision.h"
//#include "DirectXMath.h"
//#include "SimpleMath.h"

