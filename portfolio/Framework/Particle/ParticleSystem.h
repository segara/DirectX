#pragma once
#include "Renders/Renderer.h"

class ParticleSystem : public Renderer
{
public:
	ParticleSystem(wstring file);
	~ParticleSystem();
public :  
	void Update();
	void Render();
private:
	void MapVertices();
	void Activate();
	void Deactivate();
private:
	void ReadFile(wstring file);
public:
	ParticleData& GetData() { return data; }
	void SetTexture(wstring texture);
public:
	void Reset();
	void Add(Vector3& position);
private:
	struct VertexParticle
	{
		Vector3 Position;
		///Corner : ������, UV ����� ���
		Vector2 Corner;
		Vector3 Velocity;
		
		///Random
		///x:�ֱ�
		///y:ũ��
		///z:ȸ��
		///w:����
		Vector4 Random;
		float Time;
	};

private :
	struct Desc
	{
		Color MinColor;
		Color MaxColor;

		Vector3 Gravity;
		float EndVelocity;

		Vector2 StartSize;
		Vector2 EndSize;

		Vector2 RotateSpeed;
		float ReadyTime;
		float ReadyRandomTime;

		float ColorAmount;
		float CurrentTime;
		float Padding[2];
	} desc;
	
private :
	ParticleData data;
	Texture* map = NULL;

	ID3DX11EffectShaderResourceVariable* sMap;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;

	VertexParticle* vertices = NULL;
	UINT* indices = NULL; 

	float currentTime = 0.0f;
	///lasdtAddTime : ���������� �߰��Ƚð�
	float lastAddTime = 0.0f;

	UINT leadCount = 0;
	UINT gpuCount = 0;
	UINT activeCount = 0;
	UINT deactiveCount = 0;
	 
};

