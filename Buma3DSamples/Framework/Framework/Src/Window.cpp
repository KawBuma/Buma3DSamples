#include "pch.h"
#include "Window.h"

namespace buma
{

WindowBase::WindowBase()
    : surface           {}
    , swapchain         {}
    , back_buffers      {}
    , back_buffer_index {}
{

}

WindowBase::~WindowBase()
{

}

uint32_t WindowBase::AcquireNextBuffer(const buma3d::SWAP_CHAIN_ACQUIRE_NEXT_BUFFER_INFO& _info)
{
    uint32_t next_buffer_index = 0;
    auto bmr = swapchain->AcquireNextBuffer(_info, &next_buffer_index);
    if (bmr != buma3d::BMRESULT_SUCCEED)
        return ~0u;

    back_buffer_index = next_buffer_index;
    return back_buffer_index;
}

bool WindowBase::Present(const buma3d::SWAP_CHAIN_PRESENT_INFO& _info)
{
    auto bmr = swapchain->Present(_info);
    return bmr == buma3d::BMRESULT_SUCCEED ? true : false;
}


}// namespace buma
