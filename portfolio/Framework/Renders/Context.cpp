#include "Framework.h"
#include "Context.h"
#include "Viewer/Viewport.h"
#include "Viewer/Perspective.h"
#include "Viewer/MoveCamera.h"

Context* Context::instance = NULL;

Context * Context::Get()
{
	//assert(instance != NULL);

	return instance;
}

void Context::Create()
{
	assert(instance == NULL);

	instance = new Context();
}

void Context::Delete()
{
	SafeDelete(instance);
}

Context::Context()
{
	D3DDesc desc = D3D::GetDesc();

	perspective = new Perspective(desc.Width, desc.Height);
	viewport = new Viewport(desc.Width, desc.Height);
	camera = new MoveCamera();
	//position = Vector3(0, 0, -10);

}

Context::~Context()
{
	SafeDelete(perspective);
	SafeDelete(viewport);
	SafeDelete(camera);
}

void Context::ResizeScreen()
{
	perspective->Set(D3D::Width(), D3D::Height());
	viewport->Set(D3D::Width(), D3D::Height());
}

void Context::Update()
{
	//ImGui::SliderFloat3("Position", position, -100, 100); //vetor3을 float* (float[3]배열)로 리턴가능
	//Vector3 forward(0, 0, 1);
	//Vector3 right(1, 0, 0);
	//Vector3 up(0, 1, 0);

	//D3DXMatrixLookAtLH(&view, &position, &(position + forward), &up);

	camera->Update();
}

void Context::Render()
{

	viewport->RSSetViewport();

	string str = string("FrameRate : ") + to_string(ImGui::GetIO().Framerate);
	Gui::Get()->RenderText(5, 5, 1, 1, 1, str);

	Vector3 Rotation;
	camera->RotationDegree(&Rotation);

	Vector3 Position;
	camera->Position(&Position);

	str = "Camera Rotation : ";
	str += to_string((int)Rotation.x) + ", " + to_string((int)Rotation.y) + ", " + to_string((int)Rotation.z);
	Gui::Get()->RenderText(5, 20, 1, 1, 1, str);

	str = "Camera Position : ";
	str += to_string((int)Position.x) + ", " + to_string((int)Position.y) + ", " + to_string((int)Position.z);
	Gui::Get()->RenderText(5, 30, 1, 1, 1, str);
}

Matrix Context::View()
{
	Matrix viewMatrix;
	camera->GetMatrix(&viewMatrix);

	return viewMatrix;
}

Matrix Context::Projection()
{
	Matrix projection;
	perspective->GetMatrix(&projection);

	return projection;
}
