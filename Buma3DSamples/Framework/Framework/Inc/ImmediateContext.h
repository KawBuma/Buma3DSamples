#pragma once

namespace buma
{
class ImmediateContext
{
public:
    ImmediateContext();
    ~ImmediateContext();

    bool Init(buma3d::util::Ptr<buma3d::ICommandQueue> _que);

private:
    buma3d::util::Ptr<buma3d::IFence>               fence;
    buma3d::util::Ptr<buma3d::ICommandQueue>        queue;
    buma3d::util::Ptr<buma3d::ICommandAllocator>    allocator;
    buma3d::util::Ptr<buma3d::ICommandList>         list;

};


}// namespace buma
