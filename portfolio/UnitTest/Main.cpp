#include "stdafx.h"
#include "Main.h"
#include "Systems/Window.h"

#include "WeatherDemo.h"
#include "BillboardDemo.h"
#include "NormalMappingDemo.h"
#include "SpotLightingDemo.h"
#include "PointLightingDemo.h"
#include "LightingDemo.h"
#include "ColorToneDemo.h"
#include "MRTDemo.h"
#include "BlurDemo.h"
#include "GaussianBlurDemo.h"
#include "GaussianBlurDemo2.h"
#include "ShadowDemo.h"
#include "HeightMapDemo.h"
#include "DynamicCubeMapDemo.h"
#include "ThreadDemo.h"
#include "CharacterControllDemo.h"

void Main::Initialize()
{
	//Push(new WeatherDemo());
	//Push(new GaussianBlurDemo());
	//Push(new ShadowDemo());
	Push(new CharacterControllDemo());

	//Push(new NormalMappingDemo());
	//Push(new SpotLightingDemo());
	//Push(new PointLightingDemo());
	//Push(new LightingDemo());
}

void Main::Ready()
{

}

void Main::Destroy()
{
	for (IExecute* exe : executes)
	{
		exe->Destroy();
		SafeDelete(exe);
	}
}

void Main::Update()
{
	for (IExecute* exe : executes)
		exe->Update();
}

void Main::PreRender()
{
	for (IExecute* exe : executes)
		exe->PreRender();
}

void Main::Render()
{
	for (IExecute* exe : executes)
		exe->Render();
}

void Main::PostRender()
{
	for (IExecute* exe : executes)
		exe->PostRender();
}

void Main::ResizeScreen()
{
	for (IExecute* exe : executes)
		exe->ResizeScreen();
}

void Main::Push(IExecute * execute)
{
	executes.push_back(execute);

	execute->Initialize();
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prevInstance, LPSTR param, int command)
{
	D3DDesc desc;
	desc.AppName = L"D3D Game";
	desc.Instance = instance;
	desc.bFullScreen = false;
	desc.bVsync = false;
	desc.Handle = NULL;
	desc.Width = 1280;
	desc.Height = 720;
	desc.Background = Color(0.3f, 0.3f, 0.3f, 1.0f);
	D3D::SetDesc(desc);

	Main* main = new Main();
	WPARAM wParam = Window::Run(main);

	SafeDelete(main);

	return wParam;
}