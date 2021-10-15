#pragma once
#include "Systems/IExecute.h"

class InstancingDemo : public IExecute
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
	void CreateMesh();
private:
	Shader* shader;
	Material* material;

	vector<Mesh::MeshVertex> vertices;
	vector<UINT> indices;
	//Vertex vertices[2];
	//ID3D11Buffer* vertexBuffer;
	VertexBuffer* vertexBuffer;
	VertexBuffer* instanceBuffer;
	IndexBuffer* indexBuffer;

	PerFrame* perFrame;

	Transform* transforms[1000];
	Matrix worlds[1000];
	
};
