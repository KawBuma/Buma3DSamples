#pragma once
#include <vector>

#include "LazyDelegate.h"
#include "StepTimer.h"
#include "Logger.h"

#include "Buma3D.h"
#include "Util/Buma3DPtr.h"
#include "B3DInit.h"

#include "FormatUtils.h"
#include "Utils.h"

#include "VariableSizeAllocationsManager.h"
#include "ResourceHeapAllocator.h"
#include "ResourceHeapProperties.h"

#include "DeviceResources.h"
#include "SwapChain.h"
#include "Platform.h"
#include "Window.h"
#include "Application.h"

#include "ShaderLoader.h"

#include "StagingBufferPool.h"
#include "ImmediateContext.h"

namespace buma
{
PlatformBase* CreatePlatform();
void          DestroyPlatform(PlatformBase* _platform);
}

#include "PCInputs.h"
