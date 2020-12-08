#pragma once

namespace buma
{
namespace res
{

class ResourceBufferImpl : public IResourceBuffer, private ResourceImpl
{
public:
    ResourceBufferImpl(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type);
    ResourceBufferImpl(const IResource&) = delete;
    virtual ~ResourceBufferImpl();

    static std::unique_ptr<ResourceBufferImpl> Create(std::shared_ptr<DeviceResources>& _dr, RESOURCE_CREATE_TYPE _create_type
                                                      , const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags);

    RESOURCE_CREATE_TYPE                        IResourceBuffer::GetResourceCreateType() const override { return ResourceImpl::GetResourceCreateTypeImpl(); }
    const RESOURCE_HEAP_ALLOCATION&             IResourceBuffer::GetAllocation()         const override { return ResourceImpl::GetAllocationImpl(); }
    const buma3d::util::Ptr<buma3d::IResource>& IResourceBuffer::GetB3DResource()        const override { return ResourceImpl::GetB3DResourceImpl(); }

    void*                       GetMppedData()                                          override;
    const buma3d::MAPPED_RANGE* GetMppedRange()                                 const   override;
    void                        Flush(const buma3d::MAPPED_RANGE* _range)               override;
    void                        Invalidate(const buma3d::MAPPED_RANGE* _range)          override;

private:
    bool Init(const buma3d::RESOURCE_DESC& _desc, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _heap_flags, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS  _deny_heap_flags);

private:
    void*                   mapped_data;
    buma3d::MAPPED_RANGE    mapped_range;

};


}// namespace res
}// namespace buma3d
