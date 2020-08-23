#include "MainForm.h"


MainForm::MainForm() 
{
};
void MainForm::drawForm()
{
	///////First Window//////////////////
	ImGui::Begin("Test");

	ImGui::Text("This is an example Text.");
	if (ImGui::Button("Click me"))
		counter += 1;
	std::string clickCount = "Click Count " + std::to_string(counter);
	ImGui::Text(clickCount.c_str());
	ImGui::Checkbox("Another Window", &show_another_window);
	ImGui::End();
	///////First Window End///////////////

	///////second Window//////////////////


	///////Second Window End//////////////
	if (show_another_window)
	{
		ImGui::Begin("Another Window", &show_another_window);
		ImGui::Text("Hello from another Window");
		if (ImGui::Button("Close Me"))
			show_another_window = false;
		ImGui::End();
	}

	////////////////////////////////////////////////////////////
};