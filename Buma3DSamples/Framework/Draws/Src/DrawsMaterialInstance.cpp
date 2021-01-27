#include "pch.h"
#include "Draws_.h"

namespace buma
{
namespace draws
{

Draws_::Draws_()
    : ref_count { 1 }
    , ins       {}
{

}

Draws_::~Draws_()
{

}

bool Draws_::Init(DrawsInstance* _ins, const _CREATE_DESC& _desc)
{
    ins = _ins;

    return true;
}

bool Draws_::Create(DrawsInstance* _ins, const _CREATE_DESC& _desc, IDraws_** _dst)
{
    auto p = new Draws_();
    if (!p->Init(_ins, _desc))
    {
        p->Release();
        return false;
    }

    *_dst = p;
    return true;
}

void Draws_::AddRef()
{
    ++ref_count;
}

uint32_t Draws_::Release()
{
    uint32_t result = --ref_count;
    if (result == 0)
        delete this;
    return result;
}


}// namespace buma
}// namespace draws
