#pragma once
#define MAX_BILLBOARD_COUNT 10000
//4개 정점에 2000개
class Billboard : public Renderer
{
public:
	struct VertexBillboard
	{
		Vector3 Position;
		Vector2 Uv;
		Vector2 Scale;
	};
private:
	VertexBillboard* vertices;
	UINT* indices;
	UINT drawCount = 0;
	UINT preDrawCount = 0;

	Texture* texture;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
public:
	Billboard(wstring file);
	~Billboard();
	void Update();
	void Render();

	void Add(Vector3& position, Vector2& scale);
};

