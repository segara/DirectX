#include "stdafx.h"
#include "ThreadDemo.h"

void ThreadDemo::Initialize()
{

	//함수포인터 호출
	//function<void()> f = bind(&ThreadDemo::Function, this);
	//f();
	//MultiThread();
	//Join();


	//Mutex();

	//ExecuteTimer();
	ExecutePerformance();
}


void ThreadDemo::Destroy()
{
	
}

void ThreadDemo::Update()
{
	//MutexUpdate();
}

void ThreadDemo::Render()
{

}

void ThreadDemo::Loop()
{
	for (int i = 0; i < 100; i++)
	{
		printf("1 : %d\n",i);
	}
	printf("반복문1 종료\n");
	for (int i = 0; i < 100; i++)
	{
		printf("2 : %d\n", i);
	}
	printf("반복문2 종료\n");
}

void ThreadDemo::Function()
{
	printf("함수포인터 호출");
}

void ThreadDemo::MultiThread()
{
	//thread t(Mul)
	thread t(bind(&ThreadDemo::MultiThread1, this));
	thread t2(bind(&ThreadDemo::MultiThread2, this));

	t2.join();//쓰레드 종료(쓰레드가 종료된후 호출됨)
	printf("t2.join\n");//위의 join이 실행되야 실행

	t.join(); //위의 t2.join이 실행되야 실행됨

	printf("t.join\n");

}

void ThreadDemo::MultiThread1()
{
	for (int i = 0; i < 100; i++)
	{
		printf("1 : %d\n", i);
	}
	printf("반복문1 종료\n");
}

void ThreadDemo::MultiThread2()
{
	for (int i = 0; i < 100; i++)
	{
		printf("2 : %d\n", i);
	}
	printf("반복문2 종료\n");
}

void ThreadDemo::Join()
{
	//람다식
	//todo:람다식 정리
	thread t([]()
	{
		for (int i = 0; i < 100; i++)
		{
			printf("1 : %d\n", i);
		}
		printf("반복문1 종료\n"); 
	});

	thread t2([]() {
		int a = 0;
		while (true) {
			a++;
			printf("A : %d",a);
			Sleep(100);
			if (a > 30)
				break;
		}
	});

	printf("멀티쓰레드 시작\n");
	t2.join();
	printf("t2 join\n");
	t.join();
	printf("t.join\n");
}

void ThreadDemo::Mutex()
{
	thread t([&]() {
		while (true)
		{
			Sleep(1000);
			printf("Progress : %f\n", progress);
		}
	});
	t.detach(); //대기없이 다음으로 넘겨주고 스레드가 완료되면 자동으로 join을 발생
}

void ThreadDemo::MutexUpdate()
{
	progress += 0.1f;
	ImGui::ProgressBar(progress / 1000.0f);
}

void ThreadDemo::RaceCondition(int & counter)
{
	for (int i = 0; i < 1000000; i++)
		counter++;
}

void ThreadDemo::RaceConditionLock(int & counter)
{
	for (int i = 0; i < 1000000; i++) {
		m.lock();
		{
			counter++;
		}
		
		m.unlock();
	}
	
}

void ThreadDemo::Execute()
{
	int counter = 0;

	vector<thread> threads;
	for (int i = 0; i < 4; i++)
	{
		function<void(int&)> f = bind(&ThreadDemo::RaceConditionLock, this, placeholders::_1);

		threads.push_back(thread(f, ref(counter)));
	}

	for (int i = 0; i < 4; ++i)
	{
		threads[i].join();
	}
	printf("Counter : %d\n", counter);
	//4000000 보다 적게 나온다
}

void ThreadDemo::ExecuteTimer()
{
	timer.Start([]() {
		printf("Timer\n");
	},2000,2);

	timer2.Start([]() {
		printf("Timer2\n");
	}, 3000);
}

void ThreadDemo::ExecutePerformance()
{
	int arr[10000];
	for (int i = 0; i < 10000; i++)
		arr[i] = Math::Random(0, 100000);

	Performance p;
	p.Start();
	{
		sort(arr, arr + 10000);
	}
	float last = p.End();

	printf("%f\n", last);
}
