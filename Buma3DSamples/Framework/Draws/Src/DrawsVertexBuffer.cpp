#include "pch.h"
#include "DrawsVertexBuffer.h"

namespace buma
{
namespace draws
{

namespace /*anonymous*/
{

size_t GetVertexSize(VERTEX_BUFFER_TYPE _type)
{
    switch (_type)
    {
    case buma::draws::VERTEX_BUFFER_TYPE_FLOAT1 : return 4 * 1;
    case buma::draws::VERTEX_BUFFER_TYPE_FLOAT2 : return 4 * 2;
    case buma::draws::VERTEX_BUFFER_TYPE_FLOAT3 : return 4 * 3;
    case buma::draws::VERTEX_BUFFER_TYPE_FLOAT4 : return 4 * 4;
    case buma::draws::VERTEX_BUFFER_TYPE_SINT1  : return 4 * 1;
    case buma::draws::VERTEX_BUFFER_TYPE_SINT2  : return 4 * 2;
    case buma::draws::VERTEX_BUFFER_TYPE_SINT3  : return 4 * 3;
    case buma::draws::VERTEX_BUFFER_TYPE_SINT4  : return 4 * 4;
    case buma::draws::VERTEX_BUFFER_TYPE_UINT1  : return 4 * 1;
    case buma::draws::VERTEX_BUFFER_TYPE_UINT2  : return 4 * 2;
    case buma::draws::VERTEX_BUFFER_TYPE_UINT3  : return 4 * 3;
    case buma::draws::VERTEX_BUFFER_TYPE_UINT4  : return 4 * 4;

    default:
        return 0;
    }
}

}//namespace /*anonymous*/

DrawsVertexBuffer::DrawsVertexBuffer()
    : ref_count     { 1 }
    , ins           {}
    , type          {}
    , vertex_count  {}
    , flags         {}
    , buffer        {}
{

}

DrawsVertexBuffer::~DrawsVertexBuffer()
{

}

bool DrawsVertexBuffer::Init(DrawsInstance* _ins, const VERTEX_BUFFER_CREATE_DESC& _desc)
{
    ins = _ins;

    type         = _desc.type;
    vertex_count = _desc.vertex_count;
    flags        = _desc.flags;

    buffer = _ins->GetDR()->GetResourceCreate()
        ->CreateBuffer(buma3d::hlp::init::BufferResourceDesc(GetVertexSize(type) * vertex_count, buma3d::BUFFER_USAGE_FLAG_VERTEX_BUFFER)
                       , flags & BUFFER_FLAG_CPU_VISIBLE
                       ? buma3d::RESOURCE_HEAP_PROPERTY_FLAG_HOST_WRITABLE
                       : buma3d::RESOURCE_HEAP_PROPERTY_FLAG_DEVICE_LOCAL);
    return buffer.get();
}

bool DrawsVertexBuffer::Create(DrawsInstance* _ins, const VERTEX_BUFFER_CREATE_DESC& _desc, IDrawsVertexBuffer** _dst)
{
    auto p = new DrawsVertexBuffer();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

void DrawsVertexBuffer::AddRef()
{
    ++ref_count;
}

uint32_t DrawsVertexBuffer::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}

void DrawsVertexBuffer::SetName(const char* _name)
{
    buffer->SetName(_name);
}

const char* DrawsVertexBuffer::GetName()
{
    return buffer->GetName();
}

bool DrawsVertexBuffer::SetData(size_t _dst_offset, size_t _src_size, const void* _src_data)
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
