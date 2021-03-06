#pragma once
class Renderer
{
public :
	Renderer(Shader* shader);
	Renderer(wstring shaderFile);
	virtual ~Renderer();
public:
	Shader* GetShader() { return shader; }

	UINT& Pass() { return pass; }
	void Pass(UINT val) { pass = val; }

	virtual void Update();
	virtual void Render();

	Transform* GetTransform() { return transform; }

protected:
	void Topology(D3D11_PRIMITIVE_TOPOLOGY val) { topology = val; }
private:
	void Initialize();
protected:
	Shader* shader;

	Transform* transform;
	VertexBuffer* vertexBuffer = NULL;
	IndexBuffer* indexBuffer = NULL;

	UINT vertexCount = 0;
	UINT indexCount = 0;
private:
	bool bCreateShader = false; //내부생성 /외부생성 쉐이더 사용

	D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	UINT pass = 0;

	PerFrame* perFrame;
};

