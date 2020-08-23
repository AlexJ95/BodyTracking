#pragma once
#include "Form.h"
class MainForm : public Form
{
private:
	int counter = 0;
	bool show_demo_window = true;
	bool show_another_window = false;
public:
	MainForm();
	void drawForm();
};
