#include "Buma3DSamples.h"

#define WIN32_LEAN_AND_MEAN 
#include <Windows.h>
#include <shellapi.h>

// NOTE: 開発環境から実行する場合
// プロパティ>構成プロパティ>デバッグ>コマンド引数
// --app $(ProjectDir)..\$(Platform)\$(Configuration)\HelloImGui --internal-api-type [vulkanまたはd3d12] --enable-log --enable-b3d-debug --library-dir $(ProjectDir)\External\Buma3D\Project\[VulkanまたはD3D12]\v16\DLLBuild\$(Configuration)\x64\Out\

void ShowHelp()
{
    constexpr auto DESCRIPTION =
R"(USAGE: Buma3DSamples.exe Options <Inputs>
Options:
--app <application path>
起動するアプリケーションモジュールのディレクトリと、拡張子を含まないファイル名を指定します。 
for instance: --app ./HelloTriangle 

--library-dir <library path>
Buma3Dのモジュールディレクトリを指定します。 

--internal-api-type <api type>
Buma3Dが使用する内部APIを指定します。 <api type> は d3d12 または vulkan です。 

--enable-log
ログの出力を有効にします。 

--enable-b3d-debug
Buma3Dのデバッグを有効にします。 )";

    MessageBoxA(NULL, DESCRIPTION, "Buma3DSamples", MB_OK);
}

HMODULE LoadApplicationModule(LPWSTR lpCmdLine)
{
    HMODULE result = NULL;
    int  argc{};
	auto argv = CommandLineToArgvW(lpCmdLine, &argc);
    for (int i = 0; i < argc; i++)
    {
        if (wcsncmp(argv[i], L"--app", wcslen(L"--app")) == 0)
        {
            std::wstring ws = argv[i + 1];
            ws += L".dll";
            result = LoadLibrary(ws.c_str());
            break;
        }
    }

    return result;
}
void DestroyApplicationModule(HMODULE _hmodule)
{
    FreeLibrary(_hmodule);
}

buma::ApplicationBase* CreateApplication(HMODULE _app_module)
{
    auto CreateApplication = (buma::PFN_CreateApplication)GetProcAddress(_app_module, "CreateApplication");
    return CreateApplication();
}

int APIENTRY wWinMain(_In_     HINSTANCE hInstance,
                      _In_opt_ HINSTANCE hPrevInstance,
                      _In_     LPWSTR    lpCmdLine,
                      _In_     int       nCmdShow      )
{
    int code = -1;
#ifdef _DEBUG
    _CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
#endif
    {
        if (nCmdShow == 1/*実行ファイル名のみ*/)
        {
            ShowHelp();
            return 0;
        }

        HMODULE app_module = LoadApplicationModule(lpCmdLine);
        if (app_module == NULL) return code;

        auto platform = buma::CreatePlatform();
        platform->AttachApplication(std::shared_ptr<buma::ApplicationBase>(CreateApplication(app_module)));
        {
            auto data = buma::PLATFORM_DATA_WINDOWS{ hInstance, hPrevInstance, lpCmdLine, nCmdShow };
            buma::PLATFORM_DESC pd{ buma::PLATFORM_TYPE_WINDOWS, &data };
            if (!platform->Prepare(pd)) goto cleanup;
            if (!platform->Init())      goto cleanup;
        }

        code = platform->MainLoop();

    cleanup:
        platform->Term();
        buma::DestroyPlatform(platform);
        platform = nullptr;

        DestroyApplicationModule(app_module);
        app_module = NULL;
    }

    return code;
}
