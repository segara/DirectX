#pragma once
#include "Systems/IExecute.h"

class ThreadDemo : public IExecute
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
	void Loop();
	void Function();
	void MultiThread();
	void MultiThread1();
	void MultiThread2();

	void Join();

	void Mutex();
	void MutexUpdate();
	void RaceCondition(int& counter);
	void RaceConditionLock(int& counter);
	void Execute();
	void ExecuteTimer();
	void ExecutePerformance();

private:
	mutex m;
	float progress = 0.0f; //임계영역의 변수

	Timer timer;
	Timer timer2;
};
