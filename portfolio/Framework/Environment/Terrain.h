#pragma once
class Terrain
{
public:
	typedef VertexNormal TerrainVertex;

private:
	UINT pass = 0;
	Shader* shader;
	Texture* heightMap;

	UINT width, height;

	UINT vertexCount;
	TerrainVertex* vertices;
	ID3D11Buffer* vertexBuffer;

	UINT indexCount;
	UINT* indices;
	ID3D11Buffer* indexBuffer;
private:
	void CreateVertexData();
	void CreateIndexData();
	void CreateNormalData();

public:
	Terrain(Shader* shader, wstring heightFile);
	~Terrain();

	void Update();
	void Render();

	void Pass(UINT val) { pass = val; }
	float GetHeight(Vector3& position);
	float GetVerticalRaycast(Vector3& position);
	Vector3 GetRaycastPosition();

	Matrix world;
};

