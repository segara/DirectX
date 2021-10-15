#include "Framework.h"
#include "Environment/Terrain.h"

Terrain::Terrain(Shader * shader, wstring heightFile) :shader(shader)
{
	heightMap = new Texture(heightFile);
	CreateVertexData();
	CreateIndexData();
	CreateNormalData();

	D3DXMatrixIdentity(&world);
}

Terrain::~Terrain()
{
	SafeDelete(heightMap);

	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);

	SafeRelease(vertexBuffer);
	SafeRelease(indexBuffer);
}

void Terrain::Update()
{
	static Vector3 direction = Vector3(-1, -1, 1);
	ImGui::SliderFloat3("Direction", direction, -1, 1);
	shader->AsVector("Direction")->SetFloatVector(direction);

	DebugLine::Get()->RenderLine(0, 0, 0, direction.x, direction.y, direction.z, 1, 0, 0);


	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());

}

void Terrain::Render()
{
	for (int i = 0; i < vertexCount; i++)
	{
		Vector3 start = vertices[i].Position;
		Vector3 end = vertices[i].Position + vertices[i].Normal * 2;

		DebugLine::Get()->RenderLine(start, end);
	}

	UINT stride = sizeof(TerrainVertex);
	UINT offset = 0;

	//IA : �������� �ʿ��� ������ ����(���ؽ� ����, �׸��� ��� ����)
	//VS : ���ؽ� ���̴�
	//RS : �����Ͷ���¡
	//PS : �ȼ� ���̴�

	//Draw ù��° ���� : ���̴��� T0, T1,,
	//Draw �ι�° ���� : ���̴��� P0, P1,,
	//Draw ����° ���� : ���� ����
	//Draw ����° ���� : ���� �ε���

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetIndexBuffer(indexBuffer, DXGI_FORMAT_R32_UINT, 0);

	//shader->AsMatrix("World")->SetMatrix(world_Transform);
	shader->DrawIndexed(0, pass, indexCount);

}
float Terrain::GetHeight(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x<0 || x>width) return FLT_MIN;
	if (z<0 || z>height) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	Vector3 v[4];
	for (int i = 0; i < sizeof(index) / sizeof(UINT); ++i)
		v[i] = vertices[index[i]].Position;
	

	float ddx = (position.x - v[0].x) / 1.0f;   //�簢�� �Ѻ��� ���� 1�̶�� �����ϰ�, ���� * �̵��Ÿ� ��ŭ ������
	float ddz = (position.z - v[0].z) / 1.0f;

	Vector3 result;
	
	//�簢���� �̷�� �� �ﰢ������ �÷��̾ �Ʒ� �ﰢ���� ������
	if (ddx + ddz <= 1) {
		result = v[0] + (v[2] - v[0]) * ddx + (v[1] - v[0]) * ddz; //x���Ϳ� z���͸� ���� ���ѵ� ������
	}
	else //�簢���� �̷�� �� �ﰢ������ �÷��̾ �� �ﰢ���� ������
	{
		//1 ���� ���ִ� ���� : x���а� z������ 1,1�϶� v[3] ��ġ�� 
		 ddx = 1.0f - ddx;   
		 ddz = 1.0f - ddz;

		 result = v[3] + ((v[1] - v[3]) * ddx + (v[2] - v[3]) * ddz);

	}

	return result.y;
}
float Terrain::GetVerticalRaycast(Vector3 & position)
{
	UINT x = (UINT)position.x;
	UINT z = (UINT)position.z;

	if (x<0 || x>width) return FLT_MIN;
	if (z<0 || z>height) return FLT_MIN;

	UINT index[4];
	index[0] = width * z + x;
	index[1] = width * (z + 1) + x;
	index[2] = width * z + (x + 1);
	index[3] = width * (z + 1) + (x + 1);

	Vector3 p[4];
	for (int i = 0; i < sizeof(index) / sizeof(UINT); ++i)
		p[i] = vertices[index[i]].Position;

	Vector3 startPos(position.x, 100, position.z);
	Vector3 direction(0, -1, 0);

	float u, v, distance;

	Vector3 result(-1, FLT_MIN, -1);
	if (D3DXIntersectTri(&p[0], &p[1], &p[2], &startPos, &direction, &u, &v, &distance)==TRUE) {
		result = p[0] + (p[1] - p[0])*u + (p[2] - p[0])*v;
	}
	if (D3DXIntersectTri(&p[3], &p[1], &p[2], &startPos, &direction, &u, &v, &distance)==TRUE) {
		result = p[3] + (p[1] - p[3])*u + (p[2] - p[3])*v;
	}
	return result.y;
}
Vector3 Terrain::GetRaycastPosition()
{
	Matrix V = Context::Get()->View();
	Matrix P = Context::Get()->Projection();
	Viewport* vp = Context::Get()->GetViewport();

	Vector3 mouse = Mouse::Get()->GetPosition();

	string str = "";
	str += to_string(mouse.x) + ", ";
	str += to_string(mouse.y) + ", ";

	Gui::Get()->RenderText(Vector2(10, 80), Color(1, 0, 0, 1), str);

	//ray ������ ��� ���� ���
	//Unproject�� 2d�� ��ǥ�� 3d ��ǥȭ �ϴ� ���̹Ƿ�
	//���콺 ��ǥ�� z�� �������༭ �־��൵ ���� �ٲ�� �ȴ�
	Vector3 n, f; 
	mouse.z = 0.0f; //0.0f: �ٸ� z
	vp->Unproject(&n, mouse, world, V, P);

	mouse.z = 1.0f; //1.0f: ���� z
	vp->Unproject(&f, mouse, world, V, P); 

	Vector3 start = n;
	Vector3 direction = f - n;

	for (int z = 0; z < height - 1; ++z)
	{
		for (int x = 0; x < width - 1; ++x)
		{
			UINT index[4];
			index[0] = width * z + x;
			index[1] = width * (z + 1) + x;
			index[2] = width * z + (x + 1);
			index[3] = width * (z + 1) + (x + 1);

			Vector3 p[4];
			for (int i = 0; i < sizeof(index) / sizeof(UINT); ++i)
				p[i] = vertices[index[i]].Position;

			float u, v, distance;
			
			if (D3DXIntersectTri(&p[0], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE) {
				return p[0] + (p[1] - p[0])*u + (p[2] - p[0])*v;
			}
			if (D3DXIntersectTri(&p[3], &p[1], &p[2], &start, &direction, &u, &v, &distance) == TRUE) {
				return p[3] + (p[1] - p[3])*u + (p[2] - p[3])*v;
			}
		}
	}
	return Vector3(-1, FLT_MIN, -1);
}
void Terrain::CreateVertexData()
{
	vector<Color> heights;
	heightMap->ReadPixel(DXGI_FORMAT_R8G8B8A8_UNORM, &heights); //�̹��� �Ʒ����� ���� �о����
	//����
	//	vector<Color> heights; ���� vector<Color> �� �ڷ���, �� int* a = &b �� �־��ִ°Ͱ� ����

	width = heightMap->GetWidth();
	height = heightMap->GetHeight();

	vertexCount = width * height;
	vertices = new TerrainVertex[vertexCount];

	for (UINT z = 0; z < height; z++)
	{
		for (UINT x = 0; x < width; x++)
		{
			UINT index = width * z + x;
			UINT pixel = width * (height - 1 - z) + x; //���������� �Ųٷ� ������ �ְ�
			vertices[index].Position.x = (float)x;
			vertices[index].Position.y = heights[pixel].r * 255.0f / 10.0f; //255�� �ʹ� ���� ������ 10���� ����
			vertices[index].Position.z = (float)z;
		}
	}

	
	
	

}

void Terrain::CreateIndexData()
{
	indexCount = ((width - 1) * (height-1)) * 6; //�� �׸�� �ε����� 6�� (�ﰢ��2��)
	//-1 �ϴ����� : 0���� x<width ���Ǳ��� ����ϱ� ����
	indices = new UINT[indexCount];

	UINT index = 0; //�ε��� �迭�� �ε���
	for (int y = 0; y < height-1; ++y)
	{
		for (int x = 0; x < width-1; ++x)
		{
			//�����ϴ�(�� �ε���)
			indices[index + 0] = (width) * y + x;
			//���ʻ��
			indices[index + 1] = (width) * (y + 1) + x;
			//������ �ϴ�
			indices[index + 2] = (width) * y + (x + 1);
			//������ �ϴ�
			indices[index + 3] = (width) * y + (x + 1);
			//���ʻ��
			indices[index + 4] = (width) * (y + 1) + x;
			//������ ���
			indices[index + 5] = (width) * (y + 1) + (x + 1);

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

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &indexBuffer)); //Check : Assert �� ���� ����
	}
	//SafeDeleteArray(indices);//���� �Ϸ� �� delete

}

void Terrain::CreateNormalData()
{
	//�ε������� /3 = �ﰢ�� ����
	for (UINT i = 0; i < indexCount / 3; i++)
	{
		UINT index0 = indices[i * 3 + 0];
		UINT index1 = indices[i * 3 + 1];
		UINT index2 = indices[i * 3 + 2];

		TerrainVertex v0 = vertices[index0];
		TerrainVertex v1 = vertices[index1];
		TerrainVertex v2 = vertices[index2];

		Vector3 a = v1.Position - v0.Position;
		Vector3 b = v2.Position - v0.Position;

		Vector3 normal;
		D3DXVec3Cross(&normal, &a, &b);

		//normalize �� normal ���� �������� normalize �Ѱ��� ���ؽ� ������ �ֺ������� �븻���� ��հ��� ���� ���
		vertices[index0].Normal += normal;
		vertices[index1].Normal += normal;
		vertices[index2].Normal += normal;
	 }

	for (UINT i = 0; i < vertexCount; i++)
	{
		D3DXVec3Normalize(&vertices[i].Normal, &vertices[i].Normal);
	}

	//create vertex buffer
	{
		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(TerrainVertex) * vertexCount;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer)); //Check : Assert �� ���� ����
	}
	//SafeDeleteArray(vertices);//���� �Ϸ� �� delete
}

