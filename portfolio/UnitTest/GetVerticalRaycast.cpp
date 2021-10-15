#include "stdafx.h"
#include "GetVerticalRaycast.h"
#include "../Framework/Environment/Terrain.h"
#include "../Framework/Viewer/MoveCamera.h"
void GetVerticalRaycast::Initialize()
{
	Context::Get()->GetCamera()->RotationDegree(12, 0, 0);
	Context::Get()->GetCamera()->Position(35, 10, -55);

	((MoveCamera*)(Context::Get()->GetCamera()))->Speed(20);

	shader = new Shader(L"15_Terrain_Diffuse.fx");

	terrain = new Terrain(shader, L"Terrain/Gray256.png");


	///make player///

	tri_shader = new Shader(L"09_World.fx");

	vertices[0].Position = Vector3(0, 1, 0);
	vertices[1].Position = Vector3(-1, 0, 0);
	vertices[2].Position = Vector3(1, 0, 0);

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(Vertex) * 3;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = vertices;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));

	

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


void GetVerticalRaycast::Destroy()
{
	SafeDelete(shader);
	SafeDelete(tri_shader);

	SafeDelete(terrain);
	SafeRelease(vertexBuffer);
}

void GetVerticalRaycast::Update()
{
	terrain->Update();
	position.y = terrain->GetVerticalRaycast(position);

	
	if (Keyboard::Get()->Press(VK_RIGHT))
		position.x += 20.0f * Time::Delta();
	if (Keyboard::Get()->Press(VK_LEFT))
		position.x -= 20.0f * Time::Delta();

	if (Keyboard::Get()->Press(VK_UP))
		position.z += 20.0f * Time::Delta();
	if (Keyboard::Get()->Press(VK_DOWN))
		position.z -= 20.0f * Time::Delta();

	Matrix R, T;
	D3DXMatrixTranslation(&T, position.x, position.y, position.z);
	D3DXMatrixRotationY(&R, Math::ToRadian(180));

	tri_shader->AsMatrix("World")->SetMatrix(R*T);
	tri_shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	tri_shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

	/*static UINT address = 0;
	ImGui::InputInt("Address", (int*)&address);
	address %= 4;

	shader->AsScalar("Address")->SetInt(address);*/
	//if (ImGui::Button("Open"))
	//{
	//	function<void(wstring)> f = bind(&GetVerticalRaycast::LoadTexture, this, placeholders::_1); //placeholders:���� ����
	//	D3DDesc desc = D3D::GetDesc(); //window ����
	//	Path::OpenFileDialog(L"", Path::ImageFilter, L"../../_Texture", f, desc.Handle);
	//}
}

void GetVerticalRaycast::Render()
{
	terrain->Render();

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	tri_shader->Draw(0, 0, 3);
	//shader ���� ������ ���� : AsMatrix
	//if(texture != NULL)
	//	shader->AsSRV("Map")->SetResource(texture->SRV());

	//if (texture != NULL)
	//	shader->AsSRV("Map2")->SetResource(texture2->SRV());

	//shader->AsMatrix("World")->SetMatrix(world_Transform);
	//shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	//shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());


	/*UINT stride = sizeof(VertexTexture);
	UINT offset = 0;*/
	//����
	//IA : �������� �ʿ��� ������ ����(���ؽ� ����, �׸��� ��� ����)
	//VS : ���ؽ� ���̴�
	//RS : �����Ͷ���¡
	//PS : �ȼ� ���̴�

	//Draw ù��° ���� : ���̴��� T0, T1,,
	//Draw �ι�° ���� : ���̴��� P0, P1,,
	//Draw ����° ���� : ���� ����
	//Draw ����° ���� : ���� �ε���

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

	string str = to_string(position.x) + ", " + to_string(position.y) + ", " + to_string(position.z);
	Gui::Get()->RenderText(5,60,str);

}

//void GetVerticalRaycast::LoadTexture(wstring file)
//{
//	//MessageBox(D3D::GetDesc().Handle, file.c_str(), L"", MB_OK);
//	SafeDelete(texture);
//	texture = new Texture(file);
//
//	//Check(D3DX11CreateShaderResourceViewFromFile(D3D::GetDevice(), file.c_str(), NULL, NULL, &srv, NULL));
//
//}

void GetVerticalRaycast::CreateVertex()
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

	////vetex��ǥ       ===     uv��ǥ ��Ī
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

void GetVerticalRaycast::CreateVertexEnemy(ID3D11Buffer** vertexBuffer)
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

bool GetVerticalRaycast::CheckRectCollision(Matrix player, Matrix enemy)
{
	return IsIntersect(player._41 - 0.5f, player._42 - 0.5f, 0.5f, 0.5f, enemy._41 - 0.5f, enemy._42-0.5f, 0.5f, 0.5f);
	return false;
}
bool GetVerticalRaycast::IsIntersect(int x1, int y1, int w1, int h1, int x2, int y2, int w2, int h2)
{ 
	if (x1 + w1 >= x2 && x1 <= x2 + w2 && y1 + h1 >= y2 && y1 <= y2 + h2)
	{ 
		return true; 
	} 
	return false; 
}