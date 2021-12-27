#include "stdafx.h"
#include "BlurDemo.h"
#include "../Framework/Viewer/MoveCamera.h"
#include "../Framework/Environment/CubeSky.h"

void BlurDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	cubeSky = new CubeSky(L"Environment/GrassCube1024.dds");

	//Performance performance;
	//performance.Start();

	shader = new Shader(L"96_Billboard.fx");

	float width = D3D::Width(), height = D3D::Height();
	renderTarget[0] = new RenderTarget((UINT)width, (UINT)height);
	renderTarget[1] = new RenderTarget((UINT)width, (UINT)height);
	renderTarget[2] = new RenderTarget((UINT)width, (UINT)height);
	depthStencil = new DepthStencil((UINT)width, (UINT)height);
	viewport = new Viewport(width, height);

	render2D = new Render2D();
	render2D->GetTransform()->Scale(355.0f, 200, 1);
	render2D->GetTransform()->Position(200, 120, 0);
	render2D->SRV(renderTarget[0]->SRV());
	//float t = performance.End();

	postEffect = new PostEffect(L"104_Blur.fx");
	//postEffect->SRV(renderTarget->SRV());
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


void BlurDemo::Destroy()
{
	SafeDelete(shader);
	//SafeDelete(quad);
	SafeDelete(cube);
	SafeDelete(cylinder);
	SafeDelete(sphere);
	SafeDelete(cubeSky);
}

void BlurDemo::Update()
{
	static UINT selected = postEffect->GetShader()->PassCount() - 1;
	ImGui::InputInt("Pass Selected", (int *)&selected);
	selected %= postEffect->GetShader()->PassCount();
	postEffect->Pass(selected);
	
	//Blur
	{
		static UINT blurCount = 1;
		ImGui::InputInt("BlurCount", (int *)&blurCount);
		postEffect->GetShader()->AsScalar("BlurCount")->SetInt(blurCount);

	}
	//RadialBlur
	{
		ImGui::Separator();

		static UINT BlurCount = 8;
		ImGui::InputInt("RadialBlurCount", (int *)&BlurCount);
		BlurCount %= 32;
		postEffect->GetShader()->AsScalar("RadialBlurCount")->SetInt(BlurCount);

		static float Radius = 0.6f;
		ImGui::InputFloat("RadialBlurRadius", &Radius, 0.01f);
		postEffect->GetShader()->AsScalar("RadialBlurRadius")->SetFloat(Radius);

		static float Amount = 0.04f;
		ImGui::InputFloat("RadialBlurAmount", (float *)&Amount, 0.001f);
		postEffect->GetShader()->AsScalar("RadialBlurAmount")->SetFloat(Amount);

		static float CenterX = 0.5f;
		ImGui::InputFloat("CenterX", (float *)&CenterX, 0.01f);

		static float CenterY = 0.5f;
		ImGui::InputFloat("CenterX", (float *)&CenterY, 0.01f);
		   
		postEffect->GetShader()->AsVector("RadialCenter")->SetFloatVector(Vector2(CenterX,CenterY));

	}

	//UV 기준으로 pixel사이즈 계산 
	Vector2 PixelSize = Vector2(1.0f / D3D::Width(), 1.0f / D3D::Height());
	postEffect->GetShader()->AsVector("PixelSize")->SetFloatVector(PixelSize);

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
		kachujin->GetAttachTransform(i, world); //모델의 전체 본을 얻어온다.

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

void BlurDemo::PreRender()
{
	renderTarget[0]->PreRender(depthStencil); //backbuffer에 렌더링
	////////////////////////////////////////////////////////////
	//뷰표트를 셋팅 :D3D::GetDC()->RSSetViewports(1, &viewport);
	//뷰포트를 셋팅하지 않으면
	//렌더타겟 텍스쳐 사이즈를 줄인채로 렌더링을 했을시 예)200,200
	//렌더텍스쳐 200,200 에 (뷰포트)1280/920의 화면을 렌더링할것이다.
	//뷰포트 0,0부터 렌더링되기 때문에 화면 윗부분의 일부만 출력됨
	//뷰포트까지 맞춰서 줄이고 적용하면 화면에 맞게 출력된다.
	viewport->RSSetViewport();
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
		billboard->Render();
	}
	//GuassianBlurX
	{
		renderTarget[1]->PreRender(depthStencil); 
		//rendertarget1을 렌더타겟(출력용)으로 설정 (x blur 용)
		//backbuffer에 렌더링

		postEffect->Pass(3); 
		postEffect->SRV(renderTarget[0]->SRV());
		postEffect->Render();
	}

	//GuassianBlurY
	{
		renderTarget[2]->PreRender(depthStencil);
		//rendertarget2을 렌더타겟(출력용)으로 설정 (y blur 용)

		postEffect->Pass(4);
		postEffect->SRV(renderTarget[1]->SRV());
		postEffect->Render();
	}
	
}

void BlurDemo::Render()
{
	//if (Keyboard::Get()->Down(VK_SPACE))
	//	renderTarget->SaveTexture(L"../RenderTarget.png");

}

void BlurDemo::PostRender()
{
	//postEffect->Pass(0);
	//순차적으로 렌더링됨
	postEffect->SRV(renderTarget[2]->SRV());

	//postEffect->SRV(renderTarget->SRV());
	postEffect->Render();

	//render2D->SRV(renderTarget->SRV());
	render2D->Render();
}

void BlurDemo::Airplane()
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

void BlurDemo::Kachujin()
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

void BlurDemo::Colliders()
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

void BlurDemo::Weapon()
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

void BlurDemo::CreatePointLight()
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
void BlurDemo::CreateSpotLight()
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
void BlurDemo::CreateBillboard()
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
void BlurDemo::Mesh()
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

void BlurDemo::Pass(UINT mesh, UINT model, UINT anim)
{
	for (MeshRender* temp : meshes)
		temp->Pass(mesh);

	for (ModelRender* temp : models)
		temp->Pass(model);

	for (ModelAnimator* temp : animators)
		temp->Pass(anim);
}