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
	ID3D11ShaderResourceView* srv = NULL; //���� ���Ͽ��� �ε�
	ID3DX11EffectShaderResourceVariable* sSrv; //shader�� �������ִ� ����
};

