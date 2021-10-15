#include "stdafx.h"
#include "UserInterfaceDemo.h"

void UserInterfaceDemo::Initialize()
{
	shader = new Shader(L"06_Rectangle.fx");
	
	{
		vertices[0].Position = Vector3(0.0f, +0.0f, 0.0f);
		vertices[1].Position = Vector3(+0.0f, 0.5f, 0.0f);
		vertices[2].Position = Vector3(+0.5f, +0.0f, 0.0f);

		vertices[3].Position = Vector3(0.5f, 0.0f, 0.0f);
		vertices[4].Position = Vector3(0.0f, 0.5f, 0.0f);
		vertices[5].Position = Vector3(0.5f, 0.5f, 0.0f);

		D3D11_BUFFER_DESC desc;

		ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
		desc.ByteWidth = sizeof(Vertex) * 6;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

		D3D11_SUBRESOURCE_DATA subResource = { 0 };
		subResource.pSysMem = vertices;

		Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &vertexBuffer));
	}

	

}


void UserInterfaceDemo::Destroy()
{
	SafeDelete(shader);
	SafeRelease(vertexBuffer);
}

void UserInterfaceDemo::Update()
{
	if (Keyboard::Get()->Press(VK_RIGHT)) {
		vertices[0].Position.x += 10.0f * Time::Delta();
	}
	if (Keyboard::Get()->Press(VK_LEFT)) {
		vertices[0].Position.x -= 10.0f * Time::Delta();
	}

	static float y = 0.5f;
	ImGui::SliderFloat("Y", &y, -1, 1);
	vertices[1].Position.y = y;

	D3D::GetDC()->UpdateSubresource(vertexBuffer, 0, NULL, vertices, sizeof(Vertex) * 6, 0);

}

void UserInterfaceDemo::Render()
{
	static bool bOpen = true;
	//ImGui::Checkbox("Show Demo Window", &bOpen);
	/*if (ImGui::Button("Open"))
		bOpen = !bOpen;*/
	if (Keyboard::Get()->Down(VK_SPACE))
		bOpen = !bOpen;
	if(bOpen)
		ImGui::ShowDemoWindow(&bOpen);

	string debug_render = "";
	
	if (Keyboard::Get()->Press(VK_RIGHT)) {
		debug_render += "right";
	}
	if (Keyboard::Get()->Press(VK_LEFT)) {
		debug_render += "left";
	}

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	//IA : �������� �ʿ��� ������ ����(���ؽ� ����, �׸��� ��� ����)
	//VS : ���ؽ� ���̴�
	//RS : �����Ͷ���¡
	//PS : �ȼ� ���̴�

	//Draw ù��° ���� : ���̴��� T0, T1,,
	//Draw �ι�° ���� : ���̴��� P0, P1,,
	//Draw ����° ���� : ���� ����
	//Draw ����° ���� : ���� �ε���

	Gui::Get()->RenderText(10, 60, 1, 0, 0, debug_render);

	D3D::GetDC()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	
	shader->Draw(0, 1, 6);


}