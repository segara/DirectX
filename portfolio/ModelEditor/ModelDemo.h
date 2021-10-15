#pragma once
#include "Systems/IExecute.h"
#include "Framework.h"
#include "../Framework/Environment/CubeSky.h"
class ModelDemo : public IExecute
{
public:
	// IExecute��(��) ���� ��ӵ�
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override;
	virtual void Update() override;
	virtual void PreRender() override {}
	virtual void Render() override;
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}
private:
	void Tank();
	void Kachujin();
	Shader* shader;
	ModelRender* tank;
	ModelRender* kachujin;
	
};
