#pragma once
class Shadow
{
public:
	Shadow(Shader* shader, Vector3& position, float radius, UINT width = 1024, UINT height = 1024);
	~Shadow();

	void PreRender();

	ID3D11ShaderResourceView* SRV() { return renderTarget->SRV(); }

private:
	void CalcViewProjection();
private:
	struct Desc
	{
		Matrix View; //라이트 방향
		Matrix Projection; //라이트의 projection

		Vector2 MapSize;
		float Bias = -0.0006f;

		UINT Quality = 0; //퀄리티구분
	} desc;

private:
	Shader* shader;
	UINT width, height;

	Vector3 position; //조명이 어느 부분이 비추는지 position
	float radius;

	RenderTarget* renderTarget;

	DepthStencil* depthStencil;
	Viewport* viewport;

	ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer;
	ID3DX11EffectShaderResourceVariable* sShadowMap;

	ID3D11SamplerState* pcfSampler;
	ID3DX11EffectSamplerVariable* sPcfSampler;


};

