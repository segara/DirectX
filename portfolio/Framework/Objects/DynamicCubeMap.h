#pragma once
class DynamicCubeMap
{
public:
	DynamicCubeMap(Shader* shader, UINT width, UINT height);
	~DynamicCubeMap();

	void PreRender(Vector3& position, Vector3& scale, float zNear = 0.1f, float zFar = 500.0f, float fov = 0.5f);
	//void Type(UINT val) { desc_const_buffer_dynamicCube.Type = val; }
	UINT& Type(){ return desc_const_buffer_dynamicCube.Type; }
	ID3D11ShaderResourceView* SRV() { return srv; }
	Perspective* GetPerspective() { return perspective; }
private:
	struct Desc
	{
		UINT Type = 0;
		float Padding[3];
		Matrix Views[6];
		Matrix Projection;
	}desc_const_buffer_dynamicCube;
private:
	Shader* shader;
	Vector3 position;

	UINT width, height;

	ID3D11Texture2D* rtvTexture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv;

	//depth stencil view
	ID3D11Texture2D* dsvTexture;
	ID3D11DepthStencilView* dsv;

	Perspective* perspective;
	Viewport* viewport;

	class ConstantBuffer* buffer;
	ID3DX11EffectConstantBuffer* sBuffer = NULL;
};

