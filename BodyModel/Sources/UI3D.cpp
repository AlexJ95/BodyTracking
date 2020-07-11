#include "UI3D.h"
#include <Kore/System.h>
#include <Kore/Graphics4/PipelineState.h>
#include <Kore/IO/FileReader.h>
#include <Kore/Math/Quaternion.h>

using namespace Kore;
using namespace Kore::Graphics4;

VertexStructure structure;
TextureUnit tex;


ConstantLocation pLocation;
ConstantLocation vLocation;
ConstantLocation mLocation;
Shader* vertShader;
Shader* fragShader;
Kore::Graphics4::PipelineState* pipeline;
float vertices[12]{
5.f,-5.f,-10.f,
5.f,5.f,-10.f,
-5.f,5.f,-10.f,
-5.f,-5.f,-10.f
};


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


UI3D::UI3D(Kore::Window *window, bool rendering3D)
{
	this->rendering3D = rendering3D;
	this->window = window;
	initUI();
}

UI3D::~UI3D(){}

void UI3D::loadBillboard()
{

	VertexBuffer* vertBuffer = new VertexBuffer(8, structure);
	IndexBuffer* indexBuffer = new IndexBuffer(4);
	//Texture* image = new Texture("plattform/Textures/blue.png");



	float* verticesX = vertBuffer->lock();
	int* indicesX = indexBuffer->lock();

	for (int i = 0; i < 4; i++)
	{
		verticesX[i] = vertices[i];
		verticesX[i + 1] = vertices[i + 1];
		verticesX[i + 2] = vertices[i + 2];

		verticesX[i + 3] = 1.f;
		verticesX[i + 4] = 0.f;
		verticesX[i + 5] = 0.f;

		indicesX[i] = i;
	}

	//Graphics4::setTexture(tex, image);
	Graphics4::setVertexBuffer(*vertBuffer);
	Graphics4::setIndexBuffer(*indexBuffer);
	
}
void UI3D::loadShaders()
{
	FileReader vs("shader.vert");
	FileReader fs("shader.frag");

	vertShader = new Shader(vs.readAll(), vs.size(), VertexShader);
	fragShader = new Shader(fs.readAll(), fs.size(), FragmentShader);

	structure.add("pos", Float3VertexData);
	//structure.add("tex", Float2VertexData);
	//structure.add("nor", Float3VertexData);
	structure.add("col", ColorVertexData);

	pipeline = new PipelineState();
	pipeline->inputLayout[0] = &structure;
	pipeline->inputLayout[1] = nullptr;
	pipeline->vertexShader = vertShader;
	pipeline->fragmentShader = fragShader;
	pipeline->depthMode = ZCompareLess;
	pipeline->depthWrite = true;
	pipeline->blendSource = Graphics4::SourceAlpha;
	pipeline->blendDestination = Graphics4::InverseSourceAlpha;
	pipeline->alphaBlendSource = Graphics4::SourceAlpha;
	pipeline->alphaBlendDestination = Graphics4::InverseSourceAlpha;
	pipeline->compile();

	//tex = pipeline->getTextureUnit("tex");
	//Graphics4::setTextureAddressing(tex, Graphics4::U, Repeat);
	//Graphics4::setTextureAddressing(tex, Graphics4::V, Repeat);

	pLocation = pipeline->getConstantLocation("P");
	vLocation = pipeline->getConstantLocation("V");
	mLocation = pipeline->getConstantLocation("M");
}
void UI3D::render3D()
{
	Kore::Graphics4::setPipeline(pipeline);
	Graphics4::setMatrix(vLocation, V);
	Graphics4::setMatrix(pLocation, P);

	Kore::Quaternion plattformRot = Kore::Quaternion(0, 0, 0, 1);
	plattformRot.rotate(Kore::Quaternion(vec3(1, 0, 0), -Kore::pi / 2.0));
	Kore::mat4 M = Kore::mat4::Identity();
	M = mat4::Translation(0, 0, 0) * plattformRot.matrix().Transpose() * mat4::Scale(0.5f, 0.5f, 0.01f);
	Graphics4::setMatrix(mLocation, M);
	Graphics4::drawIndexedVertices();
}

void UI3D::setViewProj(Kore::mat4 V, Kore::mat4 P)
{
	this->V = V;
	this->P = P;
	draw3D = true;
}

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

	if (rendering3D)
	{
		this->loadShaders();
		this->loadBillboard();
	}
}

//https://www.youtube.com/watch?v=Btx_tujnyB4&list=PLcacUGyBsOIBlGyQQWzp6D1Xn6ZENx9Y2&index=35
void UI3D::drawUI()
{
	if (draw3D)
	{
		render3D();
	}
	else 
	{
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();


		////////////////////////////////////////////////////////////
		//here we create 3DUI with ImGUI

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



		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	}
}


bool UI3D::lockBackground()
{
	return ImGui::IsAnyWindowHovered();
}