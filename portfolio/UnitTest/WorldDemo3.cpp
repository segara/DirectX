#include "stdafx.h"
#include "WorldDemo3.h"

void WorldDemo3::Initialize()
{
	shader = new Shader(L"09_World.fx");

	CreateVertex();

	for (int i = 0; i < sizeof(vertexBufferEnemy) / sizeof(ID3D11Buffer*); ++i)
	{
		CreateVertexEnemy(&vertexBufferEnemy[i]);
	}
	

	D3DXMatrixIdentity(&world_Transform);

	for (int i = 0; i < sizeof(world_enemy) / sizeof(Matrix); ++i)
	{
		Matrix  EnemyTransform;
		int random_positionX = rand() % 5;
		int random_positionY = rand() % 5;
		int random_positionZ = rand() % 5;
		D3DXMatrixTranslation(&EnemyTransform, random_positionX, random_positionY, 0);
	
		world_enemy[i] = EnemyTransform;
	}
}


void WorldDemo3::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void WorldDemo3::Update()
{
	Matrix Scale, Transform;


	
		if (Keyboard::Get()->Press(VK_RIGHT)) {
			position.x += 2.0f * Time::Delta();
		}
		if (Keyboard::Get()->Press(VK_LEFT)) {
			position.x -= 2.0f * Time::Delta();
		}
		if (Keyboard::Get()->Press(VK_UP)) {
			position.y += 2.0f * Time::Delta();
		}
		if (Keyboard::Get()->Press(VK_DOWN)) {
			position.y -= 2.0f * Time::Delta();
		}


	D3DXMatrixScaling(&Scale, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&Transform, position.x, position.y, position.z);
	world_Transform = Scale * Transform;
}

void WorldDemo3::Render()
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

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	
	shader->AsScalar("Index")->SetInt(0);
	shader->AsMatrix("World")->SetMatrix(world_Transform);
	shader->Draw(0, 0, 6);


	for (int i = 0; i < sizeof(vertexBufferEnemy) / sizeof(ID3D11Buffer*); ++i)
	{
		if (CheckRectCollision(world_Transform, world_enemy[i]))
		{
			continue;
		}
		D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBufferEnemy[i], &stride, &offset);
		shader->AsScalar("Index")->SetInt(1);
		shader->AsMatrix("World")->SetMatrix(world_enemy[i]);
		shader->Draw(0, 0, 6);
	}


}

void WorldDemo3::CreateVertex()
{
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
}

void WorldDemo3::CreateVertexEnemy(ID3D11Buffer** vertexBuffer)
{
	//Local
	vertices2[0].Position = Vector3(0.0f, +0.0f, 0.0f);
	vertices2[1].Position = Vector3(+0.0f, 0.5f, 0.0f);
	vertices2[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

	vertices2[3].Position = Vector3(0.5f, 0.0f, 0.0f);
	vertices2[4].Position = Vector3(0.0f, 0.5f, 0.0f);
	vertices2[5].Position = Vector3(0.5f, 0.5f, 0.0f);

	D3D11_BUFFER_DESC desc;

	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 6;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices2;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, vertexBuffer));
}

bool WorldDemo3::CheckRectCollision(Matrix player, Matrix enemy)
{
	return IsIntersect(player._41 - 0.5f, player._42 - 0.5f, 0.5f, 0.5f, enemy._41 - 0.5f, enemy._42-0.5f, 0.5f, 0.5f);
	return false;
}
bool WorldDemo3::IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{ 
	if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
	{ 
		return true; 
	} 
	return false; 
}