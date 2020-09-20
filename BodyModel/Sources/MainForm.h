#pragma once
#include "Form.h"
#include <vector>

class MainForm : public Form
{
private:
	bool show_main_menu = true;
	bool show_setting_window = false;
	bool show_level_window = false;
	bool messageBoxOpen = false;
	float alpha = 0.5f;
	//Setting Flags
	bool aduio_On = true;
	float speed = 100.f;

	//Level Flags
	bool levelSelected = false;
	int currentLevel = 0;

	//Loading Bar
	bool show_loading_bar=false;
	bool show_CountDown = false;
	int countDown = 3;
	int nanoSeconds = 300;
	int maxNanoSeconds = 300;
	bool switchCountDown = false;
	bool showHighScore = false;
	bool showKindOfAttacking = false;

	bool showEnd = false;

	

public:
	MainForm();
	void drawForm();
	void displayLoading();

	void displayEnd();

	std::vector<char*> levels;
	int selectedLevel = 0;

	//Loading Bar
	float progress = 0.f;
	int highScore = 0;
	char* kindOfAttacking = NULL;
	
};
