#include "UI3D.h"
#include <Kore/System.h>


void UIshutDown()
{
	ImGui_ImplDX11_Shutdown();
}

//https://stackoverflow.com/questions/31648180/c-changing-hwnd-window-procedure-in-runtime
WNDPROC prevwnd;
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK WndProc2(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam);
	return CallWindowProc(prevwnd, hWnd, uMsg, wParam, lParam);
}


UI3D::UI3D(Kore::Window *window)
{
	this->window = window;
	initUI();
}

UI3D::~UI3D(){}


void UI3D::initUI()
{
	//chnge the wndProc for the ImGUI
	HWND hwnd = this->window->_data.handle;
	prevwnd = (WNDPROC)SetWindowLongPtr(hwnd, GWLP_WNDPROC,(LONG_PTR)WndProc2);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplDX11_Init(device, context);
	ImGui::StyleColorsDark();
}

//https://www.youtube.com/watch?v=Btx_tujnyB4&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=35
void UI3D::drawUI()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();


	////////////////////////////////////////////////////////////
	//here we create 3DUI with ImGUI
	ImGui::Begin("Test");

	ImGui::Text("This is an example Text.");
	if (ImGui::Button("Click me"))
		counter += 1;
	std::string clickCount = "Click Count " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());

	ImGui::End();
	//Creating End
	////////////////////////////////////////////////////////////

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}


bool UI3D::lockBackground()
{
	return ImGui::IsAnyWindowHovered();
}