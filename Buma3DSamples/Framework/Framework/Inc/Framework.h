#pragma once
#include <new>
#include <memory> 
#include <array> 
#include <vector>
#include <set>
#include <unordered_set>
#include <mutex>
#include <utility>
#include <algorithm>
#include <map> 
#include <set> 
#include <unordered_map> 
#include <unordered_set> 
#include <iomanip> 
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <sstream>
#include <iostream>
#include <fstream>

#include "PCInputs.h"

#include "LazyDelegate.h"
#include "StepTimer.h"
#include "Logger.h"

#include "Buma3D.h"
#include "Util/Buma3DPtr.h"
#include "B3DInit.h"

#include "FormatUtils.h"
#include "Utils.h"

#include "ResourceHeapProperties.h"
#include "VariableSizeAllocationsManager.h"
#include "ResourceHeapAllocator.h"

#include "TextureLoads.h"

#include "DeviceResources.h"
#include "SwapChain.h"
#include "Platform.h"
#include "Window.h"
#include "Application.h"

#include "Resource.h"
#include "ResourceBuffer.h"
#include "ResourceTexture.h"
#include "ResourceCreate.h"

#include "ShaderLoader.h"
#include "ShaderReflection.h"

#include "StagingBufferPool.h"
#include "DeferredContext.h"
#include "ImmediateContext.h"

namespace buma
{
PlatformBase* CreatePlatform();
void          DestroyPlatform(PlatformBase* _platform);
}

