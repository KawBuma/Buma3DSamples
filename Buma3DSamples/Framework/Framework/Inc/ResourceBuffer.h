#pragma once

namespace buma
{
namespace res
{

struct IResourceBuffer : public IResource
{
    virtual ~IResourceBuffer() {}

    buma3d::util::Ptr<buma3d::IBuffer> GetB3DBuffer() const { return GetB3DResource().As<buma3d::IBuffer>(); }

    /**
     * @brief ホスト可視ヒープでサブ割り当てされた場合、マップされたポインタを返します。
     * @return このリソースへオフセットされたマップされたポインタを返します。  マップ不可リソースの場合nullptrを返します。
    */
    virtual void*                               GetMppedData() = 0;
    virtual const buma3d::MAPPED_RANGE*         GetMppedRange() const = 0;
    virtual void                                Flush(const buma3d::MAPPED_RANGE* _range = nullptr) = 0;
    virtual void                                Invalidate(const buma3d::MAPPED_RANGE* _range = nullptr) = 0;

    template<typename T>
    T* GetMppedDataAs(size_t _index = 0) { return static_cast<T*>(GetMppedData()) + _index; }

};


}// namespace res
}// namespace buma
