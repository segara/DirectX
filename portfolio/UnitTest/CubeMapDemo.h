#pragma once
#include "Systems/IExecute.h"

class CubeMap;
class CubeMapDemo : public IExecute
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
	Shader* shader;
	Shader* shaderCubeMap;
	MeshQuad* quad;
	MeshCube* cube;
	MeshGrid* grid;
	CubeMap* cubeMap;

	MeshSphere* sphere;
	MeshCylinder* cylinder;
	Vector3 direction = Vector3(-1, -1, 1);
	ID3DX11EffectVectorVariable* sDirection;

};
