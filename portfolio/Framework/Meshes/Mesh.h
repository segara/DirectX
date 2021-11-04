#pragma once
class Mesh
{
public:
	typedef VertexTextureNormalTangent MeshVertex;
private:

	////Shader->AsMatrix 얻어오는부분이 느리기때문에 미리 받아두는 역할
	//ID3DX11EffectMatrixVariable* sWorld, *sView, *sProjection;
	ID3DX11EffectShaderResourceVariable* sDiffuseMap;
public:
	Mesh();
	virtual ~Mesh();

	void SetShader(Shader* shader);
	void Pass(UINT val) { pass = val; }
	void Update();
	void Render(UINT drawCount);
public :


protected:
	virtual void Create() = 0;

protected:
	Shader* shader;
	UINT pass = 0;

	PerFrame* perFrame = NULL;

	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	MeshVertex* vertices;
	UINT* indices;

	UINT vertexCount, indexCount;

};

