#pragma once
#include "pch.h"

#include <Kore/System.h>
#include <Kore/Direct3D11.h>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"
#include "Form.h"

void UIshutDown();

class UI3D
{
private:
	void initUI();
	Kore::Window* window;
		
	bool show_demo_window = true;
	bool show_another_window = false;
	ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
	
	//UI3D inkrementieren
	int counter=0;
public:
	UI3D(Kore::Window* window);
	~UI3D();
	void drawUI(Form* form);
	bool lockBackground();
};
