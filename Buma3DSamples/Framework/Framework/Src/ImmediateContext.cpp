#include "pch.h"
#include "ImmediateContext.h"

namespace buma
{

ImmediateContext::ImmediateContext()
    : fence     {}
    , queue     {}
    , allocator {}
    , list      {}
{

}

ImmediateContext::~ImmediateContext()
{
    queue->WaitIdle();
}

bool ImmediateContext::Init(buma3d::util::Ptr<buma3d::ICommandQueue> _que)
{
    queue = _que;
    auto d = queue->GetDevice();

    if (d->CreateFence              (buma3d::hlp::init::BinaryCpuFenceDesc(), &fence) >= buma3d::BMRESULT_FAILED)                                           return false;
    if (d->CreateCommandAllocator   (buma3d::hlp::init::CommandAllocatorDesc(queue->GetDesc().type) , &allocator) >= buma3d::BMRESULT_FAILED)               return false;
    if (d->AllocateCommandList      (buma3d::hlp::init::CommandListDesc(allocator.Get(), buma3d::B3D_DEFAULT_NODE_MASK), &list) >= buma3d::BMRESULT_FAILED) return false;

    return true;
}


}// namespace buma
