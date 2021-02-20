#pragma once

namespace buma
{
namespace draws
{

class Draws_ : public IDraws_
{
protected:
    Draws_();
    ~Draws_();

    bool Init(DrawsInstance* _ins, const _CREATE_DESC& _desc);

public:
    static bool Create(DrawsInstance* _ins, const _CREATE_DESC& _desc, IDraws_** _dst);

    void     AddRef()  override;
    uint32_t Release() override;

private:
    std::atomic_uint32_t    ref_count;
    DrawsInstance*          ins;

};


}// namespace buma
}// namespace draws
