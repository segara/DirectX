#pragma once
#define MAX_BILLBOARD_COUNT 10000
//4개 정점에 2000개
class Billboard : public Renderer
{
public:
	struct VertexBillboard
	{
		Vector3 Position;
		//Vector2 Uv;
		Vector2 Scale;
		UINT MapIndex;
	};
private:
	//VertexBillboard* vertices;
	//UINT* indices;
	
	vector<VertexBillboard> vertices;
	vector<wstring> textureNames;
	//UINT drawCount = 0;
	//UINT preDrawCount = 0;

	TextureArray* textureArray = NULL;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
public:
	Billboard(Shader* shader);
	~Billboard();
	void Update();
	void Render();

	//void SetTexture(wstring file);
	void AddTexture(wstring file);
	/// mapIndex : texture array의 인덱스
	void Add(Vector3& position, Vector2& scale, UINT mapIndex);
};

