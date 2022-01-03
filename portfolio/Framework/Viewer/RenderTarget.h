#pragma once
class RenderTarget
{
public:
	RenderTarget(UINT width = 0, UINT height = 0, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM);
	~RenderTarget();

	ID3D11RenderTargetView* RTV() { return rtv; }
	ID3D11ShaderResourceView* SRV() { return srv; }

	void SaveTexture(wstring file);

	//RenderTargetView �ؽ��Ŀ� DepthStencilView�� �ؽ��� ũ�Ⱑ �ٸ��� ������ ���� �ʴ´�.
	void PreRender(class DepthStencil* depthStencil);
	//���������� : �������迭�� ����ϰų� (vector <RenderTarget*>) �� ����ϱ� ����
	static void PreMultiRender(RenderTarget** targets, UINT count, class DepthStencil* depthStencil);
private:
	UINT width, height; //�ؽ��� ũ��
	DXGI_FORMAT format; //�ؽ��� ���� 

	ID3D11Texture2D* texture;
	ID3D11RenderTargetView* rtv;
	ID3D11ShaderResourceView* srv; 
};

