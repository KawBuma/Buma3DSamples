#include "pch.h"
#include "SampleBase.h"

namespace buma
{

SampleAppBase::SampleAppBase()
    : ApplicationBase()
{
    PrepareSettings();
    settings.window_desc.need_window = true;
    settings.window_desc.name = "Buma3DSamples_SampleAppBase";
}

SampleAppBase::~SampleAppBase()
{

}

SampleAppBase* SampleAppBase::Create()
{
    return new SampleAppBase();
}


}// namespace buma
