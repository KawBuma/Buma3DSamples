#include "pch.h"
#include "Platform.h"

namespace buma
{

PlatformBase::PlatformBase()
    : cmd_lines         {}
    , app               {}
    , device_resources  {}
    , window            {}
    , timer             {}
    , is_prepared       {}
{
    

}

PlatformBase::~PlatformBase()
{

}

void PlatformBase::AttachApplication(std::shared_ptr<ApplicationBase> _app)
{
    app = _app;
}


}// namespace buma
