#include "stdafx.h"
#include "WorldDemo2.h"
#include "WorldDemo3.h"

void WorldDemo2::Initialize()
{
	shader = new Shader(L"09_World.fx");


	

	for (int i = 0; i < sizeof(world)/sizeof(Matrix); ++i)
	{
		D3DXMatrixIdentity(&world[i]);
	}

}


void WorldDemo2::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void WorldDemo2::Update()
{
	ImGui::InputInt("Select", (int *)&index);
	index %= 3;
	if (Keyboard::Get()->Press(VK_RIGHT)) {
		world[index]._41 += 2.0f * Time::Delta();
	}
	if (Keyboard::Get()->Press(VK_LEFT)) {
		world[index]._41 -= 2.0f * Time::Delta();
	}   
}

void WorldDemo2::Render()
{
	//shader 내의 변수와 연결 : AsMatrix
	
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

	shader->AsScalar("Index")->SetInt(0);
	shader->AsMatrix("World")->SetMatrix(world[0]);
	shader->Draw(0, 0, 6);

	shader->AsScalar("Index")->SetInt(1);
	shader->AsMatrix("World")->SetMatrix(world[1]);
	shader->Draw(0, 0, 6);

	shader->AsScalar("Index")->SetInt(2);
	shader->AsMatrix("World")->SetMatrix(world[2]);
	shader->Draw(0, 0, 6);

}


