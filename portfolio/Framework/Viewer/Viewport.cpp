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
	//������ P ���� ��ȯ�� ��� x,y :-1~1 z:0~1 
	//���� 0~1280 / ���� 0 ~ 640 ���̷� ��ȯ
	pOut->x = ((pOut->x + 1.0f)*0.5f) * width + x;
	pOut->y = ((-pOut->y + 1.0f)*0.5f) * height + y;

	pOut->z = (pOut->z * (maxDepth - minDepth)) + minDepth;
}

void Viewport::Unproject(Vector3 * pOut, Vector3 & source, Matrix & W, Matrix & V, Matrix & P)
{
	Vector3 position = source;

	//ȭ����ǥ->���簢�� �������� ��ȯ
	//x 0~1280 / y 0 ~ 640 (ȭ���� ������)
	//0~1���̰� -> *2 �Ͽ��� 0~2���� -> -1 �Ͽ��� -1~1���̰� ����
	pOut->x = ((position.x - x) / width) * 2.0f -1.0f;
	pOut->y = (((position.y - y) / height) * 2.0f-1.0f) * -1.0f;
	pOut->z = ((position.z - minDepth) / (maxDepth - minDepth));

	//p�� ����� , v�� �����, w�� ������� ������� �����־�� ������
	//(A*B*C)�� ������� C�����*B�����*A����� �̴�
	Matrix wvp = W * V * P;

	D3DXMatrixInverse(&wvp, NULL, &wvp);

	D3DXVec3TransformCoord(pOut, pOut, &wvp);
}
