#include "stdafx.h"
#include "ColorToneDemo.h"
#include "../Framework/Viewer/MoveCamera.h"
#include "../Framework/Environment/CubeSky.h"

void ColorToneDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	cubeSky = new CubeSky(L"Environment/GrassCube1024.dds");

	//Performance performance;
	//performance.Start();

	shader = new Shader(L"96_Billboard.fx");

	float width = D3D::Width(), height = D3D::Height();
	renderTarget = new RenderTarget((UINT)width, (UINT)height);
	depthStencil = new DepthStencil((UINT)width, (UINT)height);
	viewport = new Viewport(width, height);

	render2D = new Render2D();
	render2D->GetTransform()->Scale(355.0f, 200, 1);
	render2D->GetTransform()->Position(200, 120, 0);
	render2D->SRV(renderTarget->SRV());
	//float t = performance.End();

	postEffect = new PostEffect(L"100_ColorTone.fx");
	postEffect->SRV(renderTarget->SRV());
	//MessageBox(D3D::GetDesc().Handle, to_wstring(t).c_str(), L"", MB_OK);

	Mesh();
	Airplane();
	Kachujin();
	Colliders();
	Weapon();
	CreatePointLight();
	CreateSpotLight();
	CreateBillboard();
}


void ColorToneDemo::Destroy()
{
	SafeDelete(shader);
	//SafeDelete(quad);
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);
	SafeDelete(cubeSky);
}

void ColorToneDemo::Update()
{
	static UINT selected = postEffect->GetShader()->PassCount() - 1;
	ImGui::InputInt("Pass Selected", (int *)&selected);
	selected %= postEffect->GetShader()->PassCount();
	postEffect->Pass(selected);

	Vector2 PixelSize = Vector2(1.0f / D3D::Width(), 1.0f / D3D::Height());
	postEffect->GetShader()->AsVector("PixelSize")->SetFloatVector(PixelSize);

	static float Sharpness = 0.0f;
	ImGui::InputFloat("Sharpness", &Sharpness, 0.1f);
	postEffect->GetShader()->AsScalar("Sharpness")->SetFloat(Sharpness);


	static float Saturation = 0.0f;
	ImGui::InputFloat("Saturation", &Saturation, 0.1f);
	postEffect->GetShader()->AsScalar("Saturation")->SetFloat(Sharpness);


	//Vignette
	{
		ImGui::Separator();

		static float Power = 1.0f;
		ImGui::InputFloat("Power", &Power, 0.1f);
		postEffect->GetShader()->AsScalar("Power")->SetFloat(Power);

		static float ScaleX = 1.0f;
		ImGui::InputFloat("ScaleX", &ScaleX, 0.1f);

		static float ScaleY = 1.0f;
		ImGui::InputFloat("ScaleY", &ScaleY, 0.1f);
		postEffect->GetShader()->AsVector("Scale")->SetFloatVector(Vector2(ScaleX, ScaleY));
	}

	//3D red, green color divide
	//LensDistortion
	{
		ImGui::Separator();

		static float LensPower = 1.0f;
		ImGui::InputFloat("LensPower", &LensPower, 0.01f);
		postEffect->GetShader()->AsScalar("LensPower")->SetFloat(LensPower);

		static Vector3 Distortion = Vector3(-0.02f, -0.02f, -0.02f);
		ImGui::InputFloat("DistortionX", &Distortion.x, 0.001f);
		ImGui::InputFloat("DistortionY", &Distortion.y, 0.001f);
		ImGui::InputFloat("DistortionZ", &Distortion.z, 0.001f);
		postEffect->GetShader()->AsVector("Distortion")->SetFloatVector(Distortion);
	}
	//Interace
	{
		ImGui::Separator();

		static float Strength = 1.0f;
		ImGui::InputFloat("Strength", &Strength, 0.01f);
		postEffect->GetShader()->AsScalar("Strength")->SetFloat(Strength);

		static int InteraceValue = 2;
		ImGui::InputInt("InteraceValue", &InteraceValue);
		postEffect->GetShader()->AsScalar("interaceValue")->SetInt(InteraceValue);
	}

	//Down Scale
	{
		ImGui::Separator();

		static float ScaleX = D3D::Width();
		ImGui::InputFloat("DownScaleX", &ScaleX, 1.0f);

		static float ScaleY = D3D::Height();
		ImGui::InputFloat("DownScaleY", &ScaleY, 1.0f);
		postEffect->GetShader()->AsVector("ScaleSourceSize")->SetFloatVector(Vector2(ScaleX, ScaleY));
	}

	//Wiggle
	{
		ImGui::Separator();

		static float OffsetX = 10;
		ImGui::InputFloat("OffsetX", &OffsetX, 0.1f);

		static float OffsetY = 10;
		ImGui::InputFloat("OffsetY", &OffsetY, 0.1f);
		postEffect->GetShader()->AsVector("WiggleOffset")->SetFloatVector(Vector2(OffsetX, OffsetY));

		static float AmountX = 0.01f;
		ImGui::InputFloat("AmountX", &AmountX, 0.001f);

		static float AmountY = 0.01f;
		ImGui::InputFloat("AmountY", &AmountY, 0.001f);
		postEffect->GetShader()->AsVector("WiggleAmount")->SetFloatVector(Vector2(AmountX, AmountY));
	}

	//static UINT selected = 0;
	//ImGui::InputInt("NormalMap Selected", (int *)&selected);
	//selected %= 4;

	//shader->AsScalar("Selected")->SetInt(selected);

	////SpotLight
	//{
	//	static UINT spotIndex = 0;
	//	ImGui::InputInt("SpotLight Index", (int *)&spotIndex);
	//	spotIndex %= Lighting::Get()->SpotLightCount();


	//	SpotLight& spotLight = Lighting::Get()->GetSpotLight(spotIndex);

	//	ImGui::ColorEdit3("SpotLight Ambient", spotLight.Ambient);
	//	ImGui::ColorEdit3("SpotLight Diffuse", spotLight.Diffuse);
	//	ImGui::ColorEdit3("SpotLight Specular", spotLight.Specular);
	//	ImGui::ColorEdit3("SpotLight Emissive", spotLight.Emissive);

	//	ImGui::SliderFloat3("SpotLight Position", spotLight.Position, -50, 50);
	//	ImGui::SliderFloat("SpotLight Range", &spotLight.Range, 0, 30);

	//	ImGui::SliderFloat3("SpotLight Direciton", spotLight.Direction, -1, 1);
	//	ImGui::SliderFloat("SpotLight Angle", &spotLight.Angle, 1, 90);

	//	ImGui::SliderFloat("SpotLight Intensity", &spotLight.Intensity, 0, 1);
	//}

	//PointLight& pointLight = Lighting::Get()->GetPointLight(0);
	//ImGui::Begin("Point Lighting");
	//{
	//	ImGui::ColorEdit3("Ambient", pointLight.Ambient);
	//	ImGui::ColorEdit3("Diffuse", pointLight.Diffuse);
	//	ImGui::ColorEdit3("Specular", pointLight.Specular);
	//	ImGui::ColorEdit3("Emissive", pointLight.Emissive);

	//	ImGui::SliderFloat3("Position", pointLight.Position, -50, 50);
	//	ImGui::SliderFloat("Range", &pointLight.Range, 0, 20);
	//	ImGui::SliderFloat("Intensity", &pointLight.intensity, 0, 1);
	//}
	//ImGui::End();

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
		kachujin->GetAttachTransform(i, world); //?????? ???? ???? ????????.

		colliders[i]->Collider->GetTransform()->World(world[40]);
		colliders[i]->Collider->Update();

		weapon->GetTransform(i)->World(weaponInitTransform->World() * world[40]);
	}
	weapon->UpdateTransforms();
	weapon->Update();
	billboard->Update();
	render2D->Update(); 
	postEffect->Update();
}

void ColorToneDemo::PreRender()
{
	renderTarget->PreRender(depthStencil); //backbuffer?? ??????
	////////////////////////////////////////////////////////////
	//???????? ???? :D3D::GetDC()->RSSetViewports(1, &viewport);
	//???????? ???????? ??????
	//???????? ?????? ???????? ???????? ???????? ?????? ??)200,200
	//?????????? 200,200 ?? (??????)1280/920?? ?????? ??????????????.
	//?????????? ?????? ?????? ???????? ?????? ???? ????????.
	viewport->RSSetViewport();
	{
		cubeSky->Render();

		Pass(0, 1, 2);

		//wall ?????????? ???????? ????????
		//???? wall?? ??????
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
		billboard->Render();
	}
}

void ColorToneDemo::Render()
{
	//if (Keyboard::Get()->Down(VK_SPACE))
	//	renderTarget->SaveTexture(L"../RenderTarget.png");

}

void ColorToneDemo::PostRender()
{
	
	//?????????? ????????

	//postEffect->SRV(renderTarget->SRV());
	postEffect->Render();

	//render2D->SRV(renderTarget->SRV());
	render2D->Render();
}

void ColorToneDemo::Airplane()
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

void ColorToneDemo::Kachujin()
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
	//kachujin->SetAttachTransform(40); //transform?? ???? ???? ???????? ????:40 ->??????
	animators.push_back(kachujin);
}

void ColorToneDemo::Colliders()
{
	UINT count = kachujin->GetTransformCount();
	colliders = new ColliderObject*[count];
	//count ?????????? ?????? ???? 

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

void ColorToneDemo::Weapon()
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

void ColorToneDemo::CreatePointLight()
{
	PointLight light;
	light =
	{
		Color(0,0,0,1),//ambient
		Color(0,0,1,1),//diffuse
		Color(0,0,0.7f,1),//specular
		Color(0,0,0.7f,1),//emissive
		Vector3(-30,10,-30),
		5,
		0.9f
	};
	Lighting::Get()->AddPointLight(light);
}
void ColorToneDemo::CreateSpotLight()
{
	SpotLight light;
	light =
	{
		Color(0.3f,1,0,1),//ambient
		Color(0.7f,1,0,1),//diffuse
		Color(0.3f,1,0,1),//specular
		Color(0.3f,1,0,1),//emissive
		Vector3(-30,10,-30),
		25.0f,
		Vector3(0,-1,0),//direction
		30.0f,//Angle
		0.9f//Intensity
	};
	Lighting::Get()->AddSpotLight(light);
}
void ColorToneDemo::CreateBillboard()
{
	billboard = new Billboard(shader);
	billboard->Pass(4);
	billboard->AddTexture(L"Terrain/grass_14.tga");
	billboard->AddTexture(L"Terrain/grass_07.tga");
	billboard->AddTexture(L"Terrain/grass_11.tga");
	for (UINT i = 0; i < 1200; i++)
	{
		Vector2 scale = Math::RandomVec2(1, 3);
		Vector2 position = Math::RandomVec2(-60, 60);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale,0);
	}

	for (UINT i = 0; i < 300; i++)
	{
		Vector2 scale = Math::RandomVec2(1, 3);
		Vector2 position = Math::RandomVec2(-60, 60);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale,1);
	}

	for (UINT i = 0; i < 700; i++)
	{
		Vector2 scale = Math::RandomVec2(1, 3);
		Vector2 position = Math::RandomVec2(-60, 60);

		billboard->Add(Vector3(position.x, scale.y * 0.5f, position.y), scale,2);
	}
}
void ColorToneDemo::Mesh()
{
	//Create Material//
	floor = new Material(shader);
	floor->DiffuseMap("Floor.png");
	floor->Specular(1, 1, 1, 20);
	floor->SpecularMap("Floor_Specular.png");
	floor->NormalMap("Floor_Normal.png");

	stone = new Material(shader);
	stone->DiffuseMap("Stones.png");
	stone->Specular(1, 1, 1, 20);
	stone->SpecularMap("Stones_Specular.png");
	stone->Emissive(1, 0, 0, 1);
	stone->NormalMap("Stones_Normal.png");

	brick = new Material(shader);
	brick->DiffuseMap("Bricks.png");
	brick->Specular(1, 0.3f, 0.3f, 20);
	brick->SpecularMap("Bricks_Specular.png");
	brick->Emissive(1, 0, 0, 0.3f);
	brick->NormalMap("Bricks_Normal.png");

	wall = new Material(shader);
	wall->DiffuseMap("Wall.png");
	wall->Specular(1, 1, 1, 20);
	wall->SpecularMap("Wall_Specular.png");
	wall->Emissive(0.15f, 0.15f, 0.15f, 0.3f);
	wall->NormalMap("Wall_Normal.png");

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

void ColorToneDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}