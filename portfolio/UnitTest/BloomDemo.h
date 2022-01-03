#pragma once
#include "Systems/IExecute.h"

//class CubeMap;
class CubeSky;
class  BloomDemo : public IExecute
{
public:
	// IExecute��(��) ���� ��ӵ�
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
	void SetBlur();
	void GetBlurParameter(vector<float>& weights, vector<Vector2>& offsets, float x, float y);
	float GetGaussFunction(float val);
private:
	bool bOrigin = false;
	float threshold = 0.05f;
	UINT blurCount = 15;

	vector<float> weightX;
	vector<Vector2> offsetX;

	vector<float> weightY;
	vector<Vector2> offsetY;
private:
	Shader* shader;
	
	//render target1 : luminosity
	//render target2 : x/y blur
	//render target3 : 1+2 ��ħ

	RenderTarget* renderTarget[5];
	DepthStencil* depthStencil;
	//post effect�ÿ� viewport�� ������ ���� rtv, dsv, viewport 1:1:1�� ����
	Viewport* viewport;
	Render2D* render2D; //2d box ������
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
	//ĳ���Ͱ�����ŭ collider�����ϱ� ������ **������(�������� �迭)
	//ColliderObject* colliders[count]; �� ������ �ʱ� �������� �𸣱� ������
	//���������ͻ��

	Transform* colliderInitTransforms;

	ModelRender* weapon = NULL;
	Transform* weaponInitTransform;

	vector<MeshRender *> meshes;
	vector<ModelRender *> models;
	vector<ModelAnimator *> animators;

};
