#include "stdafx.h"
#include "RawBufferDemo.h"

void RawBufferDemo::Initialize()
{
	Shader* shader = new Shader(L"60_RawBuffer.fx");

	UINT count = 2 * (10 * 8 * 3); //thread group 안에서 운용될 thread 갯수, x :10 y:8 z:3

	struct Output
	{
		UINT GroupID[3];
		UINT GroupThreadID[3];
		UINT DispatchThreadID[3];
		UINT GroupIndex;
		float ReturnValue;
	};

	struct Input
	{
		float Value = 0.0f;
	};
	Input* input = new Input[count];

	for (int i = 0; i < count; i++)
	{
		input[i].Value = Math::Random(0.0f, 1000.0f);
	}

	RawBuffer* rawBuffer = new RawBuffer(input, sizeof(Input)* count, sizeof(Output)* count);
	shader->AsSRV("Input")->SetResource(rawBuffer->SRV());
	//RawBuffer* rawBuffer = new RawBuffer(NULL, 0, sizeof(Output)* count);
	//RawBuffer 생성시 create input, create srv, create output, create uav, create result까지 실행
	shader->AsUAV("Output")->SetUnorderedAccessView(rawBuffer->UAV());
	shader->Dispatch(0, 0, 2, 1, 1);// compute shader의 실행 2,1,1: x,y,z group의 갯수

	Output* output = new Output[count];

	rawBuffer->CopyFromOuput(output);

	FILE* file;
	fopen_s(&file, "../Raw.csv", "w");

	for (UINT i = 0; i < count; i++)
	{
		Output temp = output[i];
		fprintf
		(
			file,
			"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%f\n",
			i,
			temp.GroupID[0], temp.GroupID[1], temp.GroupID[2],
			temp.GroupThreadID[0], temp.GroupThreadID[1], temp.GroupThreadID[2],
			temp.DispatchThreadID[0], temp.DispatchThreadID[1], temp.DispatchThreadID[2],
			temp.GroupIndex,temp.ReturnValue 
		);
		
	}
	fclose(file);
}


void RawBufferDemo::Destroy()
{
	
}

void RawBufferDemo::Update()
{

}

void RawBufferDemo::Render()
{

}
