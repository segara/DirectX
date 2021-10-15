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
	//shader ���� ������ ���� : AsMatrix
	
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());


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


