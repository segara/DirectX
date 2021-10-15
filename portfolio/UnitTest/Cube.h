#pragma once
#include "Systems/IExecute.h"
#include "Framework.h"
#include "Renders/Context.h"

class Cube : public IExecute
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
	void CreateVertex();
	void CreateIndice();
	void CreateVertexEnemy(ID3D11Buffer** vertexBuffer);
	bool CheckRectCollision(Matrix player, Matrix enemy);
	bool IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
private:
	Shader* shader;

	UINT width = 10, height = 10;
	UINT vertexCount;
	UINT indexCount;

	Vertex* vertices;
	UINT* indices;
	 
	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* indexBuffer;
	
	Matrix world_Transform;

	Vector3 position = Vector3(0,0,0);
	Vector3 rotation = Vector3(0,0,0);
	Vector3 scale = Vector3(1,1,1);

	Color color = Color(0, 0, 1, 1);
};
