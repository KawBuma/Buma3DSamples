#include "pch.h"
#include "ResourceImpl.h"
#include "ResourceBufferImpl.h"

namespace buma
{
namespace res
{

ResourceBufferImpl::ResourceBufferImpl(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type)
    : ResourceImpl(_dr, _create_type)
    , mapped_data   {}
    , mapped_range  {}
{

}

ResourceBufferImpl::~ResourceBufferImpl()
{

}

std::unique_ptr<ResourceBufferImpl> ResourceBufferImpl::Create(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type
                                                               , const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags)
{
    auto result = std::make_unique<ResourceBufferImpl>(_dr, _create_type);
    if (!result->Init(_desc, _heap_flags, _deny_heap_flags))
        return nullptr;

    return result;
}

bool ResourceBufferImpl::Init(const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags)
{
    auto&& d = dr->GetDevice();
    auto bmr = d->CreatePlacedResource(_desc, &resource);
    if (util::IsFailed(bmr))
        return false;

    if (!AllocateHeap(_heap_flags, _deny_heap_flags))
        return false;

    if (!Bind())
        return false;

    if (_heap_flags & (buma3d::hlp::init::HEAP_HOST_VISIBLE_FLAGS))
    {
        bmr = heap_allocation.heap->GetMappedData(&mapped_range, &mapped_data);
        if (util::IsFailed(bmr))
            return false;

        mapped_data         = static_cast<uint8_t*>(mapped_data) + heap_allocation.aligned_offset;
        mapped_range.offset = heap_allocation.aligned_offset;
        mapped_range.size   = heap_allocation.aligned_size;
    }

    return true;
}

void* ResourceBufferImpl::GetMppedData()
{
    return mapped_data;
}

const buma3d::MAPPED_RANGE* ResourceBufferImpl::GetMppedRange() const
{
    return (mapped_data ? &mapped_range : nullptr);
}

void ResourceBufferImpl::Flush(const buma3d::MAPPED_RANGE* _range)
{
    if (!mapped_data)
        return;

    auto bmr = heap_allocation.heap->FlushMappedRanges(1, &mapped_range);
    BMR_ASSERT_IF_FAILED(bmr);
}

void ResourceBufferImpl::Invalidate(const buma3d::MAPPED_RANGE* _range)
{
    if (!mapped_data)
        return;

    auto bmr = heap_allocation.heap->InvalidateMappedRanges(1, &mapped_range);
    BMR_ASSERT_IF_FAILED(bmr);
}


}// namespace res
}// namespace buma
