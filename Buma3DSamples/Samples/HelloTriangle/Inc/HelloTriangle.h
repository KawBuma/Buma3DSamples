#pragma once

namespace buma
{

class HelloTriangle : public ApplicationBase
{
public:
    HelloTriangle();
    virtual ~HelloTriangle();

    static HelloTriangle* Create();

    void Prepare(PlatformBase& _platform);

    void Init() override;
    virtual void LoadAssets();

    void Tick() override;
    virtual void Update();
    virtual void Render();

    void Term() override;

private:
    PlatformBase*                                           platform;
    buma3d::util::Ptr<buma3d::IDevice>                      device;

    const std::vector<buma::WindowBase::SWAP_CHAIN_BUFFER>* back_buffers;
    buma3d::util::Ptr<buma3d::ISwapChain>                   swapchain;
    buma3d::SURFACE_FORMAT                                  sfs_format;

    Ptr<b::IRootSignature> signature;


};


}// namespace buma
