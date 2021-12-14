#include "stdafx.h"
#include "Viewer.h"
#include "Viewer/MoveCamera.h"
#include "Viewer/Camera.h"
#include "Renders/Context.h"


void Viewer::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	((MoveCamera *)Context::Get()->GetCamera())->Speed(50, 2);

	shader = new Shader(L"82_NormalMapping.fx");

	sky = new CubeSky(L"Environment/GrassCube1024.dds");

	particleSystem = new ParticleSystem(L"Explosion");

	CreateMesh();
}


void Viewer::Destroy()
{
	
}

void Viewer::Update()
{
	sky->Update();
	sphere->Update();
	grid->Update();

	//static UINT selected = 0;
	//ImGui::InputInt("NormalMap Selected", (int *)&selected);
	//selected %= 4;

	//shader->AsScalar("Selected")->SetInt(selected);

	Vector3 position;
	sphere->GetTransform(0)->Position(&position);

	if (Keyboard::Get()->Press('L'))
		position.x += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('J'))
		position.x -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('I'))
		position.z += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('K'))
		position.z -= 20 * Time::Delta();

	if (Keyboard::Get()->Press('O'))
		position.y += 20 * Time::Delta();
	else if (Keyboard::Get()->Press('U'))
		position.y -= 20 * Time::Delta();

	sphere->GetTransform(0)->Position(position);
	sphere->UpdateTransforms();

	particleSystem->Add(position);

	particleSystem->Update();
}

void Viewer::Render()
{
	sky->Render();
	stone->Render();
	sphere->Render();

	floor->Render();
	grid->Render();

	particleSystem->Render();
}

void Viewer::CreateMesh()
{
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->Specular(1, 1, 1, 20);
	floor->SpecularMap("Floor_Specular.png");
	floor->NormalMap("Floor_Normal.png");

	stone = new Material(shader);
	stone->DiffuseMap("Stones.png");
	stone->Specular(1, 1, 1, 20);
	stone->SpecularMap("Stones_Specular.png");
	stone->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	stone->NormalMap("Stones_Normal.png");

	Transform* transform = NULL;

	grid = new MeshRender(shader, new MeshGrid(5, 5));
	transform = grid->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(12, 1, 12);

	sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
	transform = sphere->AddTransform();
	transform->Position(0, 5, 0);
	transform->Scale(5, 5, 5);
	
	grid->UpdateTransforms();
	sphere->UpdateTransforms();
	
}