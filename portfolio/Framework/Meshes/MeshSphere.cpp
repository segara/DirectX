#include "Framework.h"
#include "MeshSphere.h"



MeshSphere::MeshSphere(float radius, UINT stackCount, UINT sliceCount)
	:radius(radius), stackCount(stackCount), sliceCount(sliceCount) {
}

MeshSphere::~MeshSphere()
{
}

void MeshSphere::Create()
{
	vector<MeshVertex> v;
	v.push_back(MeshVertex(0, radius, 0, 0, 0, 0, 1, 0, 1, 0, 0));

	float phiStep = Math::PI / stackCount; //y
	float thetaStep = Math::PI * 2.0f / sliceCount; //x

	for(UINT i = 1; i <= stackCount - 1; i++) //ù���� �̹� ���ǵ������Ƿ� -1
	{
		float phi = i * phiStep;
		for (UINT k = 0; k <= sliceCount; k++)
		{
			float theta = k * thetaStep;
			Vector3 p = Vector3
			(
				(radius * sinf(phi)) * cosf(theta), //(radius * sinf(phi)) �̺κ��� ���� �� ������
				radius * cosf(phi), // y�� 1 ����  -1 ����
				(radius * sinf(phi)) * sinf(theta) //z���� 0���� �������� �����Ƿ� sinf(theta) 
			);

			Vector3 n;

			D3DXVec3Normalize(&n, &p); //�������� ���� ��ǥ ��ü�� �������� �ᵵ �ǹǷ� 
			
			Vector3 tangent = Vector3
			(
				-(radius * sinf(phi)) * sinf(theta), 
				0.0f,
				(radius * sinf(phi)) * cosf(theta) 
			);
			D3DXVec3Normalize(&tangent, &tangent); 

			Vector2 uv = Vector2(theta / (Math::PI * 2), phi / Math::PI);

			v.push_back(MeshVertex(p.x, p.y, p.z, uv.x, uv.y, n.x, n.y, n.z, tangent.x, tangent.y, tangent.z));
		}
	}
	v.push_back(MeshVertex(0, -radius, 0, 0, 0, 0, -1, 0,-1,0,0)); //�ǾƷ� ������

	vertices = new MeshVertex[v.size()];
	vertexCount = v.size();

	copy(v.begin(), v.end(), stdext::checked_array_iterator<MeshVertex*>(vertices, vertexCount));

	//index

	vector<UINT> indices;
	for (UINT i = 1; i <= sliceCount; i++)
	{
		indices.push_back(0);
		indices.push_back(i + 1);
		indices.push_back(i);
	}

	UINT baseIndex = 1;
	UINT ringVertCount = sliceCount + 1;
	for (UINT i = 0; i < stackCount - 2; i++)
	{
		for (UINT j = 0; j < sliceCount; j++)
		{
			indices.push_back(baseIndex + i * ringVertCount + j);
			indices.push_back(baseIndex + i * ringVertCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertCount + j);

			indices.push_back(baseIndex + (i + 1) * ringVertCount + j);
			indices.push_back(baseIndex + i * ringVertCount + j + 1);
			indices.push_back(baseIndex + (i + 1) * ringVertCount + j + 1);
		}
	}

	UINT southPoleIndex = v.size() - 1;
	baseIndex = southPoleIndex - ringVertCount;

	for (UINT i = 0; i < sliceCount; i++)
	{
		indices.push_back(southPoleIndex);
		indices.push_back(baseIndex + i);
		indices.push_back(baseIndex + i + 1);
	}

	this->indices = new UINT[indices.size()];
	indexCount = indices.size();

	copy(indices.begin(), indices.end(), stdext::checked_array_iterator<UINT*>(this->indices, indexCount));
}
