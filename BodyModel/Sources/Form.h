#pragma once
#include "pch.h"
#include <Kore/System.h>
#include <Kore/Direct3D11.h>
#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_dx11.h"
#include "ImGUI/imgui_impl_win32.h"

class Form
{
private:
public:
	Form();
	void virtual drawForm();
};
