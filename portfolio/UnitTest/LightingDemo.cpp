#include "stdafx.h"
#include "LightingDemo.h"
#include "../Framework/Viewer/MoveCamera.h"
#include "../Framework/Environment/CubeSky.h"

void LightingDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	cubeSky = new CubeSky(L"Environment/GrassCube1024.dds");

	//Performance performance;
	//performance.Start();

	shader = new Shader(L"75_Lighting.fx");

	//float t = performance.End();

	MessageBox(D3D::GetDesc().Handle, to_wstring(t).c_str(), L"", MB_OK);

	Mesh();
	Airplane();
	Kachujin();
	Colliders();
	Weapon();
}


void LightingDemo::Destroy()
{
	SafeDelete(shader);
	//SafeDelete(quad);
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);
	SafeDelete(cubeSky);
}

void LightingDemo::Update()
{

	ImGui::SliderFloat3("light Position", Context::Get()->Position(), -10, 10);
	ImGui::SliderFloat3("light Direction", Context::Get()->Direction(), -1, 1);
	//Weapon
	{
		Vector3 position;
		weaponInitTransform->Position(&position);
		ImGui::SliderFloat3("Weapon Position", position, -20, 20);

		Vector3 scale;
		weaponInitTransform->Scale(&scale);
		ImGui::SliderFloat3("Weapon Scale", scale, 0.1f, 3.0f);

		Vector3 rotation;
		weaponInitTransform->Rotation(&rotation);
		ImGui::SliderFloat3("Weapon Rotation", rotation, -1.0f, 1.0f);

		weaponInitTransform->Position(position);
		weaponInitTransform->Scale(scale);
		weaponInitTransform->Rotation(rotation);
	}
	//Collider
	{
		Vector3 position;
		colliderInitTransforms->Position(&position);
		ImGui::SliderFloat3("Collider Position", position, -20, 20);

		Vector3 scale;
		colliderInitTransforms->Scale(&scale);
		ImGui::SliderFloat3("Collider Scale", scale, 10.0f, 100.0f);

		Vector3 rotation;
		colliderInitTransforms->Rotation(&rotation);
		ImGui::SliderFloat3("Collider Rotation", rotation, -1.0f, 1.0f);

		colliderInitTransforms->Position(position);
		colliderInitTransforms->Scale(scale);
		colliderInitTransforms->Rotation(rotation);
	}

	//ImGui::SliderFloat3("Light Direction", Context::Get()->Direction(), -1, +1);
	cubeSky->Update();

	cube->Update();
	grid->Update();
	sphere->Update();
	cylinder->Update();

	airplane->Update();
	kachujin->Update();

	Matrix world[MAX_MODEL_TRANSFORMS];

	for (UINT i = 0; i < kachujin->GetTransformCount(); ++i)
	{
		//Matrix attach;
		//kachujin->GetAttachTransform(i, &attach);

		//colliders[i]->Collider->GetTransform()->World(attach);
		//colliders[i]->Collider->Update();
		kachujin->GetAttachTransform(i, world); //모델의 전체 본을 얻어온다.

		colliders[i]->Collider->GetTransform()->World(world[40]);
		colliders[i]->Collider->Update();

		weapon->GetTransform(i)->World(weaponInitTransform->World() * world[40]);
	}
	weapon->UpdateTransforms();
	weapon->Update();
}

void LightingDemo::Render()
{
	cubeSky->Render();

	Pass(0, 1, 2);

	//wall 메터리얼을 쉐이더에 밀어넣고
	//위의 wall로 렌더링
	wall->Render(); ;
	
	sphere->Render();


	brick->Render();
	cylinder->Render();

	stone->Render();
	cube->Render();

	floor->Render();
	grid->Render();

	airplane->Render();
	kachujin->Render();
	for (UINT i = 0; i < kachujin->GetTransformCount(); ++i)
	{
		colliders[i]->Collider->Render();
	}
	weapon->Render();
}

void LightingDemo::Airplane()
{
	airplane = new ModelRender(shader);
	airplane->ReadMesh(L"B787/Airplane");
	airplane->ReadMaterial(L"B787/Airplane");

	Transform* transform = airplane->AddTransform();
	transform->Position(2.0f, 9.91f, 2.0f);
	transform->Scale(0.004f, 0.004f, 0.004f);
	airplane->UpdateTransforms();

	models.push_back(airplane);
}

void LightingDemo::Kachujin()
{
	kachujin = new ModelAnimator(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Idle");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Walk");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Run");
	kachujin->ReadClip(L"Kachujin/Sword And Shield Slash");
	kachujin->ReadClip(L"Kachujin/Salsa Dancing");


	Transform* transform = NULL;

	transform = kachujin->AddTransform();
	transform->Position(0, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(0, 0, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(-15, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(1, 1, 1.0f);

	transform = kachujin->AddTransform();
	transform->Position(-30, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(2, 2, 0.75f);

	transform = kachujin->AddTransform();
	transform->Position(15, 0, -30);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayBlendMode(3, 0, 1, 2);
	kachujin->SetBlendAlpha(3, 1.5f);

	transform = kachujin->AddTransform();
	transform->Position(30, 0, -32.5f);
	transform->Scale(0.075f, 0.075f, 0.075f);
	kachujin->PlayTweenMode(4, 4, 0.75f);

	kachujin->UpdateTransforms();
	//kachujin->SetAttachTransform(40); //transform을 구할 본의 인덱스를 지정:40 ->손부분
	animators.push_back(kachujin);
}

void LightingDemo::Colliders()
{
	UINT count = kachujin->GetTransformCount();
	colliders = new ColliderObject*[count];
	//count 인스턴싱된 캐릭터 갯수 

	colliderInitTransforms = new Transform();
	colliderInitTransforms->Position(-2.9f, 1.45f, -50.0f);
	colliderInitTransforms->Scale(5,5,75);

	for (UINT i = 0; i < count; ++i)
	{
		colliders[i] = new ColliderObject();
		//colliders[i]->Init = new Transform();
		//colliders[i]->Init->Position(0, 0, 0);
		//colliders[i]->Init->Scale(10, 10, 10);

		colliders[i]->Transform = new Transform();
		//colliders[i]->Collider = new Collider(colliders[i]->Transform, colliders[i]->Init);
		colliders[i]->Collider = new Collider(colliders[i]->Transform, colliderInitTransforms);


	}
}

void LightingDemo::Weapon()
{
	weapon = new ModelRender(shader);
	weapon->ReadMesh(L"Weapon/Sword");
	weapon->ReadMaterial(L"Weapon/Sword");

	UINT count = kachujin->GetTransformCount();
	for (UINT i = 0; i < count; i++)
		weapon->AddTransform();

	weapon->UpdateTransforms();
	models.push_back(weapon);

	weaponInitTransform = new Transform();
	weaponInitTransform->Position(-2.9f, 1.45f, -6.45f);
	weaponInitTransform->Scale(0.5f, 0.5f, 0.75f);
	weaponInitTransform->Rotation(0, 0, 1);
}

void LightingDemo::Mesh()
{
	//Create Material//
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->Specular(1, 1, 1, 20);
	floor->SpecularMap("Floor_Specular.png");

	stone = new Material(shader);
	stone->DiffuseMap("Stones.png");
	stone->Specular(1, 1, 1, 20);
	stone->SpecularMap("Stones_Specular.png");
	stone->Emissive(1, 0, 0, 1);

	brick = new Material(shader);
	brick->DiffuseMap("Bricks.png");
	brick->Specular(1, 0.3f, 0.3f, 20);
	brick->SpecularMap("Bricks_Specular.png");
	brick->Emissive(1, 0, 0, 0.3f);

	wall = new Material(shader);
	wall->DiffuseMap("Wall.png");
	wall->Specular(1, 1, 1, 20);
	wall->SpecularMap("Wall_Specular.png");
	wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);

	Transform* transform = NULL;


	/*quad = new MeshQuad(shader);
	quad->DiffuseMap(L"Box.png");*/
	cube = new MeshRender(shader, new MeshCube());
	transform = cube->AddTransform();
	transform->Position(0, 5, 0);
	transform->Scale(20, 10, 20);

	grid = new MeshRender(shader, new MeshGrid(5, 5));
	transform = grid->AddTransform();
	transform->Position(0, 0, 0);
	transform->Scale(12, 1, 12);

	cylinder = new MeshRender(shader, new MeshCylinder(0.5f, 3.0f, 20, 20));
	sphere = new MeshRender(shader, new MeshSphere(0.5f, 20, 20));
	for (UINT i = 0; i < 5; i++)
	{
		transform = cylinder->AddTransform();
		transform->Position(-30, 6, -15.0f + (float)i * 15.0f);
		transform->Scale(5, 5, 5);

		transform = cylinder->AddTransform();
		transform->Position(30, 6, -15.0f + (float)i * 15.0f);
		transform->Scale(5, 5, 5);


		transform = sphere->AddTransform();
		transform->Position(-30, 15.5f, -15.0f + (float)i * 15.0f);
		transform->Scale(5, 5, 5);

		transform = sphere->AddTransform();
		transform->Position(30, 15.5f, -15.0f + (float)i * 15.0f);
		transform->Scale(5, 5, 5);
	}
	sphere->UpdateTransforms();
	cylinder->UpdateTransforms();
	cube->UpdateTransforms();
	grid->UpdateTransforms();
}

void LightingDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}