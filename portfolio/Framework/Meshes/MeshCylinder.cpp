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
	float radiusStep = (top_radius - bottom_radius) / (float)stackCount; //반지름 차이를 스텝으로 나눔

	UINT ringCount = stackCount + 1;
	for (UINT i = 0; i < ringCount; i++)
	{
		float y = (-0.5f * height) + i * stackHeight; //원점에 위치하기 위해 : (-0.5f * height)
		float r = bottom_radius + i * radiusStep;
		float theta = 2.0f * Math::PI / (float)sliceCount;

		for (UINT k = 0; k <= sliceCount; k++)
		{
			float c = cosf(k * theta);
			float s = sinf(k * theta);

			MeshVertex vertex;

			vertex.Position = Vector3(r*c, y, r*s);
			vertex.Uv = Vector2((float)k / (float)sliceCount, 1.0f - (float)i / (float)stackCount);

			//윗면 아랫면의 너비가 다르므로, 노말도 각도가 기울어지게 된다 그래서 외적을 사용
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
	UINT ringVertexCount = sliceCount + 1; //grid만들때 처럼 카운트에 + 1
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
	//원통 맨윗줄의 인덱스들과 이어줄것이므로: (vertices.size() -1) - sliceCount
	//여기에 vertice에서 이미 윗판 중점이 들어갔으므로 추가로 -1
	UINT baseIndex = vertices.size() - sliceCount - 2;
	
	//centerIndex : vertices index가 0부터 시작하므로 끝점은 vertices.size() - 1
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

		float u = (x / height) + 0.5f; //+0.5: uv 좌표로 맞춰준다
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
