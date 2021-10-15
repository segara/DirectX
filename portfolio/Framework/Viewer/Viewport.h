#pragma once

class Viewport
{
public:
	Viewport(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);
	~Viewport();

	void RSSetViewport();
	void Set(float width, float height, float x = 0, float y = 0, float minDepth = 0, float maxDepth = 1);

	float GetWidth() { return width; }
	float GetHeight() { return height; }

	//W:world V:view P:projection
	void Project(Vector3* pOut, Vector3& source, Matrix& W, Matrix& V, Matrix& P);
	
	///Unprojection은 Projection의 정확히 반대로 계산
	///Projection:3D->W,V,P->Vp->2D
	///Unprojection:2D->Vp->P,V,W->3D
	void Unproject(Vector3* pOut, Vector3& source, Matrix& W, Matrix& V, Matrix& P);
	
private:
	float x, y;
	float width, height;
	float minDepth, maxDepth;

	D3D11_VIEWPORT viewport;
};