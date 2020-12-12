#pragma once

namespace buma
{

class SampleAppBase : public ApplicationBase
{
public:
    SampleAppBase();
    virtual ~SampleAppBase();

    static SampleAppBase* Create();

    bool Prepare(PlatformBase& _platform) override { return true; }

    bool Init() override { return true; }
    virtual void LoadAssets() {}

    void Tick() override {}
    virtual void Update() {}
    virtual void Render() {}

    void Term() override {}

private:

};

BUMA_DLL_API ApplicationBase* CreateApplication() { return SampleAppBase::Create(); }


}// namespace buma
