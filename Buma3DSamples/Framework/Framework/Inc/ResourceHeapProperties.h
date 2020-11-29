#pragma once

namespace buma
{

class CompatibleResourceHeapProperties
{
    friend class ResourceHeapProperties;
    void Init(const std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>& _heap_properties, buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _flags)
    {
        for (auto& i : _heap_properties)
        {
            if (i.flags & _flags)
                props.push_back(&i);
        }
    }

public:
    CompatibleResourceHeapProperties()
        : props {}
    {
    }

    ~CompatibleResourceHeapProperties()
    {
    }

    operator bool() const { return props.empty(); }

    const std::vector<const buma3d::RESOURCE_HEAP_PROPERTIES*>& Get() const { return props; }

    const buma3d::RESOURCE_HEAP_PROPERTIES* Filter(
          buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _flags
        , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS _deny_flags = buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_COPY_DST_FIXED |
                                                             buma3d::RESOURCE_HEAP_PROPERTY_FLAG_ACCESS_GENERIC_MEMORY_READ_FIXED ) const
    {
        for (auto& i : props)
        {
            if (!(i->flags & _deny_flags) && i->flags & _flags)
                return i;
        }
        return nullptr;
    }

private:
    std::vector<const buma3d::RESOURCE_HEAP_PROPERTIES*> props;

};

class ResourceHeapProperties
{
public:
    ResourceHeapProperties(buma3d::IDevice* _device)
        : heap_properties{}
    {
        heap_properties.resize(_device->GetResourceHeapProperties(nullptr));
        _device->GetResourceHeapProperties(heap_properties.data());

        device_local_heaps   .Init(heap_properties, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
        host_readable_heaps  .Init(heap_properties, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_READABLE);
        host_writable_heaps  .Init(heap_properties, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE);
        host_read_write_heaps.Init(heap_properties, buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_READABLE | buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE);
    }

    ~ResourceHeapProperties()
    {
    }

    const std::vector<buma3d::RESOURCE_HEAP_PROPERTIES>& Get() const { return heap_properties; }
    const CompatibleResourceHeapProperties& GetDeviceLocalHeaps() const { return device_local_heaps; }
    const CompatibleResourceHeapProperties& GetHostReadableHeaps() const { return host_readable_heaps; }
    const CompatibleResourceHeapProperties& GetHostWritableHeaps() const { return host_writable_heaps; }
    const CompatibleResourceHeapProperties& GetHostReadWriteHeaps() const { return host_read_write_heaps; }

    void FindCompatibleHeaps(std::vector<const buma3d::RESOURCE_HEAP_PROPERTIES*>*  _dst
                             , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS                 _flags
                             , buma3d::RESOURCE_HEAP_PROPERTY_FLAGS                 _deny_flags)
    {
        for (auto& i : heap_properties)
        {
            if (!(i.flags & _deny_flags) && i.flags & _flags)
                _dst->push_back(&i);
        }
    }

private:
    std::vector<buma3d::RESOURCE_HEAP_PROPERTIES> heap_properties;
    CompatibleResourceHeapProperties device_local_heaps;
    CompatibleResourceHeapProperties host_readable_heaps;
    CompatibleResourceHeapProperties host_writable_heaps;
    CompatibleResourceHeapProperties host_read_write_heaps;

};


}// namespace buma
