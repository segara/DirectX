#pragma once
#include "Systems/IExecute.h"

//class CubeMap;
class CubeSky;
class ColorToneDemo : public IExecute
{
public:
	// IExecute을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override;
	virtual void Render() override;
	virtual void PostRender() override;
	virtual void ResizeScreen() override {}
private:
	void CreateBillboard();
	void Mesh();
	void Airplane();
	void Kachujin();
	void Colliders();
	void Weapon();
	void CreatePointLight();
	void CreateSpotLight();

private:
	void Pass(UINT mesh, UINT model, UINT anim);

private:
	Shader* shader;

	RenderTarget* renderTarget;
	DepthStencil* depthStencil;
	//post effect시에 viewport를 별도로 만들어서 rtv, dsv, viewport 1:1:1로 대응
	Viewport* viewport;
	Render2D* render2D; //2d box 렌더러
	PostEffect* postEffect;

	Billboard* billboard;

	CubeSky* cubeSky;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* sphere;
	MeshRender* cylinder;
	MeshRender* grid;

	ModelRender* airplane = NULL;
	ModelAnimator* kachujin = NULL;

	ColliderObject** colliders;
	//캐릭터갯수만큼 collider생성하기 때문에 **포인터(포인터의 배열)
	//ColliderObject* colliders[count]; 와 같지만 초기 갯수값을 모르기 때문에
	//더블포인터사용

	Transform* colliderInitTransforms;

	ModelRender* weapon = NULL;
	Transform* weaponInitTransform;

	vector<MeshRender *> meshes;
	vector<ModelRender *> models;
	vector<ModelAnimator *> animators;

};
