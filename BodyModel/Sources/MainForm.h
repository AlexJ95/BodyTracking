#pragma once
#include "Form.h"
#include <vector>

class MainForm : public Form
{
private:
	bool show_demo_window = true;
	bool show_setting_window = false;
	bool show_level_window = false;
	bool messageBoxOpen = false;
	//Setting Flags
	bool aduio_On = true;
	float speed = 100.f;

	//Level Flags
	std::vector<char*> levels;
	int currentLevel = 0;
public:
	MainForm();
	void drawForm();
};
