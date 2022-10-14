#include "Hack.h"

bool Hack::running = true;

void Hack::TopMost(HWND hWnd)
{
    while (running)
    {
        HWND wnd = GetWindow(GetForegroundWindow(), GW_HWNDPREV);;
        if (wnd != hWnd)
        {
            SetWindowPos(hWnd, wnd, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOMOVE | SWP_NOSIZE);
        }
        Sleep(500);
    }
    ExitThread(0);
}

void Hack::TriggerBot(Hack* h)
{
    while (running)
    {
        if (h->triggerBot)
        {
            uint32_t local = h->GetLocal();
            if (local)
            {
                uint32_t id = h->GetEntity(h->GetCrossId(local) - 1);
                if (id && id != local && !h->IsDormant(id) && h->GetHealth(id) > 0 && h->GetTeam(local) != h->GetTeam(id))
                {
                    Sleep(10);
                    INPUT Input = { 0 };
                    Input.type = INPUT_MOUSE;
                    Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
                    SendInput(1, &Input, sizeof(INPUT));
                    ZeroMemory(&Input, sizeof(INPUT));
                    Input.type = INPUT_MOUSE;
                    Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
                    SendInput(1, &Input, sizeof(INPUT));
                }
            }     
        }
        Sleep(1);
    }
    ExitThread(0);
}

void Hack::AimBot(Hack* h)
{
    Vec3 oaim = { 0.f, 0.f };
    while (running)
    {
        if (h->aimBot)
        {
           //if (GetAsyncKeyState(VK_LBUTTON) || h->triggerBot)
           //{
           //    if (h->bestId)
           //    {
           //        Vec3 pos = h->GetBonePos(h->bestId, 8);
           //        Vec2 screen;
           //        if (h->WorldToScreen(pos, screen))
           //        {
           //            Vec2 aim = h->mem.Read<Vec2>(h->GetLocal() + hazedumper::netvars::m_aimPunchAngle);
           //            INPUT Input = { 0 };
           //            Input.type = INPUT_MOUSE;
           //            Input.mi.dwFlags = MOUSE_MOVED;
           //            Input.mi.dx = (screen.x - h->width / 90 * (aim.x-oaim.x) - h->width / 2);
           //            Input.mi.dy = (screen.y + h->height / 90 * (aim.y-oaim.y) - h->height / 2);
           //            SendInput(1, &Input, sizeof(INPUT));
           //            oaim = aim;
           //        }
           //    }
           //}
           //else
           //{
           //    oaim = { 0.f, 0.f };
           //}

           //Vec3 aim = h->mem.Read<Vec3>(h->GetLocal() + hazedumper::netvars::m_aimPunchAngle);
           //if (GetAsyncKeyState(VK_LBUTTON)) {
           //    
           //    Vec3 view = h->GetViewAngle();
           //    Vec3 NewAngles = view - (aim - oaim)*2.f; 
           //    h->SetViewAngle(NewAngles);
           //}
           //oaim = aim;

            if (GetAsyncKeyState(VK_LBUTTON) || h->triggerBot)
            {
                if (h->bestId)
                {
                    Vec3 aim = h->mem.Read<Vec3>(h->GetLocal() + hazedumper::netvars::m_aimPunchAngle);
                    Vec3 angle = h->GetPos(h->GetLocal()) + h->GetCamera(h->GetLocal()) - h->GetBonePos(h->bestId, 8);
                    angle = angle.to_angle();
                    angle.Clamp();
                    angle = angle - aim * 2.f;
                    angle.Clamp();
                    Vec3 view = h->GetViewAngle();
                    Vec3 delta = angle - view;
                    delta.Clamp();
                    Vec3 newAngle = view + delta * 0.5f;
                    newAngle.Clamp();
                    h->SetViewAngle(newAngle);
                } 
            }
        }
        Sleep(1);
    }
    ExitThread(0);
}

void Hack::ShowMenu()
{
    if (esp)
    {
        ov.DrawTxt(10, 30, blue, u8"透视:     打开");
    }
    else
    {
        ov.DrawTxt(10, 30, blue, u8"透视:     关闭");
    }
    if (triggerBot)
    {
        ov.DrawTxt(10, 50, blue, u8"自动开枪: 打开");
    }
    else
    {
        ov.DrawTxt(10, 50, blue, u8"自动开枪: 关闭");
    }
    if (aimBot)
    {
        ov.DrawTxt(10, 70, blue, u8"自瞄:     打开");
    }
    else
    {
        ov.DrawTxt(10, 70, blue, u8"自瞄:     关闭");
    }
}

void Hack::Control(Hack* h)
{
    while (running)
    {
        if (GetAsyncKeyState(VK_END))
        {
            running = false;
        }
        if (GetAsyncKeyState(VK_F1) & 0x8000)
        {
            h->esp = !h->esp;
        }
        if (GetAsyncKeyState(VK_F2) & 0x8000)
        {
            h->triggerBot = !h->triggerBot;
        }
        if (GetAsyncKeyState(VK_F4) & 0x8000)
        {
            h->aimBot = !h->aimBot;
        }
        Sleep(100);
    }
    ExitThread(0);
}

void Hack::Esp()
{
    if (esp)
    {
        uint32_t local = GetLocal();
        if (local)
        {
            int bestFov = 100;
            for (uint32_t i = 0; i < 32; i++)
            {
                uintptr_t id = GetEntity(i);
                if (id && id != local && !IsDormant(id) && GetHealth(id) > 0 && GetTeam(local) != GetTeam(id))
                {
                    Vec3 pos = GetPos(id);
                    Vec2 screen;
                    if (WorldToScreen(pos, screen))
                    {
                        ov.DrawLine(width / 2, height, screen.x, screen.y, white);
                        Vec3 head = pos;
                        float bl = 1.f;
                        if (GetFlag(id) != 263)
                        {
                            head.z += 72.f;
                            bl = 1.5f;
                        }
                        else
                        {
                            head.z += 54.f;
                            bl = 1.125f;
                        }
                        Vec2 headScreen;
                        if (WorldToScreen(head, headScreen))
                        {
                            float width_ = fabsf(screen.y - headScreen.y) / bl;
                            float health = GetHealth(id);
                            float bar = health / 100.f * width_;
                            ov.DrawRect(screen.x - width_ / 2.f, screen.y, screen.x + width_ / 2.f, headScreen.y, green, 2.f);
                            ov.DrawRect(screen.x - width_ / 2.f, screen.y + 4.f, screen.x + width_ / 2.f, screen.y + 9.f, green);
                            ov.DrawFilledRect(screen.x - width_ / 2.f, screen.y + 5.f, screen.x - width_ / 2 + bar, screen.y + 8.f, red);
                            int fov = (int)sqrtf(powf(headScreen.x - width / 2.f, 2) + powf(headScreen.y - height / 2.f, 2));
                            if (fov < bestFov)
                            {
                                bestFov = fov;
                                bestId = id;
                            }
                        } 
                    }
                }
            }
            if (bestFov == 100)
            {
                bestId = 0;
            }
        }
    }
}

int Hack::Run()
{
    if (!mem.Init(xor (TEXT("csgo.exe"))))
    {
        return 1;
    }
    clinetAddress = mem.GetModuleAddress(xor (TEXT("client.dll")));
    engineAddress = mem.GetModuleAddress(xor (TEXT("engine.dll")));
    if (!clinetAddress || !engineAddress)
    {
        wprintf_s(L"找不到地址\n");
        return 1;
    }
    
    ov.Create();
    width = ov.width;
    height = ov.height;
    // Our state
    ImVec4 clear_color = ImVec4(0.f, 0.f, 0.f, 0.f);
    CloseHandle(CreateThread(0, 0, (PTHREAD_START_ROUTINE)TopMost, ov.hwnd, 0, 0));
    CloseHandle(CreateThread(0, 0, (PTHREAD_START_ROUTINE)TriggerBot, this, 0, 0));
    CloseHandle(CreateThread(0, 0, (PTHREAD_START_ROUTINE)Control, this, 0, 0));
    CloseHandle(CreateThread(0, 0, (PTHREAD_START_ROUTINE)AimBot, this, 0, 0));

    while (running)
    {
        MSG msg;
        if (::PeekMessageW(&msg, NULL, 0U, 0U, PM_REMOVE))
        {
            ::TranslateMessage(&msg);
            ::DispatchMessageW(&msg);
            if (msg.message == WM_QUIT)
                running = false;
        }

        // Start the Dear ImGui frame
        ImGui_ImplDX11_NewFrame();
        ImGui_ImplWin32_NewFrame();
        ImGui::NewFrame();
       
        static char txt[100];
        sprintf_s(txt, "%.1f FPS", ImGui::GetIO().Framerate);
        ov.DrawTxt(10, 10, ImColor(255, 255, 255), txt);
        ShowMenu();
        Esp();

        // Rendering
        ImGui::Render();
        const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
        ov.pd3dDeviceContext->OMSetRenderTargets(1, &ov.mainRenderTargetView, NULL);
        ov.pd3dDeviceContext->ClearRenderTargetView(ov.mainRenderTargetView, clear_color_with_alpha);
        ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

        ov.pSwapChain->Present(1, 0); // Present with vsync
        //ov.pSwapChain->Present(0, 0); // Present without vsync
    }
    ov.Clear();
    return 0;
}
