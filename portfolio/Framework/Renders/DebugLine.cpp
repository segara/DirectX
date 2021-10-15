#include "Framework.h"
#include "DebugLine.h"


DebugLine* DebugLine::instance = NULL;

void DebugLine::Update()
{
	Matrix world;
	D3DXMatrixIdentity(&world);

	shader->AsMatrix("World")->SetMatrix(world);
	shader->AsMatrix("View")->SetMatrix(Context::Get()->View());
	shader->AsMatrix("Projection")->SetMatrix(Context::Get()->Projection());
}

void DebugLine::Render()
{
	if (drawCount <= 0)
		return;
	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(VertexColor)* drawCount,0);
	UINT stride = sizeof(VertexColor);
	UINT offset = 0;

	//IA : 렌더링에 필요한 정보를 셋팅(버텍스 버퍼, 그리는 방법 셋팅)
	//VS : 버텍스 세이더
	//RS : 레스터라이징
	//PS : 픽셀 세이더

	//Draw 첫번째 인자 : 세이더의 T0, T1,,
	//Draw 두번째 인자 : 세이더의 P0, P1,,
	//Draw 세번째 인자 : 정점 개수
	//Draw 세번째 인자 : 시작 인덱스

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);


	//shader->AsMatrix("World")->SetMatrix(world_Transform);
	shader->Draw(0, 0, drawCount);

	drawCount = 0;
	ZeroMemory(vertices, sizeof(VertexColor)* MAX_DEBUG_LINE);
}

DebugLine::DebugLine()
{
	shader = new Shader(L"14_DebugLine.fx");
	vertices = new VertexColor[MAX_DEBUG_LINE];
	ZeroMemory(vertices, sizeof(VertexColor)* MAX_DEBUG_LINE);

	//create vertex buffer
	{
		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(VertexColor) * MAX_DEBUG_LINE;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer)); //Check : Assert 와 같은 사용법
	}
	//SafeDeleteArray(vertices);//복사 완료 후 delete

	D3DXMatrixIdentity(&world);
}


DebugLine::~DebugLine()
{
	SafeRelease(vertexBuffer);
	SafeDelete(shader);
}

void DebugLine::Create()
{
	assert(instance == NULL);

	instance = new DebugLine();
}

void DebugLine::Delete()
{
	SafeDelete(instance);
}

DebugLine* DebugLine::Get()
{
	return instance;
}
void DebugLine::RenderLine(Vector3 & start, Vector3 & end)
{
	//References as function parameters
	//	참조형은 함수 매개 변수로 가장 많이 사용된다.이때 매개 변수는 인수의 별칭으로 사용되며, 복사본이 만들어지지 않는다.이렇게 하면 복사하는데 비용이 많이 드는 겨우 성능이 향상될 수 있다.

	RenderLine(start, end, Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end, float r, float g, float b)
{
	RenderLine(start, end, Color(r, g, b, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), Color(0, 1, 0, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2, float r, float g, float b)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), Color(r, g, b, 1));
}

void DebugLine::RenderLine(float x, float y, float z, float x2, float y2, float z2, Color & color)
{
	RenderLine(Vector3(x, y, z), Vector3(x2, y2, z2), color);
}

void DebugLine::RenderLine(Vector3 & start, Vector3 & end, Color & color)
{
	vertices[drawCount].Color = color;
	vertices[drawCount++].Position = start;

	vertices[drawCount].Color = color;
	vertices[drawCount++].Position = end;
}