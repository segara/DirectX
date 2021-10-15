#include "stdafx.h"
#include "ModelDemo.h"
#include "Converter.h"
#include "../Framework/Viewer/Camera.h"
void ModelDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(20, 0, 0);
	Context::Get()->GetCamera()->Position(1, 36, -85);
	shader = new Shader(L"52_InstancingModel.fx");
	Tank();
	Kachujin();


	
}

void ModelDemo::Tank()
{
	tank = new ModelRender(shader);
	tank->ReadMesh(L"Tank/Tank");
	tank->ReadMaterial(L"Tank/Tank");

	for (float x = -50; x <= 50; x += 2.5f)
	{
		Transform* transform = tank->AddTransform();

		transform->Position(Vector3(x, 0.0f, 5.0f));
		transform->RotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
		transform->Scale(0.1f, 0.1f, 0.1f);
	}
	/*Transform* transform = tank->AddTransform();

	transform->Position(Vector3(0.0f, 0.0f, 5.0f));
	transform->RotationDegree(0, Math::Random(-180.0f, 180.0f), 0);
	transform->Scale(0.1f, 0.1f, 0.1f);*/
	tank->UpdateTransforms();
	tank->Pass(1);
}

void ModelDemo::Kachujin()
{
	kachujin = new ModelRender(shader);
	kachujin->ReadMesh(L"Kachujin/Mesh");
	kachujin->ReadMaterial(L"Kachujin/Mesh");
	kachujin->AddTransform();
	kachujin->GetTransform(0)->Position(0, 0, 0);
	kachujin->GetTransform(0)->Scale(0.025f, 0.025f, 0.025f);
	kachujin->Pass(1);
}

void ModelDemo::Destroy()
{

}

void ModelDemo::Update()
{
	static int pass;
	ImGui::InputInt("Pass", &pass);
	pass %= 2;


	if (tank != NULL) {
		//tank->Pass(pass);
		//ModelBone* bone = tank->GetModel()->BoneByIndex(10); //10Àº Æ÷Å¾

		//Transform transform;
		//float rotation = sinf(10 * Time::Delta()) ;
		//transform.Rotation(0, rotation, 0);
		//tank->UpdateTransform(bone, transform.World());
		tank->Update();
	}
	if (kachujin != NULL) {
		//kachujin->Pass(pass);
		kachujin->Update();
	}

}

void ModelDemo::Render()
{

	if (tank != NULL)tank->Render();
	if (kachujin != NULL)kachujin->Render();
}


