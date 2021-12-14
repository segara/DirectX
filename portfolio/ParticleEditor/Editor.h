#pragma once
#include "Systems/IExecute.h"
#include "Environment/CubeSky.h"
class Editor : public IExecute
{
public:
	// IExecute을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}
private:
	void UpdateParticleList();
	void UpdateTextureList();

	void OnGUI();
	void OnGUI_List();
	void OnGUI_Settings();
	void OnGUI_Write();

	void WriteFile(wstring file);
private:
	void CreateMesh();
private:

	CubeSky* sky;
	Shader* shader;

	float windowWidth = 500;

	bool bLoop = false;
	UINT maxParticle = 0;

	vector<wstring> particleList;
	vector<wstring> textureList;

	wstring file = L""; //선택한 파일명

	ParticleSystem* particleSystem = NULL;

	MeshRender* grid;
	MeshRender* sphere;

	Material* floor;
	Material* stone;
};
