#include "stdafx.h"
#include "Cube.h"

void Cube::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(13, 0, 0);
	Context::Get()->GetCamera()->Position(0, 0, -19);
	shader = new Shader(L"10_Cube.fx");

	CreateVertex();
	CreateIndice();
}


void Cube::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void Cube::Update()
{
	Matrix Scale, Transform, Rotation;

		if (Keyboard::Get()->Press(VK_RIGHT)) {
			rotation.x += 2.0f * Time::Delta();
		}
		if (Keyboard::Get()->Press(VK_LEFT)) {
			rotation.x -= 2.0f * Time::Delta();
		}
		if (Keyboard::Get()->Press(VK_UP)) {
			rotation.y += 2.0f * Time::Delta();
		}
		if (Keyboard::Get()->Press(VK_DOWN)) {
			rotation.y -= 2.0f * Time::Delta();
		}
	D3DXMatrixScaling(&Scale, scale.x, scale.y, scale.z);
	D3DXMatrixRotationYawPitchRoll(&Rotation, rotation.x, rotation.y, rotation.z);
	D3DXMatrixTranslation(&Transform, position.x, position.y, position.z);
	world_Transform = Scale *Rotation* Transform;
}

void Cube::Render()
{
	//shader 내의 변수와 연결 : AsMatrix

	shader->AsVector("Color")->SetFloatVector(color);
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

void Cube::CreateVertex()
{
	
	float width = 0.5f, height = 0.5f, depth = 0.5f;
	vertexCount = 4 * 6; //4:one face //6:total face
	vertices = new Vertex[vertexCount];

	//front face

	vertices[0].Position = Vector3(-width, -height, -depth);
	vertices[1].Position = Vector3(-width, height, -depth);
	vertices[2].Position = Vector3(width, -height, -depth);
	vertices[3].Position = Vector3(width, height, -depth);
	
	//front - left
	vertices[4].Position = Vector3(-width, -height, depth);
	vertices[5].Position = Vector3(-width, height, depth);
	vertices[6].Position = Vector3(-width, -height, -depth);
	vertices[7].Position = Vector3(-width, height, -depth);

	//front - right
	vertices[8].Position = Vector3(width, -height, -depth);
	vertices[9].Position = Vector3(width, height, -depth);
	vertices[10].Position = Vector3(width, -height, depth);
	vertices[11].Position = Vector3(width, height, depth);

	//front - up
	vertices[12].Position = Vector3(-width, height, -depth);
	vertices[13].Position = Vector3(-width, height, depth);
	vertices[14].Position = Vector3(width, height, -depth);
	vertices[15].Position = Vector3(width, height, depth);

	//front - bottom
	vertices[16].Position = Vector3(-width, -height, depth);
	vertices[17].Position = Vector3(-width, -height, -depth);
	vertices[18].Position = Vector3(width, -height, depth);
	vertices[19].Position = Vector3(width, -height, -depth);

	//front - back (후면이 보이기 위해서는 뒤방향에서 시계방향 순서대로 만들어줘야함 , 즉 아래 코드에서는 시계반대방향
	vertices[20].Position = Vector3(-width, -height, depth); //20
	vertices[21].Position = Vector3(width, -height, depth);  //21
	vertices[22].Position = Vector3(-width, height, depth); //22
	vertices[23].Position = Vector3(width, height, depth);//23



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

void Cube::CreateIndice()
{

	indexCount = 6 * 6; //각 네모당 인덱스가 6개 (삼각형2개)
	indices = new UINT[indexCount]{
		0,1,2,2,1,3,
		4,5,6,6,5,7,
		8,9,10,10,9,11,
		12,13,14,14,13,15,
		16,17,18,18,17,19,
		20,21,22,22,21,23,	
	};
	
	
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

void Cube::CreateVertexEnemy(ID3D11Buffer** vertexBuffer)
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

bool Cube::CheckRectCollision(Matrix player, Matrix enemy)
{
	return IsIntersect(player._41 - 0.5f, player._42 - 0.5f, 0.5f, 0.5f, enemy._41 - 0.5f, enemy._42-0.5f, 0.5f, 0.5f);
	return false;
}
bool Cube::IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{ 
	if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
	{ 
		return true; 
	} 
	return false; 
}