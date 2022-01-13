#pragma once
class ModelAnimator
{
public:
	ModelAnimator();
	ModelAnimator(Shader* shader);
	~ModelAnimator();

	void Update();
	void Render();
protected:
	void UpdateTweenMode(UINT instanceIndex);
	void UpdateBlendMode(UINT instanceIndex);
public :
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	void ReadClip(wstring file);

	
	Model* GetModel() { return model; }

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();
	UINT GetTransformCount() { return transforms.size(); }

	void Pass(UINT pass);
	void PlayTweenMode(UINT instanceId, UINT clip, float speed = 1.0f, float takeTime = 1.0f); //takeTime: 동작 전환시 소요시간
	void PlayBlendMode(UINT instanceId, UINT clip1, UINT clip2, UINT clip3);
	void SetBlendAlpha(UINT instanceId, float alpha);
	//void SetAttachTransform(UINT boneIndex);
	void GetAttachTransform(UINT instance, Matrix* outResult);
public :
	void CreateTexture();
	void CreateClipTransform(UINT index);


protected:
	struct ClipTransform
	{
		Matrix** Transform;

		ClipTransform()
		{
			Transform = new Matrix*[MAX_MODEL_KEYFRAMES];

			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				Transform[i] = new Matrix[MAX_MODEL_KEYFRAMES];

		}

		~ClipTransform()
		{
		
			for (UINT i = 0; i < MAX_MODEL_KEYFRAMES; i++)
				SafeDeleteArray(Transform[i]);

			SafeDeleteArray(Transform);

		}
	};
	ClipTransform* clipTransforms = NULL;

	ID3D11Texture2D* texture = NULL;
	ID3D11ShaderResourceView* srv = NULL;
protected:
	struct KeyframeDesc
	{
		int Clip = 0;

		UINT CurrFrame = 0;
		UINT NextFrame = 0;

		float Time = 0.0f;
		float RunningTime = 0.0f;

		float Speed = 1.0f;

		Vector2 Padding;
	};// keyframeDesc;

	ConstantBuffer* tweenBuffer;
	ID3DX11EffectConstantBuffer* sTweenBuffer; 

	struct TweenDesc
	{
		float TakeTime = 1.0f; //tween이 진행되는 시간
		float TweenTime = 0.0f; //1이 완료
		float ChangeTime = 0.0f; //경과시간 저장용 
		float Padding;

		KeyframeDesc Curr;
		KeyframeDesc Next;

		TweenDesc()
		{
			Curr.Clip = 0;
			Next.Clip = -1;  //-1이면 다음클립이 없는것
		}

	}tweenDesc[MAX_MODEL_INSTANCE];

protected:
	struct BlendDesc
	{
		UINT Mode = 0; //1이면 블렌드 모드
		float Alpha = 0; 
		Vector2 Padding;
		KeyframeDesc Clip[3]; //3가지 동작을 블렌드
	}blendDesc[MAX_MODEL_INSTANCE];

	ConstantBuffer* blendBuffer;
	ID3DX11EffectConstantBuffer* sBlendBuffer;
protected:
	Shader* shader;
	Model* model;
	
	vector<Transform *> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];

	VertexBuffer* instanceBuffer;
protected:
	Color colors[MAX_MODEL_INSTANCE];
	VertexBuffer* instanceBuffer_Color;
protected:
		struct CS_InputDesc
		{
			Matrix Bone;
		};
		struct CS_OutputDesc
		{
			Matrix Result;
		};
		
protected:
	const float frameRate = 30;
	float frameTime = 0.0f;

	Shader* computeShader;
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;

	StructuredBuffer* inputWorldBuffer;
	ID3DX11EffectShaderResourceVariable* sInputWorldSRV;

	StructuredBuffer* inputBoneBuffer;
	ID3DX11EffectShaderResourceVariable* sInputBoneSRV;

	TextureBuffer* outputBuffer;
	ID3DX11EffectUnorderedAccessViewVariable* sOutputUAV;


	ID3DX11EffectConstantBuffer* sComputeTweenBuffer;
	ID3DX11EffectConstantBuffer* sComputeBlendBuffer;

	
};

