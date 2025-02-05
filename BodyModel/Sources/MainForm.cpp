#include "MainForm.h"


//Here we build the Gui. ImGui is an immediate mode API, that means we use boolean variables to show some windows and to hide others. 
//The whole Gui will drawn together with the rest of the scene at the same time. We use many if-conditions to go through the states.
MainForm::MainForm() {
	levels.push_back("Level 0");
	levels.push_back("Level 1");
	levels.push_back("Level 2");
	levels.push_back("Level 3");
};

void MainForm::drawForm()
{
	ImGuiIO io = ImGui::GetIO();
	ImVec2 contentSize = io.DisplaySize;
	ImGui::SetNextWindowBgAlpha(alpha);
	
	//ImGui Flags in imgui.h
	if (show_main_menu & !show_setting_window & !show_level_window & !show_loading_bar)
	{
		showingForm = true;
		ImGui::Begin("Menu", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Menu", ImVec2(400, 130), 0);
		ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
		ImGui::SetWindowPos(wPos);

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);
		if (ImGui::Button("Start", ImVec2(ImGui::GetWindowSize().x * 0.8f, 0.0f)))
			show_loading_bar = true;

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);
		if (ImGui::Button("Setting", ImVec2(ImGui::GetWindowSize().x * 0.8f, 0.0f)))
			show_setting_window = true;

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);
		if (ImGui::Button("Choose Level", ImVec2(ImGui::GetWindowSize().x * 0.8f, 0.0f)))
			show_level_window = true;

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);
		if (ImGui::Button("Quit", ImVec2(ImGui::GetWindowSize().x * 0.8f, 0.0f)))
		{
			
			messageBoxOpen = true;
		}
			

		ImGui::End();

		if (messageBoxOpen)
		{
			ImGui::SetNextWindowBgAlpha(alpha+0.3);
			ImGui::Begin("Quit Game", &messageBoxOpen, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
			ImGui::SetWindowSize("Quit Game", ImVec2(200, 80), 0);
			ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
			ImGui::SetWindowPos(wPos);
			ImGui::Text("Are you sure?");
			
			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 20 - ImGui::GetWindowSize().y * 0.1f);
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);
			if (ImGui::Button("Quit", ImVec2(60, 20)))
			{
				Kore::System::stop();
			}
			ImGui::SameLine();
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x-60-ImGui::GetWindowSize().x * 0.1f);
			if (ImGui::Button("Cenecel", ImVec2(60, 20)))
			{
				messageBoxOpen = false;
			}
			ImGui::End();
		}
	}

	if (show_setting_window)
	{
		ImGui::Begin("Setting", &show_setting_window, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Setting", ImVec2(400, 300), 0);
		ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
		ImGui::SetWindowPos(wPos);
		ImGui::Checkbox("Audio", &aduio_On);
		ImGui::Text("Speed:");
		static int e =0;
		ImGui::RadioButton("50%", &e, 0); ImGui::SameLine();
		ImGui::RadioButton("75%", &e, 1); ImGui::SameLine();
		ImGui::RadioButton("100%", &e, 2);
		if (e == 0)
			speed = 50.f;
		else if (e == 1)
			speed = 75.f;
		else
			speed = 100.f;

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 90);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 50);
		if (ImGui::Button("Close", ImVec2(70, 30)))
		{
			show_setting_window = false;
		}
	}else if (show_level_window)
	{
		ImGui::Begin("Levels", &show_level_window, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Levels", ImVec2(400, 300), 0);
		ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
		ImGui::SetWindowPos(wPos);

		ImGui::ListBox("", &currentLevel, levels.data(), levels.size());

		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 180);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 50);
		if (ImGui::Button("Save", ImVec2(70, 30)))
		{
			show_level_window = false;
			levelSelected = true;
		}
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x - 90);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y - 50);
		if (ImGui::Button("Start", ImVec2(70, 30)))
		{
			show_level_window = false;
			levelSelected = true;
			show_loading_bar = true;
		}
		ImGui::End();
	}
	else if (levelSelected)
	{
		selectedLevel = currentLevel;
		levelSelected = false;
	}
	else if (show_loading_bar)
	{
		ImGui::Begin("Loading...", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);
		ImGui::SetWindowSize("Loading...", ImVec2(200, 60), 0);
		ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
		ImGui::SetWindowPos(wPos);


		ImVec2 pos = ImGui::GetCursorScreenPos();
		float height = 20;
		float width = ImGui::GetWindowWidth() - 2.f * (pos.x - ImGui::GetWindowPos().x);

		ImGui::GetWindowDrawList()->AddRectFilled(pos, ImVec2(pos.x + width * progress, pos.y + height), IM_COL32(0, 255, 0, 255));
		ImGui::GetWindowDrawList()->AddRect(pos, ImVec2(pos.x + width, pos.y + height), IM_COL32(255, 50, 0, 255));
		if (progress > 1.f)
		{
			show_loading_bar = false;
			show_main_menu = false;
			showingForm = false;
			progress = 0;
		}

		progress += 0.01;
		ImGui::End();
	}else if (show_CountDown)
	{
		ImGui::Begin("Count Down", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize("Count Down", ImVec2(100, 50), 0);
		ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
		ImGui::SetWindowPos(wPos);
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ImGui::GetWindowSize().x * 0.01);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y * 0.5f - ImGui::GetWindowSize().y * 0.1f);

		if (nanoSeconds > (maxNanoSeconds * (2.0 / 4.0)) && nanoSeconds <= (maxNanoSeconds * (3.0 / 4.0)) && !switchCountDown)
		{
			countDown--;
			switchCountDown = true;
		}
		else if (nanoSeconds > (maxNanoSeconds * (1.0 / 4.0)) && nanoSeconds <= (maxNanoSeconds * (2.0 / 4.0)) && switchCountDown)
		{
			countDown--;
			switchCountDown = false;
		}
		else if (nanoSeconds > 0 && nanoSeconds <= maxNanoSeconds * (1.0 / 7.0))
		{
			ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.28f);
			ImGui::Text("Action!");
		}
		else if (nanoSeconds <= 0)
		{
			show_CountDown = false;
			startGame = true;
			showHighScore = true;
			showKindOfAttacking = true;
		}
		else
		{
			std::string s = std::to_string(countDown);
			ImGui::Text(s.c_str());
		}


		ImGui::End();
		nanoSeconds--;
	}
	else if (showHighScore)
	{
		ImGui::Begin("Highscore", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
		ImGui::SetWindowSize("Highscore", ImVec2(100, 50), 0);
		ImVec2 wPos(contentSize.x - ImGui::GetWindowWidth() - 20, ImGui::GetWindowHeight());
		ImGui::SetWindowPos(wPos);
		ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.5f - ImGui::GetWindowSize().x * 0.01);

		std::string s = std::to_string(highScore);
		ImGui::Text(s.c_str());

		ImGui::End();
	}
	
	if (showEnd) {

	ImGui::Begin("End", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowSize("End", ImVec2(400, 130), 0);
	ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, contentSize.y * 0.3f - ImGui::GetWindowHeight() * 0.5f);
	ImGui::SetWindowPos(wPos);
	ImGui::SetCursorPosX(ImGui::GetWindowSize().x * 0.1f);

	std::string s = "Thank you for playing\n This is a project from Sier Chen,\n Sanel Hasic,\n Alexander Jaeger,\n Hanjo Schnellbaecher.\n You have reach a max points of "+std::to_string(highScore);
	ImGui::Text(s.c_str());

	ImGui::End();
	}

	if (kindOfAttacking != NULL && showKindOfAttacking)
	{
		ImGui::Begin("Attacking", 0, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoTitleBar);
		ImGui::SetWindowSize("Attacking", ImVec2(220, 50), 0);
		ImVec2 wPos(contentSize.x * 0.5f - ImGui::GetWindowWidth() * 0.5f, ImGui::GetWindowHeight() * 0.5f);
		ImGui::SetWindowPos(wPos);
		ImGui::SetCursorPosY(ImGui::GetWindowSize().y * 0.5f - ImGui::GetWindowSize().y * 0.1f);

		ImGui::Text(kindOfAttacking);

		ImGui::End();
	}
}
void MainForm::displayLoading()
{
	show_loading_bar = true;
	show_main_menu = true;
	showingForm = true;

	if (progress > 1.f)
	{
		show_loading_bar = false;
		show_main_menu = false;
		showingForm = false;
		show_CountDown = true;
	}

	progress += 0.01;

	if (nanoSeconds <= 0)
	{
		show_CountDown = false;
		startGame = true;
		showHighScore = true;
		showKindOfAttacking = true;
	}
	nanoSeconds--;
}	
void MainForm::displayEnd() {
	showEnd = true;
	showingForm = true;
}
void MainForm::addHighScore(int score) {
	highScore += score;
}
;