#pragma once
#include "Systems/IExecute.h"

class UserInterfaceDemo : public IExecute
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
	Shader* shader;

	Vertex vertices[6];
	//Vertex vertices2[6];
	ID3D11Buffer* vertexBuffer;
	//ID3D11Buffer* vertexBuffer2;
};
