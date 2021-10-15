#pragma once
#include "Systems/IExecute.h"
class Terrain;
class HeightMapDemo : public IExecute
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
	//void LoadTexture(wstring file);

	void CreateVertex();
	void CreateVertexEnemy(ID3D11Buffer** vertexBuffer);
	bool CheckRectCollision(Matrix player, Matrix enemy);
	bool IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2);
private:
	Shader* shader;
	Terrain* terrain;

};
