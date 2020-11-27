#include "pch.h"
#include "ResourceHeapAllocator.h"

namespace buma
{

ResourceHeapAllocationPage::ResourceHeapAllocationPage(ResourceHeapAllocator& _owner, const RESOURCE_HEAP_PAGE_DESC& _desc)
    : owner         { _owner }
    , desc          { _desc } 
    , allocation    {}
    , heap          {}
{
    buma3d::RESOURCE_HEAP_DESC heap_desc{};
    heap_desc.heap_index            = desc.heap_index;
    heap_desc.size_in_bytes         = desc.page_size;
    heap_desc.alignment             = owner.owner.limits.max_resource_heap_alignment;
    heap_desc.flags                 = buma3d::RESOURCE_HEAP_FLAG_NONE;
    heap_desc.creation_node_mask    = buma3d::B3D_DEFAULT_NODE_MASK;
    heap_desc.visible_node_mask     = buma3d::B3D_DEFAULT_NODE_MASK;
    auto bmr = owner.owner.device->CreateResourceHeap(heap_desc, &heap);
    BMR_ASSERT_IF_FAILED(bmr);

    allocation = std::make_unique<VariableSizeAllocationsManager>(desc.page_size);
}

ResourceHeapAllocationPage::~ResourceHeapAllocationPage()
{
    BUMA_ASSERT(allocation->IsEmpty());
}

RESOURCE_HEAP_ALLOCATION ResourceHeapAllocationPage::Allocate(size_t _size, size_t _alignment)
{
    auto alloc = allocation->Allocate(_size, _alignment);
    RESOURCE_HEAP_ALLOCATION{this, }
    return ;
}

void ResourceHeapAllocationPage::Free(RESOURCE_HEAP_ALLOCATION& _allocation)
{

}



ResourceHeapAllocator::ResourceHeapAllocator(ResourceHeapsAllocator& _owner)
    : owner{ _owner }
{

}

ResourceHeapAllocator::~ResourceHeapAllocator()
{
}



ResourceHeapsAllocator::ResourceHeapsAllocator(buma3d::IDeviceAdapter* _adapter, buma3d::IDevice* _device)
    : device        { _device }
    , allocations   {}
    , limits        {}
{
    _adapter->GetDeviceAdapterLimits(&limits);
}

ResourceHeapsAllocator::~ResourceHeapsAllocator()
{

}


}// namespace buma
