#include "pch.h"
#include "Platform.h"

namespace buma
{

PlatformBase::PlatformBase()
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
