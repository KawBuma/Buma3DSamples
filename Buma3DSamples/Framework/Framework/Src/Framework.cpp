#include "pch.h"
#include "Framework.h"

namespace buma
{

PlatformBase* CreatePlatform()
{
    return new PlatformWindows();
}

void DestroyPlatform(PlatformBase* _platform)
{
    delete _platform;
}

}// namespace buma
