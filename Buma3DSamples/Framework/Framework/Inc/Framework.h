#pragma once
#include <vector>
#include "Buma3D.h"
#include "Util/Buma3DPtr.h"
#include "StepTimer.h"
#include "Utils.h"
#include "DeviceResources.h"
#include "Platform.h"
#include "Window.h"
#include "Application.h"

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

