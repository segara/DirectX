#include "stdafx.h"
#include "TriangleList.h"

void TriangleList::Initialize()
{
	shader = new Shader(L"05_Triangle.fx");
	
	{
		vertices[0].Position = Vector3(-0.5f, +0.0f, 0.0f);
		vertices[1].Position = Vector3(+0.0f, +0.5f, 0.0f);
		vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

		vertices[3].Position = Vector3(0.5f, -0.5f, 0.0f);
		vertices[4].Position = Vector3(-0.5f, -0.5f, 0.0f);
		vertices[5].Position = Vector3(0.0f, 0.0f, 0.0f);

		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	/*{
		vertices2[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
		vertices2[1].Position = Vector3(0.0f, 0.0f, 0.0f);
		vertices2[2].Position = Vector3(+0.5f, -0.5f, 0.0f);

		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 3;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices2;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer2));
	}*/
}


void TriangleList::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	//SafeRelease(vertexBuffer2);
}

void TriangleList::Update()
{

}

void TriangleList::Render()
{
	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//IA : 렌더링에 필요한 정보를 셋팅(버텍스 버퍼, 그리는 방법 셋팅)
	//VS : 버텍스 세이더
	//RS : 레스터라이징
	//PS : 픽셀 세이더

	//Draw 첫번째 인자 : 세이더의 T0, T1,,
	//Draw 두번째 인자 : 세이더의 P0, P1,,
	//Draw 세번째 인자 : 정점 개수
	//Draw 세번째 인자 : 시작 인덱스


	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	shader->Draw(0, 0, 3);


	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	shader->Draw(0, 1, 3,3);
}