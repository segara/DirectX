#pragma once
class CubeMap
{
public:
	CubeMap(Shader* shader);
	~CubeMap();
	void Update();
	void Render();
private:
	UINT pass;
	Shader* shader;
	MeshSphere* mesh;
public:
	void Pass(UINT val) { pass = val; }

	void Texture(wstring file);

	Transform* GetTransform() { return mesh->GetTransform(); }
private:
	ID3D11ShaderResourceView* srv = NULL; //실제 파일에서 로드
	ID3DX11EffectShaderResourceVariable* sSrv; //shader로 연결해주는 역할
};

