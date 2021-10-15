#include "stdafx.h"
#include "Rect.h"

void Rect::Initialize()
{
	shader = new Shader(L"06_Rectangle.fx");
	
	{
		vertices[0].Position = Vector3(0.0f, +0.0f, 0.0f);
		vertices[1].Position = Vector3(+0.0f, 0.5f, 0.0f);
		vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

		vertices[3].Position = Vector3(0.5f, 0.0f, 0.0f);
		vertices[4].Position = Vector3(0.0f, 0.5f, 0.0f);
		vertices[5].Position = Vector3(0.5f, 0.5f, 0.0f);

		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	{
		vertices2[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
		vertices2[1].Position = Vector3(-0.5f, 0.0f, 0.0f);
		vertices2[2].Position = Vector3(+0.0f, -0.5f, 0.0f);

		vertices2[3].Position = Vector3(0.0f, -0.5f, 0.0f);
		vertices2[4].Position = Vector3(-0.5f, 0.0f, 0.0f);
		vertices2[5].Position = Vector3(0.0f, 0.0f, 0.0f);

		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices2;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer2));
	}

}


void Rect::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(vertexBuffer2);
}

void Rect::Update()
{

}

void Rect::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//IA : �������� �ʿ��� ������ ����(���ؽ� ����, �׸��� ��� ����)
	//VS : ���ؽ� ���̴�
	//RS : �����Ͷ���¡
	//PS : �ȼ� ���̴�

	//Draw ù��° ���� : ���̴��� T0, T1,,
	//Draw �ι�° ���� : ���̴��� P0, P1,,
	//Draw ����° ���� : ���� ����
	//Draw ����° ���� : ���� �ε���


	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	shader->Draw(0, 1, 6);



	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer2, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 1, 6);
}