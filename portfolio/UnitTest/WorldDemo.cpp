#include "stdafx.h"
#include "WorldDemo.h"

void WorldDemo::Initialize()
{
	shader = new Shader(L"08_World.fx");
	
	
		//Local
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
	
		D3DXMatrixIdentity(&world);

	

}


void WorldDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void WorldDemo::Update()
{
	/*if (Keyboard::Get()->Press(VK_RIGHT)) {
		world._41 += 2.0f * Time::Delta();
	}
	if (Keyboard::Get()->Press(VK_LEFT)) {
		world._41 -= 2.0f * Time::Delta();
	}*/   

	if (Keyboard::Get()->Press(VK_RIGHT)) {
		world._11 += 2.0f * Time::Delta();
		world._22 += 2.0f * Time::Delta();
	}
	if (Keyboard::Get()->Press(VK_LEFT)) {
		world._11 -= 2.0f * Time::Delta();
		world._22 -= 2.0f * Time::Delta();
	}
}

void WorldDemo::Render()
{
	//shader 내의 변수와 연결 : AsMatrix
	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());


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
	
	shader->Draw(0, 0, 6);


}