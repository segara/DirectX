#pragma once
#include "Systems/IExecute.h"

class RotationDemo : public IExecute
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
	void CreateVertexEnemy(ID3D11Buffer** vertexBuffer);
	bool CheckRectCollision(Matrix player, Matrix enemy);
	bool IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
private:
	Shader* shader;

	Vertex vertices[6];
	Vertex vertices2[6];
	Vertex vertices3[6];
	Vertex vertices4[6];
	Vertex vertices5[6];

	ID3D11Buffer* vertexBuffer;
	ID3D11Buffer* vertexBufferEnemy[4];

	UINT index = 0;
	Matrix world_Transform;
	

	Matrix world_enemy[4];

	Vector3 position = Vector3(0,0,0);
	Vector3 scale = Vector3(1,1,1);

	float z_rotation = 0.0f;
};
