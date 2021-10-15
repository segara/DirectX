#include "stdafx.h"
#include "GetRayCastDemo.h"
#include "../Framework/Environment/Terrain.h"
#include "../Framework/Viewer/MoveCamera.h"
void GetRayCastDemo::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(12, 0, 0);
	Context::Get()->GetCamera()->Position(35, 10, -55);

	((MoveCamera*)(Context::Get()->GetCamera()))->Speed(20);

	shader = new Shader(L"13_Terrain.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");
	

	/*for (int i = 0; i < sizeof(vertexBufferEnemy) / sizeof(ID3D11Buffer*); ++i)
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
	}*/
}


void GetRayCastDemo::Destroy()
{
	SafeDelete(shader);

	SafeDelete(terrain);
}

void GetRayCastDemo::Update()
{
	Vector3 position = terrain->GetRaycastPosition();

	string str = "";
	str += to_string(position.x) + ", ";
	str += to_string(position.y) + ", ";
	str += to_string(position.z) + ", ";

	Gui::Get()->RenderText(Vector2(10, 60), Color(1, 0, 0, 1), str);

	terrain->Update();

	
	
	/*static UINT address = 0;
	ImGui::InputInt("Address", (int*)&address);
	address %= 4;

	shader->AsScalar("Address")->SetInt(address);*/
	//if (ImGui::Button("Open"))
	//{
	//	function<void(wstring)> f = bind(&GetRayCastDemo::LoadTexture, this, placeholders::_1); //placeholders:인자 갯수
	//	D3DDesc desc = D3D::GetDesc(); //window 정보
	//	Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Texture", f, desc.Handle);
	//}
}

void GetRayCastDemo::Render()
{
	terrain->Render();


	//shader 내의 변수와 연결 : AsMatrix
	//if(texture != NULL)
	//	shader->AsSRV("Map")->SetResource(texture->SRV());

	//if (texture != NULL)
	//	shader->AsSRV("Map2")->SetResource(texture2->SRV());

	//shader->AsMatrix("World")->SetMatrix(world_Transform);
	//shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	//shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());


	/*UINT stride = sizeof(VertexTexture);
	UINT offset = 0;*/
	//설명
	//IA : 렌더링에 필요한 정보를 셋팅(버텍스 버퍼, 그리는 방법 셋팅)
	//VS : 버텍스 세이더
	//RS : 레스터라이징
	//PS : 픽셀 세이더

	//Draw 첫번째 인자 : 세이더의 T0, T1,,
	//Draw 두번째 인자 : 세이더의 P0, P1,,
	//Draw 세번째 인자 : 정점 개수
	//Draw 세번째 인자 : 시작 인덱스

	/*D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	shader->DrawIndexed(0, 0, 6);
*/

	//for (int i = 0; i < sizeof(vertexBufferEnemy) / sizeof(ID3D11Buffer*); ++i)
	//{
	//	if (CheckRectCollision(world_Transform, world_enemy[i]))
	//	{
	//		continue;
	//	}
	//	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBufferEnemy[i], &stride, &offset);
	//	shader->AsScalar("Index")->SetInt(1);
	//	shader->AsMatrix("World")->SetMatrix(world_enemy[i]);
	//	shader->Draw(0, 0, 6);
	//}


}

//void GetRayCastDemo::LoadTexture(wstring file)
//{
//	//MessageBox(D3D::GetDesc().Handle, file.c_str(), L"", MB_OK);
//	SafeDelete(texture);
//	texture = new Texture(file);
//
//	//Check(D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), file.c_str(), NULL, NULL, &srv, NULL));
//
//}

void GetRayCastDemo::CreateVertex()
{
	////      1     3       
	////      
	////      0     2
	////
	////Local
	//vertices[0].Position = Vector3(-0.5f, -0.5f, 0.0f);
	//vertices[1].Position = Vector3(-0.5f, 0.5f, 0.0f);
	//vertices[2].Position = Vector3(0.5f, -0.5f, 0.0f);
	//vertices[3].Position = Vector3(0.5f, 0.5f, 0.0f);

	////00      10
	////
	////
	////01      11

	////vetex좌표       ===     uv좌표 매칭
	//vertices[0].Uv = Vector2(0, 1.0f);
	//vertices[1].Uv = Vector2(0, 0);
	//vertices[2].Uv = Vector2(1.0f, 1.0f);
	//vertices[3].Uv = Vector2(1.0f, 0);

	//
	//D3D11_BUFFER_DESC desc;
	//ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	//desc.ByteWidth = sizeof(VertexTexture) * 4;
	//desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	//D3D11_SUBRESOURCE_DATA subResource = { 0 };
	//subResource.pSysMem = vertices;

	//Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));



	//D3D11_BUFFER_DESC desc_index;
	//ZeroMemory(&desc_index, sizeof(D3D11_BUFFER_DESC));
	//desc_index.ByteWidth = sizeof(UINT) * 6;
	//desc_index.BindFlags = D3D11_BIND_INDEX_BUFFER;
	//D3D11_SUBRESOURCE_DATA subResource_index = { 0 };
	//subResource_index.pSysMem = indices;

	//Check(D3D::GetDevice()->CreateBuffer(&desc_index, &subResource_index, &indexBuffer));
}

void GetRayCastDemo::CreateVertexEnemy(ID3D11Buffer** vertexBuffer)
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

bool GetRayCastDemo::CheckRectCollision(Matrix player, Matrix enemy)
{
	return IsIntersect(player._41 - 0.5f, player._42 - 0.5f, 0.5f, 0.5f, enemy._41 - 0.5f, enemy._42-0.5f, 0.5f, 0.5f);
	return false;
}
bool GetRayCastDemo::IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{ 
	if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
	{ 
		return true; 
	} 
	return false; 
}