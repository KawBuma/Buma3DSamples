#pragma once

namespace buma
{
namespace res
{

class ResourceImpl
{
public:
    ResourceImpl(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type)
        : dr                { _dr }
        , create_type       { _create_type }
        , heap_allocation   {}
        , resource          {}
    {}

    ResourceImpl(const IResource&) = delete;

    // ResourceBufferImpl, ResourceTextureImpl が継承します。
    virtual ~ResourceImpl()
    {
        resource.Reset();
        if (heap_allocation)
            dr->GetResourceHeapsAllocator()->Free(heap_allocation);
    }

protected:
    RESOURCE_CREATE_TYPE                        GetResourceCreateTypeImpl()        const { return create_type; }
    const RESOURCE_HEAP_ALLOCATION&             GetAllocationImpl()                const { return heap_allocation; }
    const buma3d::util::Ptr<buma3d::IResource>& GetB3DResourceImpl()               const { return resource; }

    bool AllocateHeap(buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags)
    {
        buma3d::RESOURCE_ALLOCATION_INFO        info{};
        buma3d::RESOURCE_HEAP_ALLOCATION_INFO   heap_info{};
        auto bmr = dr->GetDevice()->GetResourceAllocationInfo(1, resource.GetAddressOf(), &info, &heap_info);
        if (util::IsFailed(bmr))
            return false;

        uint32_t bits = dr->GetResourceHeapProperties()->FindCompatibleHeaps(_heap_flags, _deny_heap_flags);
        if ((bits & heap_info.heap_type_bits) == 0x0)
            return false;

        heap_allocation = dr->GetResourceHeapsAllocator()->Allocate(heap_info.total_size_in_bytes, heap_info.required_alignment, (uint32_t)util::GetFirstBitIndex(bits));
        return heap_allocation;
    }

    bool Bind()
    {
        buma3d::BIND_RESOURCE_HEAP_INFO bi{};
        bi.src_heap            = heap_allocation.heap;
        bi.src_heap_offset     = heap_allocation.aligned_offset;
        bi.num_bind_node_masks = 0;
        bi.bind_node_masks     = nullptr;
        bi.dst_resource        = resource.Get();

        auto bmr = dr->GetDevice()->BindResourceHeaps(1, &bi);
        if (util::IsFailed(bmr))
            return false;

        return true;
    }

protected:
    std::shared_ptr<DeviceResources>        dr;
    RESOURCE_CREATE_TYPE                    create_type;
    RESOURCE_HEAP_ALLOCATION                heap_allocation;
    buma3d::util::Ptr<buma3d::IResource>    resource;

};


}// namespace res
}// namespace buma
