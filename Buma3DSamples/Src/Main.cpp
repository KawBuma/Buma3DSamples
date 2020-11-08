#include <memory>

#include <Windows.h>

#include "Framework.h"
#include "SampleBase.h"

int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow      )
{
    int code = -1;
    {
        _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);

        auto platform = buma::CreatePlatform();
        {
            auto app = std::shared_ptr<buma::SampleAppBase>(buma::SampleAppBase::Create());
            platform->AttachApplication(app);
        }

        auto data = buma::PLATFORM_DATA_WINDOWS{ hInstance, hPrevInstance, lpCmdLine, nCmdShow };
        buma::PLATFORM_DESC pd{buma::PLATFORM_TYPE_WINDOWS, &data };

        if (platform->Init(pd))
        {
            code = platform->MainLoop();
            platform->Term();
        }
        buma::DestroyPlatform(platform);
        platform = nullptr;

        _CrtDumpMemoryLeaks();
    }

    return code;
}
