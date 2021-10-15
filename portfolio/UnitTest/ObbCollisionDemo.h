#pragma once
#include "Systems/IExecute.h"

//class CubeMap;
class CubeSky;
class ObbCollisionDemo : public IExecute
{
public:
	// IExecute을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}

private:
	//void Pass(UINT mesh, UINT model, UINT anim);

private:
	Shader* shader;

	CubeSky* cubeSky;

	Material* floor;
	
	MeshRender* grid;

	Transform* transform[2];
	Collider* collider[2];

};
