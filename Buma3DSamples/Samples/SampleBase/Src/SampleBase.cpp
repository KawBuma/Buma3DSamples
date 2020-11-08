#include "pch.h"
#include "SampleBase.h"

namespace buma
{

SampleAppBase::SampleAppBase()
    : ApplicationBase()
{

}

SampleAppBase::~SampleAppBase()
{

}

SampleAppBase* SampleAppBase::Create()
{
    return new SampleAppBase();
}


}// namespace buma
