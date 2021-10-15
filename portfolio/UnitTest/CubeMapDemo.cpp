#include "stdafx.h"
#include "CubeMapDemo.h"
#include "../Framework/Viewer/MoveCamera.h"
#include "../Framework/Objects/CubeMap.h"

void CubeMapDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(13, 0, 0);
	Context::Get()->GetCamera()->Position(0, 5, -19);
	shader = new Shader(L"25_Mesh.fx");
	
	shader->AsVector("Direction")->SetFloatVector(direction);
	sDirection = shader->AsVector("Direction");
	/*quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");*/

	shaderCubeMap = new Shader(L"29_CubeMap.fx");
	
	cubeMap = new CubeMap(shaderCubeMap);
	cubeMap->Texture(L"Environment/Earth.dds");
	cubeMap->GetTransform()->Position(0, 5, 0);


	grid = new MeshGrid(shader);
	grid->DiffuseMap(L"Floor.png");

	cube = new MeshCube(shader);
	cube->DiffuseMap(L"Box.png");
	cube->GetTransform()->Position(0, 5, 0);

	cylinder = new MeshCylinder(shader, 0.5f, 3.0f, 20, 20);
	cylinder->DiffuseMap(L"Bricks.png");
	cylinder->GetTransform()->Position(-5, 5, 0);
	
	sphere = new MeshSphere(shader, 0.5f);
	sphere->DiffuseMap(L"Stones.png");
	sphere->GetTransform()->Position(-10, 5, 0);
}


void CubeMapDemo::Destroy()
{
	SafeDelete(shader);
	SafeDelete(quad);
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);
	SafeDelete(cubeMap);
}

void CubeMapDemo::Update()
{
	Vector3 scale;
	
	/*quad->Scale(&scale);
	ImGui::SliderFloat3("Scale", scale, -3, 3);
	quad->Scale(scale);
	quad->Update();*/

	cube->GetTransform()->Scale(&scale);
	ImGui::SliderFloat3("Scale", scale, 1, 100);
	cube->GetTransform()->Scale(scale);
	cube->Update();
	grid->Update();
	sphere->Update();
	cylinder->Update();
}

void CubeMapDemo::Render()
{
	//sDirection->SetFloatVector(direction);
	//quad->Render();
	sDirection->SetFloatVector(direction);
	cube->Render();

	static int pass = 1;
	ImGui::InputInt("Pass", &pass);
	pass %= 2;
	grid->Pass(pass);
	grid->Render();

	sphere->Render();
	cylinder->Render();

	cubeMap->Render();
}

