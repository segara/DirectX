#include "Framework.h"
#include "Billboard.h"

Billboard::Billboard(wstring file) : Renderer(L"83_BillBoard.fx")
{
	vertexCount = MAX_BILLBOARD_COUNT * 4;
	indexCount = MAX_BILLBOARD_COUNT * 6;
	vertices = new VertexBillboard[vertexCount];

	//cpu write : true -> 계속 빌보드가 늘어날수있기 때문에
	vertexBuffer = new VertexBuffer(vertices, vertexCount, sizeof(VertexBillboard), 0, true);

	indices = new UINT[indexCount];

	for (UINT i = 0; i < MAX_BILLBOARD_COUNT; i++)
	{
		indices[i * 6 + 0] = i * 4 + 0;
		indices[i * 6 + 1] = i * 4 + 1;
		indices[i * 6 + 2] = i * 4 + 2;
		indices[i * 6 + 3] = i * 4 + 2;
		indices[i * 6 + 4] = i * 4 + 1;
		indices[i * 6 + 5] = i * 4 + 3;
	}

	indexBuffer = new IndexBuffer(indices, indexCount);

	texture = new Texture(file);
	sDiffuseMap = shader->AsSRV("DiffuseMap");
}

Billboard::~Billboard()
{
	SafeDeleteArray(vertices);
	SafeDeleteArray(indices);
	SafeDelete(texture);
}

void Billboard::Update()
{
	Super::Update();
}

void Billboard::Render()
{
	if (drawCount != preDrawCount)
	{
		preDrawCount = drawCount;
		
		D3D11_MAPPED_SUBRESOURCE subResource;
		D3D::GetDC()->Map(vertexBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
		{
			memcpy(subResource.pData, vertices, sizeof(VertexBillboard)*vertexCount);
		}
		D3D::GetDC()->Unmap(vertexBuffer->Buffer(), 0);
	}

	Super::Render();

	sDiffuseMap->SetResource(texture->SRV());
	shader->DrawIndexed(0, Pass(), drawCount * 6);
}

void Billboard::Add(Vector3 & position, Vector2 & scale)
{
	vertices[drawCount * 4 + 0].Position = position;
	vertices[drawCount * 4 + 1].Position = position;
	vertices[drawCount * 4 + 2].Position = position;
	vertices[drawCount * 4 + 3].Position = position;

	vertices[drawCount * 4 + 0].Uv = Vector2(0, 1);//좌하단
	vertices[drawCount * 4 + 1].Uv = Vector2(0, 0);//좌상단
	vertices[drawCount * 4 + 2].Uv = Vector2(1, 1);//우하단
	vertices[drawCount * 4 + 3].Uv = Vector2(1, 0);//우상단

	vertices[drawCount * 4 + 0].Scale = scale;
	vertices[drawCount * 4 + 1].Scale = scale;
	vertices[drawCount * 4 + 2].Scale = scale;
	vertices[drawCount * 4 + 3].Scale = scale;
	drawCount++;

	
}
