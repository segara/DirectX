#include "stdafx.h"
#include "ThreadDemo.h"

void ThreadDemo::Initialize()
{

	//�Լ������� ȣ��
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
	printf("�ݺ���1 ����\n");
	for (int i = 0; i < 100; i++)
	{
		printf("2 : %d\n", i);
	}
	printf("�ݺ���2 ����\n");
}

void ThreadDemo::Function()
{
	printf("�Լ������� ȣ��");
}

void ThreadDemo::MultiThread()
{
	//thread t(Mul)
	thread t(bind(&ThreadDemo::MultiThread1, this));
	thread t2(bind(&ThreadDemo::MultiThread2, this));

	t2.join();//������ ����(�����尡 ������� ȣ���)
	printf("t2.join\n");//���� join�� ����Ǿ� ����

	t.join(); //���� t2.join�� ����Ǿ� �����

	printf("t.join\n");

}

void ThreadDemo::MultiThread1()
{
	for (int i = 0; i < 100; i++)
	{
		printf("1 : %d\n", i);
	}
	printf("�ݺ���1 ����\n");
}

void ThreadDemo::MultiThread2()
{
	for (int i = 0; i < 100; i++)
	{
		printf("2 : %d\n", i);
	}
	printf("�ݺ���2 ����\n");
}

void ThreadDemo::Join()
{
	//���ٽ�
	//todo:���ٽ� ����
	thread t([]()
	{
		for (int i = 0; i < 100; i++)
		{
			printf("1 : %d\n", i);
		}
		printf("�ݺ���1 ����\n"); 
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

	printf("��Ƽ������ ����\n");
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
	t.detach(); //������ �������� �Ѱ��ְ� �����尡 �Ϸ�Ǹ� �ڵ����� join�� �߻�
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
	//4000000 ���� ���� ���´�
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
