#include "pch.h"
#include "DrawsIndexBuffer.h"

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

size_t GetIndexSize(INDEX_BUFFER_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::INDEX_BUFFER_TYPE_UINT16: return 2;
    case buma::draws::INDEX_BUFFER_TYPE_UINT32: return 4;

    default:
        return 0;
    }
}

}//namespace /*anonymous*/


DrawsIndexBuffer::DrawsIndexBuffer()
    : ref_count     { 1 }
    , ins           {}
    , type          {}
    , index_count   {}
    , flags         {}
    , buffer        {}
{

}

DrawsIndexBuffer::~DrawsIndexBuffer()
{

}

bool DrawsIndexBuffer::Init(DrawsInstance* _ins, const INDEX_BUFFER_CREATE_DESC& _desc)
{
    ins = _ins;

    type         = _desc.type;
    index_count  = _desc.index_count;
    flags        = _desc.flags;

    buffer = ins->GetDR()->GetResourceCreate()
        ->CreateBuffer(buma3d::hlp::init::BufferResourceDesc(GetIndexSize(type) * index_count, buma3d::BUFFER_USAGE_FLAG_INDEX_BUFFER)
                       , flags & BUFFER_FLAG_CPU_VISIBLE
                       ? buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE
                       : buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
    return buffer.get();
}

bool DrawsIndexBuffer::Create(DrawsInstance* _ins, const INDEX_BUFFER_CREATE_DESC& _desc, IDrawsIndexBuffer** _dst)
{
    auto p = new DrawsIndexBuffer();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

void DrawsIndexBuffer::AddRef()
{
    ++ref_count;
}

uint32_t DrawsIndexBuffer::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}

void DrawsIndexBuffer::SetName(const char* _name)
{
    buffer->SetName(_name);
}

const char* DrawsIndexBuffer::GetName()
{
    return buffer->GetName();
}

bool DrawsIndexBuffer::SetData(size_t _dst_offset, size_t _src_size, const void* _src_data)
{
    if (flags & BUFFER_FLAG_CPU_VISIBLE)
    {
        auto b = (uint8_t*)buffer->GetMppedData() + _dst_offset;
        memcpy(b, _src_data, _src_size);
    }
    else
    {

    }
    return true;
}


}// namespace buma
}// namespace draws
