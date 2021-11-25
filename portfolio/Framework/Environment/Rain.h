#pragma once
class Rain :Renderer
{
public:
	Rain(Vector3& extent, UINT count, wstring file);
	~Rain();
	void Update();
	void Render();
private:
	struct VertexRain
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Scale;
	};
private:
	struct Desc
	{
		D3DXCOLOR Color = D3DXCOLOR(1, 1, 1, 1);
		
		Vector3 Velocity = Vector3(-10, -100, 0);
		float DrawDistance = 0;

		Vector3 Origin = Vector3(0, 0, 0);
		float Padding = 0;

		Vector3 Extent = Vector3(0, 0, 0);
		float Padding2;
	}desc;
private:
	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexRain* vertices;
	UINT* indices;

	Texture* texture;
	UINT drawCount = 100;
};

