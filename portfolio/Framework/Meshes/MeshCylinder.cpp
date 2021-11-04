#include "Framework.h"
#include "MeshCylinder.h"




MeshCylinder::MeshCylinder(float radius, float height, UINT stackCount, UINT sliceCount)
	: top_radius(radius), bottom_radius(radius), height(height), stackCount(stackCount), sliceCount(sliceCount)
{
}

MeshCylinder::MeshCylinder(float topRadius, float bottomRadius, float height, UINT stackCount, UINT sliceCount)
	: top_radius(topRadius), bottom_radius(bottomRadius), height(height), stackCount(stackCount), sliceCount(sliceCount)
{
}

MeshCylinder::~MeshCylinder()
{
}

void MeshCylinder::Create()
{
	vector<MeshVertex> v;
	
	float stackHeight = height / (float)stackCount;
	float radiusStep = (top_radius - bottom_radius) / (float)stackCount; //������ ���̸� �������� ����

	UINT ringCount = stackCount + 1;
	for (UINT i = 0; i < ringCount; i++)
	{
		float y = (-0.5f * height) + i * stackHeight; //������ ��ġ�ϱ� ���� : (-0.5f * height)
		float r = bottom_radius + i * radiusStep;
		float theta = 2.0f * Math::PI / (float)sliceCount;

		for (UINT k = 0; k <= sliceCount; k++)
		{
			float c = cosf(k * theta);
			float s = sinf(k * theta);

			MeshVertex vertex;

			vertex.Position = Vector3(r*c, y, r*s);
			vertex.Uv = Vector2((float)k / (float)sliceCount, 1.0f - (float)i / (float)stackCount);

			//���� �Ʒ����� �ʺ� �ٸ��Ƿ�, �븻�� ������ �������� �ȴ� �׷��� ������ ���
			Vector3 tangent = Vector3(-s, 0.0f, c);
			float dr = bottom_radius - top_radius;
			
			Vector3 biTangent = Vector3(dr * c, -height, dr*s);
			
			D3DXVec3Cross(&vertex.Normal, &tangent, &biTangent);
			 
			D3DXVec3Normalize(&vertex.Normal, &vertex.Normal);

			vertex.Tangent = tangent;

			v.push_back(vertex);
		}
	}

	vector<UINT> i;
	UINT ringVertexCount = sliceCount + 1; //grid���鶧 ó�� ī��Ʈ�� + 1
	for (UINT y = 0; y < stackCount; y++)
	{
		for (UINT x = 0; x < sliceCount; x++)
		{
			i.push_back(y * ringVertexCount + x);
			i.push_back((y + 1) * ringVertexCount + x);
			i.push_back((y + 1) * ringVertexCount + (x + 1));

			i.push_back(y * ringVertexCount + x);
			i.push_back((y + 1) * ringVertexCount + x + 1);
			i.push_back(y * ringVertexCount + x + 1);
		}
	}

	BuildTopCap(v, i);
	BuildBottomCap(v, i);
	//vertex

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();

	copy(v.begin(), v.end(), stdext::checked_array_iterator<MeshVertex*>(vertices, vertexCount));

	//index

	this->indices = new UINT[i.size()];
	indexCount = i.size();

	copy(i.begin(), i.end(), stdext::checked_array_iterator<UINT*>(this->indices, indexCount));
}

void MeshCylinder::BuildTopCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	float y = 0.5f * height;
	float theta = 2.0f * Math::PI / (float)sliceCount;

	for (UINT i = 0; i <= sliceCount; i++)
	{
		float x = top_radius * cosf(i * theta);
		float z = top_radius * sinf(i * theta);

		float u = x / height + 0.5f;
		float v = z / height + 0.5f;

		vertices.push_back(MeshVertex(x, y, z, u, v, 0, 1, 0, 1, 0, 0));
	}
	vertices.push_back(MeshVertex(0, y, 0, 0.5f, 0.5f, 0, 1, 0, 1, 0, 0));
	//���� �������� �ε������ �̾��ٰ��̹Ƿ�: (vertices.size() -1) - sliceCount
	//���⿡ vertice���� �̹� ���� ������ �����Ƿ� �߰��� -1
	UINT baseIndex = vertices.size() - sliceCount - 2;
	
	//centerIndex : vertices index�� 0���� �����ϹǷ� ������ vertices.size() - 1
	UINT centerIndex = vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}

}

void MeshCylinder::BuildBottomCap(vector<MeshVertex>& vertices, vector<UINT>& indices)
{
	float y = -0.5f * height;
	float theta = 2.0f * Math::PI / (float)sliceCount;

	for (UINT i = 0; i <= sliceCount; i++)
	{
		float x = top_radius * cosf(i * theta);
		float z = top_radius * sinf(i * theta);

		float u = (x / height) + 0.5f; //+0.5: uv ��ǥ�� �����ش�
		float v = (z / height) + 0.5f;

		vertices.push_back(MeshVertex(x, y, z, u, v, 0, -1, 0, -1, 0, 0));
	}
	vertices.push_back(MeshVertex(0, y, 0, 0.5f, 0.5f, 0, -1, 0, -1, 0, 0));
	UINT baseIndex = vertices.size() - sliceCount - 2;
	UINT centerIndex = vertices.size() - 1;

	for (UINT i = 0; i < sliceCount; i++)
	{
		indices.push_back(centerIndex);
		indices.push_back(baseIndex + i + 1);
		indices.push_back(baseIndex + i);
	}
}
