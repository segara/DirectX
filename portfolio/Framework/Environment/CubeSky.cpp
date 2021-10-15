#include "Framework.h"
#include "CubeSky.h"
//#include "../Framework/Renders/Context.h"
#include "../Framework/Viewer/MoveCamera.h"
CubeSky::CubeSky(wstring file)
{
	shader = new Shader(L"30_CubeSky.fx");
	sphere = new MeshRender(shader, new MeshSphere(0.5f));
	sphere->AddTransform();
	file = L"../../_Textures/" + file;
	Check(D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), file.c_str(), NULL, NULL, &srv, NULL));
	sSrv = shader->AsSRV("SkyCubeMap");
}


CubeSky::~CubeSky()
{
	SafeDelete(shader);
	SafeDelete(sphere);
	SafeRelease(srv);
}

void CubeSky::Update()
{
	Vector3 currentCameraPos; 
	Context::Get()->GetCamera()->Position(&currentCameraPos);
	sphere->GetTransform(0)->Position(currentCameraPos);
	sphere->UpdateTransforms();
}

void CubeSky::Render()
{
	sSrv->SetResource(srv);
	sphere->Render();
}
