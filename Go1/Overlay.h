#pragma once
#include <Windows.h>
#include "imgui.h"
#include "backends/imgui_impl_win32.h"
#include "backends/imgui_impl_dx11.h"
#include <d3d11.h>
#include <dwmapi.h>
#include <stdio.h>

#pragma comment(lib, "D3D11.lib")

class Overlay
{
public:
	bool Create();
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void Clear();
	void DrawFilledRect(float x1, float y1, float x2, float y2, ImColor color);
	void DrawLine(float x1, float y1, float x2, float y2, ImColor color, float thinkness = 1.f);
	void DrawRect(float x1, float y1, float x2, float y2, ImColor color, float thinkness = 1.f);
	void DrawTxt(float x, float y, ImColor color, const char* text);
public:
	HWND hwnd = nullptr;
	ID3D11Device* pd3dDevice = nullptr;
	ID3D11DeviceContext* pd3dDeviceContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;
	ID3D11RenderTargetView* mainRenderTargetView = nullptr;
	LPCTSTR windowName = nullptr;
	HINSTANCE hInstance = nullptr;
	int width = 0;
	int height = 0;
};