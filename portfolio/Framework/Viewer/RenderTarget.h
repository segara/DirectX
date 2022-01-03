#pragma once
class RenderTarget
{
public:
	RenderTarget(UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	ID3D11RenderTargetView* RTV() { return rtv; }
	ID3D11ShaderResourceView* SRV() { return srv; }

	void SaveTexture(wstring file);

	//RenderTargetView 텍스쳐와 DepthStencilView의 텍스쳐 크기가 다르면 렌더링 되지 않는다.
	void PreRender(class DepthStencil* depthStencil);
	//더블포인터 : 이차원배열로 사용하거나 (vector <RenderTarget*>) 로 사용하기 위해
	static void PreMultiRender(RenderTarget** targets, UINT count, class DepthStencil* depthStencil);
private:
	UINT width, height; //텍스쳐 크기
	DXGI_FORMAT format; //텍스쳐 포맷 

	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv; 
};

