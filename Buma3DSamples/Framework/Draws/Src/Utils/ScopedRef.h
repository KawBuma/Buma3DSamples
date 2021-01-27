#pragma once

namespace buma
{
namespace draws
{

template <typename T>
class ScopedRef
{
public:
    ScopedRef()
        : ptr{}
    {
    }
    ScopedRef(T* _ptr)
        : ptr{ _ptr }
    {
        if (ptr)
            ptr->AddRef();
    }
    ScopedRef(const ScopedRef<T>& _ptr)
    {
        Reset(_ptr.ptr);
    }
    ScopedRef<T>& operator= (const ScopedRef<T>& _ptr)
    {
        Reset(_ptr.ptr);
        return *this;
    }
    ScopedRef<T>& operator= (T* _ptr)
    {
        Reset(_ptr);
        return *this;
    }
    ~ScopedRef() { if (ptr) ptr->Release(); }

    void Reset(T* _ptr = nullptr)
    {
        if (ptr)
            ptr->Release();
        ptr = _ptr;
        if (ptr)
            ptr->AddRef();
    }

    T* Detach()
    {
        auto result = ptr;
        ptr = nullptr;
        return result;
    }

    T* Get() const 
    {
        return ptr;
    }

    T* operator->()
    {
        return ptr;
    }
    const T* operator->() const 
    {
        return ptr;
    }

private:
    T* ptr;

};

template<typename T, typename U>
inline bool operator==(const ScopedRef<T>& _a, const ScopedRef<U>& _b)
{ return _a.Get() == _b.Get(); }

template<typename T, typename U>
inline bool operator!=(const ScopedRef<T>& _a, const ScopedRef<U>& _b)
{ return _a.Get() != _b.Get(); }

template<typename T, typename U>
inline bool operator==(const ScopedRef<T>& _a, const U* _b)
{ return _a.Get() == _b; }

template<typename T, typename U>
inline bool operator!=(const ScopedRef<T>& _a, const U* _b)
{ return _a.Get() != _b; }



}// namespace draws
}// namespace buma
