#include "stdafx.h"
#include "ProjectionDemo.h"
#include "../Framework/Viewer/MoveCamera.h"
#include "../Framework/Environment/CubeSky.h"

void ProjectionDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(23, 7, 0);
	Context::Get()->GetCamera()->Position(-27, 77, -188);

	cubeSky = new CubeSky(L"Environment/GrassCube1024.dds");

	shader = new Shader(L"54_Render.fx");


	Mesh();
	Airplane();
	Kachujin();
	Colliders();
}


void ProjectionDemo::Destroy()
{
	SafeDelete(shader);
	//SafeDelete(quad);
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);
	SafeDelete(cubeSky);
}

void ProjectionDemo::Update()
{
	Vector3 position;
	Vector3 positionUpdate;

	
	airplane->GetTransform(0)->Position(&positionUpdate);
	positionUpdate.y += Time::Delta();
	airplane->GetTransform(0)->Position(positionUpdate);
	airplane->UpdateTransforms();

	airplane->GetTransform(0)->Position(&position);

	Vector3 position2D;

	Matrix W,V,P;
	D3DXMatrixIdentity(&W);
	V = Context::Get()->View();
	P = Context::Get()->Projection();
	Context::Get()->GetViewport()->Project(&position2D, position, W,V,P);
	
	Gui::Get()->RenderText(position2D.x, position2D.y, 1, 0, 0, "airplane");

	ImGui::SliderFloat3("Light Direction", Context::Get()->Direction(), -1, +1);
	cubeSky->Update();

	cube->Update();
	grid->Update();
	sphere->Update();
	cylinder->Update();

	airplane->Update();
	kachujin->Update();

	for (UINT i = 0; i < kachujin->GetTransformCount(); ++i)
	{
		Matrix attach;
		kachujin->GetAttachTransform(i, &attach);

		position = Vector3(0, 0, 0);

		Context::Get()->GetViewport()->Project(&position2D, position, attach, V, P);
		Gui::Get()->RenderText(position2D.x, position2D.y, 1, 1, 1, "hand");

		colliders[i]->Collider->GetTransform()->World(attach);
		colliders[i]->Collider->Update();
	}
}

void ProjectionDemo::Render()
{
	static float width = D3D::Width();
	static float height = D3D::Height();

	static float x = 0.0f;
	static float y = 0.0f;

	ImGui::InputFloat("Width", &width, 1.0f);
	ImGui::InputFloat("Height", &height, 1.0f);
	ImGui::InputFloat("X", &x, 1.0f);
	ImGui::InputFloat("Y", &y, 1.0f);

	Context::Get()->GetViewport()->Set(width, height, x, y);

	static float fov = 0.25f;
	ImGui::InputFloat("Fov", &fov, 0.001f);

	static float minZ = 0.1f, maxZ = 1000.0f;
	ImGui::InputFloat("MinZ", &minZ, 1.0f);
	ImGui::InputFloat("MaxZ", &maxZ, 1.0f);

	Perspective* perspective = Context::Get()->GetPerspective();
	perspective->Set(D3D::Width(), D3D::Height(), minZ, maxZ, fov);

	cubeSky->Render();

	Pass(0, 1, 2);

	wall->Render(); ; //wall 메터리얼을 쉐이더에 밀어넣고
	sphere->Render();//위의 wall로 렌더링


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
}

void ProjectionDemo::Airplane()
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

void ProjectionDemo::Kachujin()
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

void ProjectionDemo::Colliders()
{
	UINT count = kachujin->GetTransformCount();
	colliders = new ColliderObject*[count];
	//count 인스턴싱된 캐릭터 갯수 

	for (UINT i = 0; i < count; ++i)
	{
		colliders[i] = new ColliderObject();
		colliders[i]->Init = new Transform();
		colliders[i]->Init->Position(0, 0, 0);
		colliders[i]->Init->Scale(10, 10, 10);

		colliders[i]->Transform = new Transform();
		colliders[i]->Collider = new Collider(colliders[i]->Transform, colliders[i]->Init);


	}
}

void ProjectionDemo::Mesh()
{
	//Create Material//
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");

	stone = new Material(shader);
	stone->DiffuseMap("Stones.png");

	brick = new Material(shader);
	brick->DiffuseMap("Bricks.png");

	wall = new Material(shader);
	wall->DiffuseMap("Wall.png");


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

void ProjectionDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}