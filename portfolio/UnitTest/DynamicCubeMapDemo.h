#pragma once
#include "Systems/IExecute.h"

//class CubeMap;
class CubeSky;
class  DynamicCubeMapDemo : public IExecute
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
	Shader* shader;
	
	DynamicCubeMap* cubeMap;

	Billboard* billboard;

	CubeSky* cubeSky;

	Material* floor;
	Material* stone;
	Material* brick;
	Material* wall;

	MeshRender* cube;
	MeshRender* sphere;
	MeshRender* sphere_dynamicCubeMap;
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