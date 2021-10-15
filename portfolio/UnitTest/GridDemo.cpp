#include "stdafx.h"
#include "GridDemo.h"

void GridDemo::Initialize()
{
	shader = new Shader(L"09_World.fx");

	CreateVertex();
	CreateIndice();
}


void GridDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void GridDemo::Update()
{
	Matrix Scale, Transform;


	
		//if (Keyboard::Get()->Press(VK_RIGHT)) {
		//	position.x += 2.0f * Time::Delta();
		//}
		//if (Keyboard::Get()->Press(VK_LEFT)) {
		//	position.x -= 2.0f * Time::Delta();
		//}
		//if (Keyboard::Get()->Press(VK_UP)) {
		//	position.y += 2.0f * Time::Delta();
		//}
		//if (Keyboard::Get()->Press(VK_DOWN)) {
		//	position.y -= 2.0f * Time::Delta();
		//}


	D3DXMatrixScaling(&Scale, scale.x, scale.y, scale.z);
	D3DXMatrixTranslation(&Transform, position.x, position.y, position.z);
	world_Transform = Scale * Transform;
}

void GridDemo::Render()
{
	//shader 내의 변수와 연결 : AsMatrix

	shader->AsScalar("Index")->SetInt(0);
	shader->AsMatrix("World")->SetMatrix(world_Transform);
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
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//shader->AsMatrix("World")->SetMatrix(world_Transform);
	shader->DrawIndexed(0, 1, indexCount);

}

void GridDemo::CreateVertex()
{
	
	vertexCount = (width + 1) * (height + 1);
	vertices = new Vertex[vertexCount];

	for (int y = 0; y <= height; ++y)
	{
		for (int x = 0; x <= width; ++x)
		{
			UINT i = (width + 1) * y + x; //정점배열의 인덱스 구하기
			vertices[i].Position.x = (float)x;
			vertices[i].Position.y = 0.0f;
			vertices[i].Position.z = (float)y;
		}
	}

	//create vertex buffer
	{
		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer)); //Check : Assert 와 같은 사용법
	}
	SafeDeleteArray(vertices);//복사 완료 후 delete
}

void GridDemo::CreateIndice()
{

	indexCount = (width * height) * 6; //각 네모당 인덱스가 6개 (삼각형2개)
	indices = new UINT[indexCount];

	UINT index = 0; //인덱스 배열의 인덱스
	for (int y = 0; y < height; ++y)
	{
		for (int x = 0; x < width; ++x)
		{
			//왼쪽하단(의 인덱스)
			indices[index + 0] = (width + 1) * y + x;
			//왼쪽상단
			indices[index + 1] = (width + 1) * (y + 1) + x;
			//오른쪽 하단
			indices[index + 2] = (width + 1) * y + (x + 1);
			//오른쪽 하단
			indices[index + 3] = (width + 1) * y + (x + 1);
			//왼쪽상단
			indices[index + 4] = (width + 1) * (y + 1) + x;
			//오른쪽 상단
			indices[index + 5] = (width + 1) * (y + 1) + (x + 1);

			index += 6;

		}
	}
	
	//create Index buffer
	{
		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(UINT) * indexCount;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = indices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer)); //Check : Assert 와 같은 사용법
	}
	SafeDeleteArray(indices);//복사 완료 후 delete

}

void GridDemo::CreateVertexEnemy(ID3D11Buffer** vertexBuffer)
{
	////Local
	//vertices2[0].Position = Vector3(0.0f, +0.0f, 0.0f);
	//vertices2[1].Position = Vector3(+0.0f, 0.5f, 0.0f);
	//vertices2[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

	//vertices2[3].Position = Vector3(0.5f, 0.0f, 0.0f);
	//vertices2[4].Position = Vector3(0.0f, 0.5f, 0.0f);
	//vertices2[5].Position = Vector3(0.5f, 0.5f, 0.0f);

	//D3D11_BUFFER_DESC desc;

	//ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	//desc.ByteWidth = sizeof(Vertex) * 6;
	//desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	//D3D11_SUBRESOURCE_DATA subResource = { 0 };
	//subResource.pSysMem = vertices2;

	//Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, vertexBuffer));
}

bool GridDemo::CheckRectCollision(Matrix player, Matrix enemy)
{
	return IsIntersect(player._41 - 0.5f, player._42 - 0.5f, 0.5f, 0.5f, enemy._41 - 0.5f, enemy._42-0.5f, 0.5f, 0.5f);
	return false;
}
bool GridDemo::IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{ 
	if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
	{ 
		return true; 
	} 
	return false; 
}