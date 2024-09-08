#define IMGUI_DEFINE_MATH_OPERATORS
#pragma once
#include "Render.hpp"
#include "LocalPlayer.hpp"
#include "Player.hpp"
#include "Camera.hpp"
#include "Misc.hpp"
#include "Spectator.hpp"
#include "Aimbot.hpp"
#include "Glow.hpp"
#include "Config.hpp"
#include <iostream>
#include <tchar.h>

#include "Drawmisc.hpp"

#pragma execution_character_set("utf-8")

int getWidth() {
    return width;
}

int getHeight() {
    return height;
}

// Data
ID3D11Device* g_pd3dDevice = nullptr;
ID3D11DeviceContext* g_pd3dDeviceContext = nullptr;
IDXGISwapChain* g_pSwapChain = nullptr;
UINT g_ResizeWidth = 0, g_ResizeHeight = 0;
ID3D11RenderTargetView* g_mainRenderTargetView = nullptr;
D3DX11_IMAGE_LOAD_INFO info; ID3DX11ThreadPump* pump{ nullptr };

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

// ImGui 上下文
ImGuiContext* g_ImGuiContext = nullptr;

// Define the display names and corresponding values separately
const char *keyNames[] = {
        "鼠标左键",
        "鼠标右键",
        "鼠标中键",
        "鼠标上侧键",
        "鼠标下侧键",
        "SHIFT键",
        "大小写键",
        "游戏手柄 A",
        "游戏手柄 B",
        "游戏手柄 X",
        "游戏手柄 Y",
        "侧顶部R1键",
        "侧顶部R2键",
        "侧顶部L1键",
        "侧顶部L2键",
        "关闭",
};
int keyValues[] = {
        VK_LBUTTON, // Left Mouse Button
        VK_RBUTTON, // Right Mouse Button
        VK_MBUTTON, // Middle Mouse Button
        VK_XBUTTON2, // Side2 Mouse Button
        VK_XBUTTON1, // Side1 Mouse Button
        VK_SHIFT, // SHIFT
        VK_CAPITAL, // CAPITAL
        VK_GAMEPAD_A,
        VK_GAMEPAD_B,
        VK_GAMEPAD_X,
        VK_GAMEPAD_Y,
        VK_GAMEPAD_RIGHT_SHOULDER, //R1
        VK_GAMEPAD_RIGHT_TRIGGER, //R2
        VK_GAMEPAD_LEFT_SHOULDER, //L1
        VK_GAMEPAD_LEFT_TRIGGER, //L2
        0x00, // OFF
};

const char *items[] = {"BPro", "Net"};

void AuthRender()
{
    // 创建应用程序窗口
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr,
                       nullptr, nullptr, L"Auth", nullptr };
    ::RegisterClassExW(&wc);
    // 获取用户的屏幕分辨率
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);

    GameCamera->ScreenSize = Vector2D((float)width, (float)height);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"AuthKey", WS_POPUP, 0, 0, 400, 200, nullptr, nullptr,
                                wc.hInstance, nullptr);//隐藏标题
    // 初始化 Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    // 显示窗口
    ::ShowWindow(hwnd, 10);
    ::UpdateWindow(hwnd);

    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);//窗口透明
    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);//鼠标穿透

    // 设置 Dear ImGui 上下文
    IMGUI_CHECKVERSION();
    g_ImGuiContext = ImGui::CreateContext();  // 创建 ImGui 上下文
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    io.IniFilename = nullptr;
    ImFontConfig Font_cfg;
    Font_cfg.FontDataOwnedByAtlas = false;

    ImFont* TitleFont = io.Fonts->AddFontFromMemoryTTF((void *) ttf_data, ttf_data_len, 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    ImFont* TextFont = io.Fonts->AddFontFromMemoryTTF((void *) ttf_data, ttf_data_len, 14.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    io.Fonts->AddFontFromMemoryTTF((void *) ttf_data, ttf_data_len, 14.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());

    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    style.FrameRounding = 12;

    // 菜单主题
    ImGui::StyleColorsDark();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 0.0f); // 黑色背景
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 深灰色框架
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 黑色标题栏背景
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 活动窗口标题栏背景
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 0.5f); // 活动窗口标题栏背景
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.3f, 0.2f, 0.5f); // 活动窗口标题栏背景
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.3f, 0.2f, 1.0f); // 活动窗口标题栏背景

    // 主循环
    while (!LoginSuccess && !IsLatestVersion) {
        MSG msg;
        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                exit(0);
        }
        if (LoginSuccess && !IsLatestVersion)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        if(!LoginSuccess && !IsLatestVersion)
        {
            ImGuiWindowClass noAutoMerge;
            noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
            ImGui::SetNextWindowClass(&noAutoMerge); // 自动脱离

            //获取窗口的宽度
            float windowWidth = ImGui::GetWindowWidth();

            ImGui::Begin("FATE Login", (bool *) true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
            ImGui::SetWindowSize({400, 240}, ImGuiCond_Once);

            LoginKey = loadCardKey();
            static char loginKeyBuffer[1024];
            strcpy(loginKeyBuffer, LoginKey.c_str()); // 将std::string转换为char数组

            ImGui::NewLine();
            //文本宽度
            std::string FATE_TITLE = "FATE v" + LocalVersion + " Login";
            float TitleTextWidth = ImGui::CalcTextSize(FATE_TITLE.c_str()).x; // 使用一个最长可能的日期时间字符串来估计宽度
            //设置光标到窗口的最右侧减去文本宽度

            ImGui::PushFont(TitleFont);
            ImGui::SetCursorPosX((windowWidth - TitleTextWidth) / 2);
            ImGui::TextColored(ImVec4(0, 1, 0.5, 1), "%s", FATE_TITLE.c_str());
            ImGui::PopFont();

            ImGui::PushFont(TextFont);
            ImGui::NewLine();
            ImGui::Separator();
            //ImGui::SetNextItemWidth(ImGui::GetContentRegionMax().x - 50);
            if (ImGui::InputTextEx("", "卡密", loginKeyBuffer, IM_ARRAYSIZE(loginKeyBuffer), ImVec2(ImGui::GetContentRegionMax().x - 10, 30), NULL))
            {
                LoginKey = loginKeyBuffer;
                saveCardKey(LoginKey);
            }
            //ImGui::SetCursorPos(ImVec2(ImGui::GetContentRegionMax().x / 2, 50));
            ImGui::Separator();

            ImGui::NewLine();
            // 计算两个按钮的总宽度
            float buttonWidth = 80 * 2 + ImGui::GetStyle().ItemSpacing.x; // 假设每个按钮宽80，加上按钮间的间隔
            ImVec2 center = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetCursorPosX(center.x - buttonWidth * 0.5f); // 设置光标位置为窗口中心减去按钮宽度的一半，y轴保持不变

            // 登录按钮
            if (ImGui::Button("登录", ImVec2(80, 20)))
            {
                LoginKey = loginKeyBuffer;
                LoginSuccess = SingleLogin();
                if (LoginSuccess)
                {
                    GetExpired();
                    IsLatestVersion = CheckAppVersion();
                    if (!IsLatestVersion)
                    {
                        GetWeb();
                        std::cout << FinalUrl << std::endl;
                        DownloadFile(FinalUrl, "FATE_NEW.exe");
                        selfDelete(FATEName);
                    }
                }
            }
            ImGui::SameLine();

            //解绑按钮
            if (ImGui::Button("解绑", ImVec2(80, 20)))
            {
                ResetHWID(LoginKey);
            }
            ImGui::NewLine();
            ImGui::NewLine();

            std::string dateTimeStr = GetCurrentDateTimeString();
            //文本宽度
            float TimeTextWidth = ImGui::CalcTextSize(dateTimeStr.c_str()).x; // 使用一个最长可能的日期时间字符串来估计宽度
            //设置光标到窗口的最右侧减去文本宽度
            ImGui::SetCursorPosX((windowWidth - TimeTextWidth) / 2);
            ImGui::TextColored(ImVec4(0, 1, 0.5, 1), "%s", dateTimeStr.c_str());

            ImGui::PopFont();
            ImGui::End();
        }
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                                  clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        g_pSwapChain->Present(EnableVSync ? 1 : 0, 0);
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(g_ImGuiContext);  // 销毁 ImGui 上下文

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(reinterpret_cast<LPCSTR>(wc.lpszClassName), wc.hInstance);

    return;
}
// 主代码
void Render(LocalPlayer* Myself, std::vector<Player*>* Players, Camera* GameCamera, Spectator* Spectators, Aimbot* AimAssist, Sense* ESP)
{
    // 创建应用程序窗口
    WNDCLASSEXW wc = { sizeof(WNDCLASSEXW), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(nullptr), nullptr, nullptr,
                       nullptr, nullptr, L"ImGui Example", nullptr };
    ::RegisterClassExW(&wc);
    // 获取用户的屏幕分辨率
    width = GetSystemMetrics(SM_CXSCREEN);
    height = GetSystemMetrics(SM_CYSCREEN);

    GameCamera->ScreenSize = Vector2D((float)width, (float)height);

    HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"Spider", WS_POPUP, 0, 0, width, height, nullptr, nullptr,
                                wc.hInstance, nullptr);//隐藏标题
    // 初始化 Direct3D
    if (!CreateDeviceD3D(hwnd)) {
        CleanupDeviceD3D();
        ::UnregisterClassW(wc.lpszClassName, wc.hInstance);
        return;
    }

    // 显示窗口
    ::ShowWindow(hwnd, 10);
    ::UpdateWindow(hwnd);

    ImGui_ImplWin32_EnableAlphaCompositing(hwnd);//窗口透明

    LONG exStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
    exStyle |= WS_EX_TRANSPARENT | WS_EX_LAYERED;
    SetWindowLong(hwnd, GWL_EXSTYLE, exStyle);//鼠标穿透

    // 设置 Dear ImGui 上下文
    IMGUI_CHECKVERSION();
    g_ImGuiContext = ImGui::CreateContext();  // 创建 ImGui 上下文
    ImGuiIO& io = ImGui::GetIO();
    (void)io;

    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

    ImGuiStyle &style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable) {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    ImGui_ImplWin32_Init(hwnd);
    ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);
    io.IniFilename = nullptr;
    ImFontConfig Font_cfg;
    Font_cfg.FontDataOwnedByAtlas = false;

    io.Fonts->AddFontFromMemoryTTF((void *) ttf_data, ttf_data_len, 14.0f, &Font_cfg,
                                   io.Fonts->GetGlyphRangesChineseFull());

    bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);

    style.FrameRounding = 12;
// 添加字体
//ImFont* Font = io.Fonts->AddFontFromMemoryTTF((void*)Font_data, Font_size, 18.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());
    //ImFont* Font = io.Fonts->AddFontFromMemoryTTF((void*)ttf_data, ttf_data_len, 16.0f, &Font_cfg, io.Fonts->GetGlyphRangesChineseFull());

//KmBoxB+ 优化控件支持
// 将 int 转换为字符串
    std::string KmboxComPortStr = std::to_string(AimAssist->KmboxComPort);
    std::string BPro_PorterStr = std::to_string(AimAssist->BaudRate);

// 创建字符数组来存储输入
    char KmboxComPortBuf[64];
    char BPro_PorterBuf[64];

// 将字符串复制到字符数组中
    strncpy(KmboxComPortBuf, KmboxComPortStr.c_str(), sizeof(KmboxComPortBuf));
    strncpy(BPro_PorterBuf, BPro_PorterStr.c_str(), sizeof(BPro_PorterBuf));

    // 菜单主题
    ImGui::StyleColorsDark();
    style.Colors[ImGuiCol_WindowBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 黑色背景
    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.2f, 0.2f, 0.2f, 1.0f); // 深灰色框架
    style.Colors[ImGuiCol_TitleBg] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 黑色标题栏背景
    style.Colors[ImGuiCol_TitleBgActive] = ImVec4(0.0f, 0.0f, 0.0f, 1.0f); // 活动窗口标题栏背景
    style.Colors[ImGuiCol_Button] = ImVec4(0.2f, 0.2f, 0.2f, 0.5f); // 活动窗口标题栏背景
    style.Colors[ImGuiCol_ButtonHovered] = ImVec4(0.0f, 0.3f, 0.2f, 0.5f); // 活动窗口标题栏背景
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.0f, 0.3f, 0.2f, 1.0f); // 活动窗口标题栏背景

    // 主循环
    bool done = false;
    while (!done) {
        MSG msg;

        // 获取当前时间点
        auto start = std::chrono::high_resolution_clock::now();

        // 获取当前时间
        newTime = std::chrono::steady_clock::now();

        // 检查时间间隔
        std::chrono::duration<double> elapsed_second1 = newTime - oldTime1;

        while (::PeekMessage(&msg, nullptr, 0U, 0U, PM_REMOVE)) {
            ::TranslateMessage(&msg);
            ::DispatchMessage(&msg);
            if (msg.message == WM_QUIT)
                done = true;
        }

        if (done)
            break;

        if (g_ResizeWidth != 0 && g_ResizeHeight != 0) {
            CleanupRenderTarget();
            g_pSwapChain->ResizeBuffers(0, g_ResizeWidth, g_ResizeHeight, DXGI_FORMAT_UNKNOWN, 0);
            g_ResizeWidth = g_ResizeHeight = 0;
            CreateRenderTarget();
        }

        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();

        //SetWindowPos(hwnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);//设置优先级别最高
        if (RenderEsp) {
            ImGui::SetNextWindowPos(ImVec2(0, 0));
            ImGui::SetNextWindowSize(ImVec2((float) getWidth(), (float) getHeight()));
            ImGui::Begin(("##esp"), (bool *) true,
                         ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
                         ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoBackground |
                         ImGuiWindowFlags_NoBringToFrontOnFocus);

            if (Map->IsPlayable) {

                int count = 0; // 用于存储距离小于等于300的人数
                float nearestDistance = std::numeric_limits<float>::max(); // 存储最近人的距离
                int nearestIndex = -1; // 存储最近人的索引

                for (int i = 0; i < Globalarray.size(); i++) {
                    float distance = CalculateEntityDistance(GameCamera->LocalCamera, Globalarray[i].coord);
                    if (distance <= v.WarningDistance) {
                        count++; // 距离小于等于300的人，计数增加
                        if (distance < nearestDistance) {
                            nearestDistance = distance;
                            nearestIndex = i;
                        }
                    }
                    if (!Globalarray[i].Admin) if (distance > v.drawdistance)continue;

                    if (Globalarray[i].LocalPlayer)continue;
                    if (Globalarray[i].Teammates && !GlowESPTeam)continue;
                    if (Globalarray[i].Dead || Globalarray[i].Health <= 0)continue;
                    Vector4D Box;
                    if (GameCamera->WorldToScreen4D(Globalarray[i].coord, Box)) {
//                        Vector2D GrdPos;
//                        if (AimAssist->SkyPoint.x != 0) {
//                            bool x1 = GameCamera->WorldToScreen(AimAssist->SkyPoint, GrdPos);
//                            ImGui::GetForegroundDrawList()->AddCircleFilled(ImVec2(GrdPos.x,GrdPos.y), 5, ImColor(0, 255, 0), 0);
//                        }

                        if (v.box) {
                            //DrawBox(Box, Globalarray[i].Visible ? boxViz : boxInviz, 2);
                            PlayerColors.Box.InVisible = DRAW.FloatToImColor(DRAW.Box.Invisible);
                            PlayerColors.Box.Visible = DRAW.FloatToImColor(DRAW.Box.Visible);
                            DrawBoxStroke(Box, Globalarray[i].Visible ? PlayerColors.Box.Visible : PlayerColors.Box.InVisible, 2);
                        }

//                        if (v.skeleton && distance < BoneFreshDistance) {
//                            HitBoxManager HitBoxs;
//
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::Head, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenHeadBone); // 0
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::Neck, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenNeckBone); // 1
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::Hip, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenBotmBone); // 4
//
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::MidSpine, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenLeftshoulderBone); // 6
//                            GameCamera->WorldToScreen(GetBonePOS(HitboxType::LowerSpine, Globalarray[i].ModelAddress,
//                                                                 Globalarray[i].coord, Globalarray[i].BoneAddress),
//                                                      HitBoxs.ScreenLeftelbowBone); // 7
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::PropGun, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenLeftHandBone); // 8
//
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::NeckA, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenRightshoulderBone); // 9
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::NeckB, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenRightelbowBone); // 10
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::NeckC, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenRightHandBone); // 11
//
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::HeadB, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenLeftThighsBone); // 12
//                            GameCamera->WorldToScreen(GetBonePOS(HitboxType::LeftClavicle, Globalarray[i].ModelAddress,
//                                                                 Globalarray[i].coord, Globalarray[i].BoneAddress),
//                                                      HitBoxs.ScreenLeftkneesBone); // 13
//                            GameCamera->WorldToScreen(GetBonePOS(HitboxType::LeftShoulder, Globalarray[i].ModelAddress,
//                                                                 Globalarray[i].coord, Globalarray[i].BoneAddress),
//                                                      HitBoxs.ScreenLeftlegBone); // 14
//
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::LeftShoulderMid, Globalarray[i].ModelAddress,
//                                               Globalarray[i].coord, Globalarray[i].BoneAddress),
//                                    HitBoxs.ScreenRightThighsBone); // 16
//                            GameCamera->WorldToScreen(GetBonePOS(HitboxType::LeftElbowB, Globalarray[i].ModelAddress,
//                                                                 Globalarray[i].coord, Globalarray[i].BoneAddress),
//                                                      HitBoxs.ScreenRightkneesBone); // 17
//                            GameCamera->WorldToScreen(
//                                    GetBonePOS(HitboxType::LeftElbow, Globalarray[i].ModelAddress, Globalarray[i].coord,
//                                               Globalarray[i].BoneAddress), HitBoxs.ScreenRightlegBone); // 18
//
//                            DrawLine(ImVec2(HitBoxs.ScreenHeadBone.x, HitBoxs.ScreenHeadBone.y),
//                                     ImVec2(HitBoxs.ScreenNeckBone.x, HitBoxs.ScreenNeckBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenNeckBone.x, HitBoxs.ScreenNeckBone.y),
//                                     ImVec2(HitBoxs.ScreenBotmBone.x, HitBoxs.ScreenBotmBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//
//                            DrawLine(ImVec2(HitBoxs.ScreenNeckBone.x, HitBoxs.ScreenNeckBone.y),
//                                     ImVec2(HitBoxs.ScreenLeftshoulderBone.x, HitBoxs.ScreenLeftshoulderBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenLeftshoulderBone.x, HitBoxs.ScreenLeftshoulderBone.y),
//                                     ImVec2(HitBoxs.ScreenLeftelbowBone.x, HitBoxs.ScreenLeftelbowBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenLeftelbowBone.x, HitBoxs.ScreenLeftelbowBone.y),
//                                     ImVec2(HitBoxs.ScreenLeftHandBone.x, HitBoxs.ScreenLeftHandBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//
//                            DrawLine(ImVec2(HitBoxs.ScreenNeckBone.x, HitBoxs.ScreenNeckBone.y),
//                                     ImVec2(HitBoxs.ScreenRightshoulderBone.x, HitBoxs.ScreenRightshoulderBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenRightshoulderBone.x, HitBoxs.ScreenRightshoulderBone.y),
//                                     ImVec2(HitBoxs.ScreenRightelbowBone.x, HitBoxs.ScreenRightelbowBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenRightelbowBone.x, HitBoxs.ScreenRightelbowBone.y),
//                                     ImVec2(HitBoxs.ScreenRightHandBone.x, HitBoxs.ScreenRightHandBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//
//                            DrawLine(ImVec2(HitBoxs.ScreenBotmBone.x, HitBoxs.ScreenBotmBone.y),
//                                     ImVec2(HitBoxs.ScreenLeftThighsBone.x, HitBoxs.ScreenLeftThighsBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenLeftThighsBone.x, HitBoxs.ScreenLeftThighsBone.y),
//                                     ImVec2(HitBoxs.ScreenLeftkneesBone.x, HitBoxs.ScreenLeftkneesBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenLeftkneesBone.x, HitBoxs.ScreenLeftkneesBone.y),
//                                     ImVec2(HitBoxs.ScreenLeftlegBone.x, HitBoxs.ScreenLeftlegBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//
//                            DrawLine(ImVec2(HitBoxs.ScreenBotmBone.x, HitBoxs.ScreenBotmBone.y),
//                                     ImVec2(HitBoxs.ScreenRightThighsBone.x, HitBoxs.ScreenRightThighsBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenRightThighsBone.x, HitBoxs.ScreenRightThighsBone.y),
//                                     ImVec2(HitBoxs.ScreenRightkneesBone.x, HitBoxs.ScreenRightkneesBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                            DrawLine(ImVec2(HitBoxs.ScreenRightkneesBone.x, HitBoxs.ScreenRightkneesBone.y),
//                                     ImVec2(HitBoxs.ScreenRightlegBone.x, HitBoxs.ScreenRightlegBone.y),
//                                     Globalarray[i].Visible ? SkeltonViz : SkeltonInviz, SkeltonThickness);
//                        }
                        if (v.line) {
                            ImGui::GetForegroundDrawList()->AddLine({GameCamera->ScreenCenter.x, 0},
                                                                    {Box.x + Box.w / 2, Box.y - 16},
                                                                    Globalarray[i].Admin ? ImColor(255, 0, 0, 255)
                                                                                         : ImColor(255, 255, 255, 255),
                                                                    1);
                        }
                        if (v.drawname) {
                            std::string a;
                            a += Globalarray[i].Name;
                            auto Size = ImGui::CalcTextSize(a.c_str());
                            DrawFontStroke(15, (Box.x + (Box.w / 2)) - (Size.x / 2), Box.y - 14,
                                           Globalarray[i].Admin ? ImColor(255, 0, 0, 255) : ImColor(255, 255, 255, 255),
                                           a.c_str());
                        }
                        if (v.showfov) {
                            ImGui::GetForegroundDrawList()->AddCircle(
                                    {GameCamera->ScreenCenter.x, GameCamera->ScreenCenter.y}, (mem.GetKeyboard()->IsKeyDown(0x2) || mem.isPressed(109)) ? AimAssist->ZoomFOV * AimAssist->ZoomScale * 12 : AimAssist->FOV * 12,
                                    ImColor(255, 255, 255, 255), 0, 1.5);
                        }
                        if (v.drawinfo && distance < v.infodistance) {
                            std::string playerRank = Globalarray[i].PlayerRank;
                            std::string a;
                            ImColor color = ImColor(255, 255, 255, 255); // 默认白色
                            ImColor InfoColor = DRAW.FloatToImColor(DRAW.Text.Info);
                            if (v.healthbar && !Globalarray[i].Knocked) {
                                if (BarStyle == 1) {
                                    DrawHealth(Box.x + Box.w / 2, Box.y - 14, Globalarray[i].Sheld,
                                               Globalarray[i].MaxSheld,
                                               0, Globalarray[i].Health);
                                }
                                if (BarStyle == 2) {
                                    HealthBar(Box, Globalarray[i].Health);
                                    ShildBar(Box, Globalarray[i].Sheld, Globalarray[i].MaxSheld);
                                }
                            }
                            if (v.distance) {
                                a += std::to_string((int) distance);
                                a += "M ";
                            }
                            if (v.drawteam) {
                                a += "[Team:";
                                a += std::to_string(Globalarray[i].Team);
                                a += "] ";
                            }
                            if (v.xp) {
                                a += " LV:";
                                std::ostringstream oss;
                                oss << Globalarray[i].PlayerLV;
                                a += oss.str();
                            }
                            if (v.rank) {
                                a += Globalarray[i].PlayerRank;
//                                a += " LV:";
//                                a += Globalarray[i].PlayerLevel;

                                if (playerRank.find("青铜") != std::string::npos) {
                                    color = ImColor(205, 127, 50); // 青铜色
                                } else if (playerRank.find("白银") != std::string::npos) {
                                    color = ImColor(192, 192, 192); // 白银色
                                } else if (playerRank.find("黄金") != std::string::npos) {
                                    color = ImColor(255, 215, 0); // 金色
                                } else if (playerRank.find("铂金") != std::string::npos) {
                                    color = ImColor(176, 224, 230); // 浅蓝色
                                } else if (playerRank.find("钻石") != std::string::npos) {
                                    color = ImColor(0, 0, 139); // 深蓝色
                                } else if (playerRank.find("猎杀") != std::string::npos) {
                                    color = ImColor(255, 0, 0); // 红色
                                } else if (playerRank.find("大师") != std::string::npos) {
                                    color = ImColor(128, 0, 128); // 紫色
                                }
                            }
                            auto Size = ImGui::CalcTextSize(a.c_str());
                            DrawFontStroke(DRAW.Text.size, (Box.x + (Box.w / 2)) - (Size.x / 2), Box.y + Box.z + 1, DRAW.Text.EnableRankColor ? color : InfoColor, a.c_str());
                        }

                        if (RList.IsAdmin(Globalarray[i].ID)) {
//                        if (Globalarray[i].Admin) {
                            std::string adminName = Globalarray[i].Name;
                            std::string a;
                            a += " *PRO* ";
                            auto Size = ImGui::CalcTextSize(a.c_str());
                            DrawFontStroke(DRAW.Text.size, (Box.x + (Box.w / 2)) - (Size.x / 2), Box.y + Box.z + 14,
                                           Globalarray[i].Admin ? ImColor(255, 0, 0, 255) : ImColor(255, 255, 255, 255),
                                           a.c_str());
                            // 更新 startY 以在下一行绘制下一个Admin名字
                        }
                        else if (RList.IsBlack(Globalarray[i].ID)) {
//                        else if (Globalarray[i].Black) {
                            std::string adminName = Globalarray[i].Name;
                            std::string a;
                            a += " BLACK ";
                            auto Size = ImGui::CalcTextSize(a.c_str());
                            DrawFontStroke(DRAW.Text.size, (Box.x + (Box.w / 2)) - (Size.x / 2), Box.y + Box.z + 14,
                                           Globalarray[i].Black ? ImColor(255, 255, 0, 255) : ImColor(255, 255, 255, 255),
                                           a.c_str());
                            // 更新 startY 以在下一行绘制下一个Admin名字
                        }
                        else if (RList.IsWhite(Globalarray[i].ID)) {
//                        else if (Globalarray[i].White) {
                            std::string adminName = Globalarray[i].Name;
                            std::string a;
                            a += " WHITE ";
                            auto Size = ImGui::CalcTextSize(a.c_str());
                            DrawFontStroke(DRAW.Text.size, (Box.x + (Box.w / 2)) - (Size.x / 2), Box.y + Box.z + 14,
                                           Globalarray[i].White ? ImColor(0, 255, 0, 255) : ImColor(255, 255, 255, 255),
                                           a.c_str());
                            // 更新 startY 以在下一行绘制下一个Admin名字
                        }
                    }
                }
                // 文字显示距离小于等于300的人数
                if (v.PlayerWarning)
                {
                    std::string countStr = "Players: " + std::to_string(count);
                    if (nearestIndex != -1) {
                        countStr += "Nearest: " + std::to_string(nearestDistance) + " 米";
                    }
                    DrawFontStroke(16, GameCamera->ScreenCenter.x - ImGui::CalcTextSize(countStr.c_str()).x / 2, 20,
                                   ImColor(255, 255, 255, 255), countStr.c_str());
                }
                //文字显示观战
                if (textspector) {
                    std::unordered_set<std::string> uniqueSpectators(Spectators->Spectators.begin(),
                                                                     Spectators->Spectators.end());

                    std::string countStr = "Spectators:" + std::to_string(uniqueSpectators.size());
                    DrawFontStroke(16, 10, 20, ImColor(255, 255, 255, 255), countStr.c_str());

                    int g = 0;
                    for (const auto &spectatorName: uniqueSpectators) {
                        DrawFontStroke(16, 20, 30 + GameCamera->ScreenCenter.y * 0.1 + g * 16, ImColor(255, 255, 255, 255), spectatorName.c_str());
                        g++;
                    }
                }
                ImGui::End();
            }
            else
            {
                Globalarray.clear();//退出对局清空数据
                Spectators->Spectators.clear();//退出对局清空观战
            }

            if (elapsed_second1.count() >= 8) {
                // VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_MEM_PARTIAL, 1);
                // VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_TLB_PARTIAL, 1);
                VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_MEM, 1);
                VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_TLB, 1);
                oldTime1 = newTime;
            }
        }

        static std::chrono::time_point LastTimePoint = std::chrono::steady_clock::now();
        auto CurTimePoint = std::chrono::steady_clock::now();

        //退出进程
        if ((mem.GetKeyboard()->IsKeyDown(VK_END) || mem.isPressed(75)) && CurTimePoint - LastTimePoint >= std::chrono::milliseconds(150)) {
            ImGui_ImplDX11_Shutdown();
            ImGui_ImplWin32_Shutdown();
            ImGui::DestroyContext();
            mem.~Memory();
            Sleep(500);
            exit(0);
        }
        //开关融合器
        if ((mem.GetKeyboard()->IsKeyDown(VK_INSERT) || mem.isPressed(72)) && CurTimePoint - LastTimePoint >= std::chrono::milliseconds(150)) {
            isBlenderChecked = !isBlenderChecked;
            LastTimePoint = CurTimePoint;
        }
        // 根据勾选框的状态启动或关闭Bwindow.exe
        if (isBlenderChecked) {
            clear_color = ImVec4{0, 0, 0, 255};
        }
        else {
            clear_color = ImVec4{0, 0, 0, 0};
        }
        //开关垂直同步
        if ((mem.GetKeyboard()->IsKeyDown(VK_DELETE) || mem.isPressed(73)) && CurTimePoint - LastTimePoint >= std::chrono::milliseconds(150)) {
            EnableVSync = !EnableVSync;
            LastTimePoint = CurTimePoint;
        }

        if ((mem.GetKeyboard()->IsKeyDown(VK_F5) || mem.isPressed(96)) && CurTimePoint - LastTimePoint >= std::chrono::milliseconds(150)) {
            VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_MEM, 1);//写入 - 刷新内存缓存（不包括 TLB）
            VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_TLB, 1);//写入 - 刷新页表（TLB）缓存
            VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_FREQ_MEDIUM, 1);
            VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_ALL, 1);
            RList.adminList.clear();
            RList.whiteList.clear();
            RList.blackList.clear();
            RList.loadLists();
            Players->clear();
            Globalarray.clear();
            PlayersPopulated = false;
            LastTimePoint = CurTimePoint;
        }

        if ((mem.GetKeyboard()->IsKeyDown(VK_HOME) || mem.isPressed(74)) && CurTimePoint - LastTimePoint >= std::chrono::milliseconds(150)) {
            ShowMenu = !ShowMenu;
            LastTimePoint = CurTimePoint;
        }

        if (guispector) {
            ImGuiWindowClass noAutoMerge;
            noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
            ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
            ImGui::Begin("Spector");
            ImGui::SetWindowSize({300, 200}, ImGuiCond_Once);

            for (int i = 0; i < Globalarray.size(); i++) {
                if (Globalarray[i].Admin) {
                    std::string adminName = "Pro " + Globalarray[i].Name;
                    ImGui::TextColored(ImVec4(1,0,0,1), "%s", adminName.c_str());
                    i++;
                }
            }

            std::unordered_set<std::string> uniqueSpectators(Spectators->Spectators.begin(), Spectators->Spectators.end());
            std::string countStr = "Spectators:" + std::to_string(uniqueSpectators.size());
            ImGui::Text("%s", countStr.c_str());
            int g = 0;
            for (const auto &spectatorName: uniqueSpectators) {
                ImGui::Text("%s", spectatorName.c_str());
                g++;
            }
            ImGui::End();
        }

        if (ShowMenu) {
            ImVec4 ListColor;

            ImGuiWindowClass noAutoMerge;
            noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
            ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
            ImGui::Begin("Player List");
            ImGui::SetWindowSize({800, 400}, ImGuiCond_Once);

            // 表格标志
            ImGuiTableFlags flags = ImGuiTableFlags_Sortable | ImGuiTableFlags_ScrollY |
                                    ImGuiTableFlags_RowBg | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

            // 开始表格
            if (ImGui::BeginTable("PlayerList", 7, flags)) {
                ImGui::TableSetupScrollFreeze(0, 1);

                // 获取内容区域尺寸
                ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();
                float columnWidth = contentRegionAvail.x / 24;

                // 设置列，但不指定宽度
                ImGui::TableSetupColumn(" Team", ImGuiTableColumnFlags_WidthFixed, columnWidth * 2);
                ImGui::TableSetupColumn("NAME", ImGuiTableColumnFlags_WidthFixed, columnWidth * 4);
                ImGui::TableSetupColumn("RANK", ImGuiTableColumnFlags_WidthFixed, columnWidth * 3);
                ImGui::TableSetupColumn("LEVEL", ImGuiTableColumnFlags_WidthFixed, columnWidth * 3);
                ImGui::TableSetupColumn("Status", ImGuiTableColumnFlags_WidthFixed, columnWidth * 3);
                ImGui::TableSetupColumn("UID", ImGuiTableColumnFlags_WidthFixed, columnWidth * 4);
                ImGui::TableSetupColumn("AddList", ImGuiTableColumnFlags_DefaultSort, columnWidth * 4);

                // 表头
                ImGui::TableHeadersRow();

                if (Map->IsPlayable) {
                    for (const auto &i: Globalarray) {
                        if (i.Dead || i.Health <= 0)
                            continue;

                        std::string ListName = i.Name;
                        int ListTeam = i.Team;
                        std::string ListRank = i.PlayerRank;
                        std::string ListLevel = std::to_string(i.PlayerLV);
                        std::string PlayerType = "Player";
                        ListColor = ImVec4(1,1,1,1);

                        if (i.LocalPlayer) { PlayerType = "Local"; ListColor = ImVec4(0,1,0,1);}
                        else if (i.Teammates) { PlayerType = "Ally"; ListColor = ImVec4(0,1,1,1);}
                        else if (RList.IsAdmin(i.ID)) { PlayerType = "Pro"; ListColor = ImVec4(1,0,0,1);}
                        else if (RList.IsWhite(i.ID)) { PlayerType = "White"; ListColor = ImVec4(0,1,1,1);}
                        else if (RList.IsBlack(i.ID)) { PlayerType = "Black"; ListColor = ImVec4(1,1,0,1);}
//                        else if (i.Admin) { PlayerType = "权限"; ListColor = ImVec4(1,0,0,1);}
//                        else if (i.White) { PlayerType = "白名单"; ListColor = ImVec4(0,1,1,1);}
//                        else if (i.Black) { PlayerType = "黑名单"; ListColor = ImVec4(1,1,0,1);}

                        std::string ListUID = std::to_string(i.ID);

                        ImGui::TableNextRow();
                        ImGui::TableSetColumnIndex(0);
                        ImGui::TextColored(ListColor, "%d", ListTeam);
                        ImGui::TableSetColumnIndex(1);
                        ImGui::TextColored(ListColor, "%s", ListName.c_str());
                        ImGui::TableSetColumnIndex(2);
                        ImGui::TextColored(ListColor, "%s", ListRank.c_str());
                        ImGui::TableSetColumnIndex(3);
                        ImGui::TextColored(ListColor, "%s", ListLevel.c_str());
                        ImGui::TableSetColumnIndex(4);
                        ImGui::TextColored(ListColor, "%s", PlayerType.c_str());
                        ImGui::TableSetColumnIndex(5);
                        ImGui::TextColored(ListColor, "%s", ListUID.c_str());
                        ImGui::TableSetColumnIndex(6);
                        ImGui::PushID(ListUID.c_str());
                        if (ImGui::Button("White"))
                        {
                            RList.AddToList("list_White.json", ListName, i.ID, 0);
                            RList.adminList.clear();
                            RList.whiteList.clear();
                            RList.blackList.clear();
                            RList.loadLists();
                            Players->clear();
                            Globalarray.clear();
                            PlayersPopulated = false;
                        }
                        ImGui::SameLine();
                        if (ImGui::Button("Black"))
                        {
                            RList.AddToList("list_Black.json", ListName, i.ID, 0);
                            RList.adminList.clear();
                            RList.whiteList.clear();
                            RList.blackList.clear();
                            RList.loadLists();
                            Players->clear();
                            Globalarray.clear();
                            PlayersPopulated = false;
                        }
//                        ImGui::SameLine();
//                        if (ImGui::Button("加权"))
//                        {
//                            RList.AddToList("list_Admin.json", ListName, i.ID, 0);
//                            Players->clear();
//                            Globalarray.clear();
//                            PlayersPopulated = false;
//                        }
                        ImGui::PopID();
                    }
                }
            }
            ImGui::EndTable();

            ImGui::End();
        }
        if (ShowMenu) {
            ImGuiWindowClass noAutoMerge;
            noAutoMerge.ViewportFlagsOverrideSet = ImGuiViewportFlags_NoAutoMerge;
            ImGui::SetNextWindowClass(&noAutoMerge);//自动脱离
            ImGui::Begin("FATE");
            ImGui::SetWindowSize({600, 500}, ImGuiCond_Once);

            {
                if (ImGui::CollapsingHeader("Render"))
                {
                    ImGui::InputFloat("ScreenSizeX", &GameCamera->ScreenSizeX);
                    ImGui::InputFloat("ScreenSizeY", &GameCamera->ScreenSizeY);
                    if (ImGui::Button("AutoGet"))
                    {
                        GameCamera->ScreenSizeX = (float)GetSystemMetrics(SM_CXSCREEN);
                        GameCamera->ScreenSizeY = (float)GetSystemMetrics(SM_CYSCREEN);
                    }
                    if (ImGui::TreeNode("Players"))
                    {
                        ImGui::Checkbox("EnableESP", &RenderEsp);
                        ImGui::SameLine();
                        ImGui::Checkbox("Teammates", &GlowESPTeam);
                        ImGui::SameLine();
                        ImGui::Checkbox("Info", &v.drawinfo);

                        ImGui::Checkbox("BOX", &v.box);
                        ImGui::SameLine();
                        ImGui::Checkbox("LINE", &v.line);
                        ImGui::SameLine();
                        ImGui::Checkbox("Distance", &v.distance);
                        ImGui::SameLine();
                        ImGui::Checkbox("HealthBar", &v.healthbar);
                        const char* items[] = { "Seer", "ALGS" };
                        static int item_current = BarStyle - 1; // 将BarStyle映射到下拉框的索引，因为索引从0开始
                        if (ImGui::Combo("Bar STYLE", &item_current, items, IM_ARRAYSIZE(items))) {
                            BarStyle = item_current + 1; // 更新BarStyle，加1是因为我们的选项是从1开始的
                        }
                        ImGui::Checkbox("LEVEL", &v.xp);
                        ImGui::SameLine();
                        ImGui::Checkbox("RANK", &v.rank);
                        ImGui::SameLine();
                        ImGui::Checkbox("NAME", &v.drawname);
                        ImGui::SameLine();
                        ImGui::Checkbox("TEAM", &v.drawteam);

                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Render Setting"))
                    {
                        ImGui::ColorEdit4("Info Color", (float*)&DRAW.Text.Info, picker_flags);
                        ImGui::SameLine();
                        ImGui::Checkbox("Color Based on RANK", &DRAW.Text.EnableRankColor);

                        ImGui::SliderFloat("Text Size", &DRAW.Text.size, 0, 20,"%.1f");
                        ImGui::SliderFloat("ESP Distance", &v.drawdistance, 0, 1000,"%.1f");
                        ImGui::SliderFloat("ESP Info Distance", &v.infodistance, 0, 1000,"%.1f");

                        ImGui::ColorEdit4("Box.Invisible", (float*)&DRAW.Box.Invisible, picker_flags);
                        ImGui::SameLine();
                        ImGui::ColorEdit4("Box.Visible", (float*)&DRAW.Box.Visible, picker_flags);

                        ImGui::TreePop();
                    }
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Aimbot"))
                {
                    const char *AimItems[] = {"Memory", "KmBox"};
                    // 当前选项的索引，这个将根据你选择的选项变化
                    int TypeItem = (AimAssist->AimBotType == "Memory") ? 0 : 1;
                    if (ImGui::Combo("AimBotType", &TypeItem,AimItems, IM_ARRAYSIZE(AimItems))) {
                        AimAssist->AimBotType = TypeItem[AimItems];
                    }
                    ImGui::Checkbox("Enable Aimbot", &AimAssist->Sticky);
                    ImGui::SameLine();
                    ImGui::Checkbox("Aim Teammate", &AimAssist->AimTeam);
                    ImGui::SameLine();
                    ImGui::Checkbox("Show FOV", &v.showfov);

                    if (ImGui::Checkbox("Auto Aim Bone", &AimAssist->autobone)) {
                        if (AimAssist->autobone) {
                            AimAssist->allbone = false;
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Head to chest，nearest crosshair");
                    }
                    ImGui::SameLine();
                    if (ImGui::Checkbox("Auto All Aim Bone", &AimAssist->allbone)) {
                        if (AimAssist->allbone) {
                            AimAssist->autobone = false;
                        }
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("All of the Bone，nearest crosshair");
                    }

                    ImGui::Separator();
                    ImGui::SliderFloat("Aim frequency", &AimAssist->aim_time, 0.0f, 0.05f);
                    ImGui::SliderFloat("No ADS FOV", &AimAssist->FOV, 1.0f, 50.0f, "% .1f");
                    ImGui::SliderFloat("ADS FOV", &AimAssist->ZoomFOV, 1.0f, 50.0f, "% .1f");
                    ImGui::SliderFloat("No ADS Smooth", &AimAssist->Smooth, 1.0f, 30.0f, "% .1f");
                    ImGui::SliderFloat("ADS Smooth", &AimAssist->ZoomSmooth, 1.0f, 30.0f, "% .1f");
                    ImGui::SliderFloat("Flick smooth", &AimAssist->Flicksmooth, 1.0f, 10.0f, "% .1f");
                    ImGui::SliderFloat("Skynade Smooth", &AimAssist->SkySmooth, 1.0f, 20.0f, "% .1f");
                    float displayValue = AimAssist->MaxSmoothIncrease * 100.0f; // 将内部数值转换为百分比显示值
                    if (ImGui::SliderFloat("Max Smooth Increase", &displayValue, -100.0f, 100.0f, "%.0f%%")) {
                        AimAssist->MaxSmoothIncrease = displayValue / 100.0f; // 将显示值转换回内部数值
                    }

                    ImGui::SliderFloat("Recoil Compensation", &AimAssist->RecoilCompensation, 0.0f, 2.0f, "% .2f");

                    const char* aimPartNames[] = {
                            "头部",
                            "脖子",
                            "上胸",
                            "腹部",
                            "中髋",
                            "瘠薄",
                            "左肩",
                            "左肘关节",
                            "左手腕",
                            "右肩",
                            "右肘关节",
                            "右手腕",
                            "左大腿根",
                            "左膝盖",
                            "左脚后跟",
                            "左脚尖",
                            "右大腿根",
                            "右膝盖",
                            "右脚后跟",
                            "右脚尖",
                    };
                    ImGui::SliderInt("Aim Bone", &AimAssist->aimBone, 0, 19, "%d");
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0.1,0.8,0.4,0.5),":%s", aimPartNames[AimAssist->aimBone]);

                    ImGui::Separator();

                    if (!AimAssist->InputModel)
                    {
                        ImGui::InputInt("AimBotKey[1]", &AimAssist->AimBotKey, 0, 0);
                        ImGui::InputInt("AimBotKey[2]", &AimAssist->AimBotKey2, 0, 0);
                        ImGui::InputInt("ThrowKey", &AimAssist->ThrowKey, 0, 0);
                        ImGui::InputInt("AimFlickKey", &AimAssist->AimFlickKey, 0, 0);
                        ImGui::InputInt("AimTriggerKey", &AimAssist->AimTriggerKey, 0, 0);
                        ImGui::InputInt("ForceHeadKey", &AimAssist->AimHeadKey, 0, 0);
                    }
                    else
                    {
                        ImGui::InputInt("inputAimKey[1]", &AimAssist->inputAimKey, 0, 0);
                        ImGui::InputInt("inputAimKey[2]", &AimAssist->inputAimKey2, 0, 0);
                        ImGui::InputInt("inputThrowKey", &AimAssist->inputThrowKey, 0, 0);
                        ImGui::InputInt("inputFlickKey", &AimAssist->inputFlickKey, 0, 0);
                        ImGui::InputInt("inputTriggerKey", &AimAssist->inputTriggerKey, 0, 0);
                        ImGui::InputInt("inputHeadKey", &AimAssist->inputHeadKey, 0, 0);
                    }
                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Other Features"))
                {
                    ImGui::TextColored(ImVec4(1,1,0,0.5),"[*] MainPC Press F10 to Open ALGS Map Radar");
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::SetTooltip("Writing Memory");
                    }
                    ImGui::Separator();
                    ImGui::Checkbox("Text show spector", &textspector);
                    ImGui::Checkbox("Gui show spector", &guispector);
                    ImGui::Checkbox("PlayerWarning", &v.PlayerWarning);
                    ImGui::SliderFloat("WarningDistance", &v.WarningDistance, 0, 500,"%.0f");

                    ImGui::Checkbox("SuperGlide", &Miscellanous->SuperGlide);
                    ImGui::Checkbox("TS", &Miscellanous->isTS);
                    ImGui::Checkbox("Auto rabbit jump", &Miscellanous->autorbjump);
                    ImGui::Checkbox("Auto Grpple", &Miscellanous->autoGrpple);
                    ImGui::Checkbox("TacticalReload", &AimAssist->fastreload);
                    ImGui::NewLine();
                    ImGui::Checkbox("Input system key Model", &AimAssist->InputModel);
                    ImGui::TextColored(ImVec4(0,1,1,0.5),"Use it if listening to the keyboard fails");

                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Aimbot Device"))
                {
                    ImGui::Text("Device Type");
                    const char *items[] = {"BPro", "Net"};

                    int currentItem = (AimAssist->KmboxType == "BPro") ? 0 : 1;

                    // Combo 下拉选择框
                    if (ImGui::Combo("DeviceType", &currentItem, items, IM_ARRAYSIZE(items))) {
                        // 当选择变更时更新 KmboxType 的值
                        AimAssist->KmboxType = items[currentItem];
                    }
                    if (AimAssist->KmboxType == "BPro") {
                        // 使用 ImGui::InputText 函数
                        if (ImGui::InputTextEx("ComPort", "...", KmboxComPortBuf, IM_ARRAYSIZE(KmboxComPortBuf), ImVec2(ImGui::GetContentRegionMax().x - 100, 20), NULL)) {
                            // 将输入文本转换回 int
                            AimAssist->KmboxComPort = std::stoi(KmboxComPortBuf);
                        }

                        if (ImGui::InputTextEx("BaudRate", "...", BPro_PorterBuf, IM_ARRAYSIZE(BPro_PorterBuf), ImVec2(ImGui::GetContentRegionMax().x - 100, 20), NULL)) {
                            // 将输入文本转换回 int
                            AimAssist->BaudRate = std::stoi(BPro_PorterBuf);
                        }
                    } else if (AimAssist->KmboxType == "Net") {
                        ImGui::InputTextEx("KmboxIP", "...", AimAssist->KmboxIP, IM_ARRAYSIZE(AimAssist->KmboxIP), ImVec2(ImGui::GetContentRegionMax().x - 100, 20), NULL);
                        ImGui::InputTextEx("KmboxPort", "...", AimAssist->KmboxPort, IM_ARRAYSIZE(AimAssist->KmboxPort), ImVec2(ImGui::GetContentRegionMax().x - 100, 20), NULL);
                        ImGui::InputTextEx("KmboxUUID", "...", AimAssist->KmboxUUID, IM_ARRAYSIZE(AimAssist->KmboxUUID), ImVec2(ImGui::GetContentRegionMax().x - 100, 20), NULL);
                    }
                    // 示范：打印当前选中的 KmboxType
                    // ImGui 按钮
                    if (ImGui::Button("Initialize KmBox")) {
                        AimAssist->Initialize(); // 手动执行初始化
                    }
                    if (AimAssist->KmboxInitialized) {
                        ImGui::TextColored(ImVec4(0, 1, 0, 0.5), "Initialization Successful"); // GREEN
                    } else {
                        ImGui::TextColored(ImVec4(1, 0, 0, 0.5), "Initialization Failure"); // RED
                    }

                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Glow"))
                {
                    if (ImGui::TreeNode("Player Glow")) {

                        ImGui::ColorEdit3("Glow Knocked", (float*)&ESP->GlowKnocked);
                        ImGui::ColorEdit3("Glow Invisible", (float*)&ESP->GlowInvisible);
                        ImGui::ColorEdit3("Glow Visible", (float*)&ESP->GlowVisible);
                        ImGui::ColorEdit3("Glow Admin", (float*)&ESP->GlowAdmin);
                        ImGui::SliderFloat("Highlight Scale", &ESP->HighlightScale, 1, 255, "%.0f");

                        ImGui::Checkbox("Enable Glow", &ESP->enableGlow);

                        ImGui::SliderInt("insidetype", &ESP->insidetype, 0, 255);

                        // 创建一个滑动块来控制轮廓风格
                        ImGui::SliderInt("outlinetype", &ESP->outlinetype, 0, 255);

                        // 创建一个滑动块来控制轮廓大小
                        ImGui::SliderInt("outlinesize", &ESP->outlinesize, 0, 255);
                        ImGui::Separator();
                        ImGui::Checkbox("Based on Shield", &ESP->checkShield);

                        ImGui::TreePop();
                    }
                    if (ImGui::TreeNode("Item Glow"))
                    {
                        ImGui::Checkbox("Enable ItemGlow", &ESP->ItemGlow);
                        ImGui::Checkbox("Ammo", &Item_Ammo);
                        ImGui::SameLine();
                        ImGui::Checkbox("Weapons", &Item_Weapons);

                        ImGui::Checkbox("Grey", &Item_Grey);
                        ImGui::SameLine();
                        ImGui::Checkbox("Blue", &Item_Blue);

                        ImGui::Checkbox("Purple", &Item_Purple);
                        ImGui::SameLine();
                        ImGui::Checkbox("Gold", &Item_Gold);
                        ImGui::SameLine();
                        ImGui::Checkbox("Red", &Item_Red);

                        ImGui::TreePop();
                    }

                    ImGui::Separator();
                }
                if (ImGui::CollapsingHeader("Config System"))
                {
                    const std::string configFolderPath = "Config/";
                    std::vector<std::string> configFileNames = getFilesInDirectory(configFolderPath);

                    static int selectedFileIndex = 0; // 初始化选中第一个文件
                    static std::string currentConfigPath = configFileNames.empty() ? "" : (configFolderPath + configFileNames[0]);

                    // 显示下拉框让用户选择配置文件
                    if (ImGui::BeginCombo("Config", configFileNames.empty() ? "No Config File" : configFileNames[selectedFileIndex].c_str())) {
                        for (size_t i = 0; i < configFileNames.size(); ++i) {
                            if (ImGui::Selectable(configFileNames[i].c_str(), selectedFileIndex == i)) {
                                selectedFileIndex = i;
                                currentConfigPath = configFolderPath + configFileNames[i]; // 更新路径
                            }
                        }
                        ImGui::EndCombo();
                    }

                    // 保存按钮逻辑
                    if (ImGui::Button("Save Config")) {
                        if (!currentConfigPath.empty()) {
                            Config::GetInstance().Save(currentConfigPath); // 调用保存配置方法到当前选中的文件路径
                        }
                    }
                    ImGui::SameLine();
                    // 加载按钮逻辑
                    if (ImGui::Button("Load Config")) {
                        if (!currentConfigPath.empty()) {
                            Config::GetInstance().LoadConfig(currentConfigPath); // 调用加载配置方法从当前路径加载
                        }
                    }

                    // 添加一个输入文本框用于输入文件名
                    static char inputFileName[256] = "";
                    ImGui::InputText("File Name", inputFileName, IM_ARRAYSIZE(inputFileName));

                    ImGui::SameLine();
                    // 添加一个按钮，点击后创建文件
                    if (ImGui::Button("Create")) {
                        if (strlen(inputFileName) > 0) {
                            std::string newFilePath = configFolderPath + std::string(inputFileName) + ".cfg";
                            std::ofstream newFile(newFilePath);
                            if (newFile.is_open()) {
                                newFile.close();
                                std::cout << "Created successfully: " << newFilePath << std::endl;
                                configFileNames = getFilesInDirectory(configFolderPath); // 重新加载文件列表
                                selectedFileIndex = configFileNames.size() - 1;
                                currentConfigPath = newFilePath;
                            } else {
                                std::cout << "Creation failure" << std::endl;
                            }
                        } else {
                            std::cout << "Please enter the file name!" << std::endl;
                        }
                    }

//                    if (ImGui::Button("test"))
//                    {
//                        RList.AddUIDsToJSON("uid.txt", "list_Admin.json");
//                    }
                }
                ImGui::Separator();

                ImGui::Checkbox("Fusion Mode[INS]", &isBlenderChecked);
                ImGui::SameLine();
                ImGui::Checkbox("Enable VSync[DEL]", &EnableVSync);
                ImGui::TextColored(ImVec4(0,1,1,0.5),"[HOME] Show/Hide Menu [END] Exit");
                ImGui::SameLine();
                ImGui::Text("FPS:%.1f", io.Framerate);

                if (ImGui::Button("Fresh Data[F5]")) {
                    VMMDLL_ConfigSet(mem.vHandle, VMMDLL_OPT_REFRESH_ALL, 1);
                    RList.adminList.clear();
                    RList.whiteList.clear();
                    RList.blackList.clear();
                    RList.loadLists();
                    Players->clear();
                    Globalarray.clear();
                    PlayersPopulated = false;
                }

                ImGui::SameLine();

                if (ImGui::Button("Re-initialize DMA")) {
                    mem.~Memory();
                    Sleep(200);
                    if (!mem.Init("r5apex.exe", true, false))
                    {
                        std::cout << "[!] DMA REINT FAILED" << std::endl;
                        std::cout << "[#] PRESS ENTER TO EXIT...";
                        std::cin.get();
                    }
                    std::cout << "[#] DMA REINT SUCCESS" << std::endl;

                    if (!mem.GetKeyboard()->InitKeyboard())
                    {
                        std::cout << "[!] KEYBOARD REINT FAILED" << std::endl;
                        std::cout << "[#] PRESS ENTER TO EXIT...";
                        std::cin.get();
                    }
                    std::cout << "[#] KEYBOARD REINT SUCCESS" << std::endl;
                }
                ImGui::SameLine();
                Helpmarker("Refind the process and initialize DMA, too many clicks will crash the software", ImVec4(0.0f,1.0f,0.0f,1.0f));
                ImGui::SameLine();
                if (ImGui::Button("EXIT")) {
                    Players->clear();
                    Globalarray.clear();
                    ImGui_ImplDX11_Shutdown();
                    ImGui_ImplWin32_Shutdown();
                    ImGui::DestroyContext();
                    mem.~Memory();//释放DMA进程
                    Sleep(500);
                    exit(0);  // 退出程序
                }

            }
            ImGui::End();
        }
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w,
                                                  clear_color.z * clear_color.w, clear_color.w };
        g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
        g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
        }

        g_pSwapChain->Present(EnableVSync ? 1 : 0, 0);
    }
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext(g_ImGuiContext);  // 销毁 ImGui 上下文

    CleanupDeviceD3D();
    ::DestroyWindow(hwnd);
    ::UnregisterClass(reinterpret_cast<LPCSTR>(wc.lpszClassName), wc.hInstance);

    return;
}

extern IMGUI_IMPL_API LRESULT
ImGui_ImplWin32_WndProcHandler(HWND
                               hWnd,
                               UINT msg, WPARAM
                               wParam,
                               LPARAM lParam
);

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    switch (msg) {
        case WM_SIZE:
            if (wParam == SIZE_MINIMIZED)
                return 0;
            g_ResizeWidth = (UINT)LOWORD(lParam); // Queue resize
            g_ResizeHeight = (UINT)HIWORD(lParam);
            return 0;
        case WM_SYSCOMMAND:
            if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
                return 0;
            break;
        case WM_DESTROY:
            ::PostQuitMessage(0);
            return 0;
    }
    return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}
