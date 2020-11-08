#pragma once

namespace buma
{

class SampleAppBase : public ApplicationBase
{
public:
    SampleAppBase();
    virtual ~SampleAppBase();

    static SampleAppBase* Create();

    void Prepare(PlatformBase& _platform) {}

    void Init() override {}
    virtual void LoadAssets() {}

    void Tick() override {}
    virtual void Update() {}
    virtual void Render() {}

    void Term() override {}

private:

};


}// namespace buma
