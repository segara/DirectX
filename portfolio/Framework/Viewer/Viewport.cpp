#include "Framework.h"
#include "Viewport.h"

Viewport::Viewport(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	Set(width, height, x, y, minDepth, maxDepth);
}

Viewport::~Viewport()
{
	
}

void Viewport::RSSetViewport()
{
	D3D::GetDC()->RSSetViewports(1, &viewport);
}

void Viewport::Set(float width, float height, float x, float y, float minDepth, float maxDepth)
{
	viewport.TopLeftX = this->x = x;
	viewport.TopLeftY = this->y = y;
	viewport.Width = this->width = width;
	viewport.Height = this->height = height;
	viewport.MinDepth = this->minDepth = minDepth;
	viewport.MaxDepth = this->maxDepth = maxDepth;

	RSSetViewport();
}

void Viewport::Project(Vector3 * pOut, Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;  
	Matrix wvp = W * V * P;

	D3DXVec3TransformCoord(pOut, &position, &wvp); 
	//위에서 P 까지 변환된 결과 x,y :-1~1 z:0~1 
	//가로 0~1280 / 세로 0 ~ 640 사이로 변환
	pOut->x = ((pOut->x + 1.0f)*0.5f) * width + x;
	pOut->y = ((-pOut->y + 1.0f)*0.5f) * height + y;

	pOut->z = (pOut->z * (maxDepth - minDepth)) + minDepth;
}

void Viewport::Unproject(Vector3 * pOut, Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;

	//화면좌표->정사각형 공간으로 변환
	//x 0~1280 / y 0 ~ 640 (화면의 시작점)
	//0~1사이값 -> *2 하여서 0~2사이 -> -1 하여서 -1~1사이값 조정
	pOut->x = ((position.x - x) / width) * 2.0f -1.0f;
	pOut->y = (((position.y - y) / height) * 2.0f-1.0f) * -1.0f;
	pOut->z = ((position.z - minDepth) / (maxDepth - minDepth));

	//p의 역행렬 , v의 역행렬, w의 역행렬을 순서대로 곱해주어야 하지만
	//(A*B*C)의 역행렬은 C역행렬*B역행렬*A역행렬 이다
	Matrix wvp = W * V * P;

	D3DXMatrixInverse(&wvp, NULL, &wvp);

	D3DXVec3TransformCoord(pOut, pOut, &wvp);
}
