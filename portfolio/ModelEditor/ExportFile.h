#pragma once
#include "Systems/IExecute.h"

class ExportFile : public IExecute
{
public:
	// IExecute을(를) 통해 상속됨
	virtual void Initialize() override;
	virtual void Ready() override {}
	virtual void Destroy() override {}
	virtual void Update() override{}
	virtual void PreRender() override {}
	virtual void Render() override{}
	virtual void PostRender() override {}
	virtual void ResizeScreen() override {}
private:
	void Tank();
	void Kachujin();
	void Weapon();
	void CharacterDummy();
};
