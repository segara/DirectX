#include "Framework.h"
#include "RenderTarget.h"

RenderTarget::RenderTarget(UINT width, UINT height, DXGI_FORMAT format)
	:format(format)
{
	//왼쪽위 부터의 너비와 높이

	this->width = (width < 1) ? (UINT)D3D::Width() : width;
	this->height = (height < 1) ? (UINT)D3D::Height() : height;

	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(D3D11_TEXTURE2D_DESC));
	textureDesc.Width = this->width;
	textureDesc.Height = this->height;
	textureDesc.ArraySize = 1;
	textureDesc.Format = format;
	//shader resource 와 render target bind를 명시 해주어야 함
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.MipLevels = 1;
	textureDesc.SampleDesc.Count = 1;
	//이미지중에 하나를 추출하는것을 SampleDesc 라고한다.
	Check(D3D::GetDevice()->CreateTexture2D(&textureDesc, NULL, &texture));

	//렌더타겟을 만들기 위한 기본적인 desc 설정
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	ZeroMemory(&rtvDesc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
	rtvDesc.Format = format;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Check(D3D::GetDevice()->CreateRenderTargetView(texture, &rtvDesc, &rtv));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MipLevels = 1;
	Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv));

}

RenderTarget::~RenderTarget()
{
	SafeRelease(texture);
	SafeRelease(rtv);
	SafeRelease(srv);
}

void RenderTarget::SaveTexture(wstring file)
{
	Check(D3DX11SaveTextureToFile(D3D::GetDC(), texture, D3DX11_IFF_PNG, file.c_str()));
}

void RenderTarget::PreRender(DepthStencil * depthStencil)
{
	//void OMSetRenderTargets(
	//  [in]           UINT                   NumViews,
	//	[in, optional] ID3D11RenderTargetView * const *ppRenderTargetViews,
	//	[in, optional] ID3D11DepthStencilView *pDepthStencilView
	//	);
	//Type: ID3D11RenderTargetView*
	//Pointer to an array of ID3D11RenderTargetView that represent the render targets 
	// to bind to the device.If this parameter is NULL and NumViews is 0, no render targets are bound.
	D3D::GetDC()->OMSetRenderTargets(1, &rtv, depthStencil->DSV());
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, depthStencil->DSV()); //rtv를 클리어
}

void RenderTarget::PreMultiRender(RenderTarget ** targets, UINT count, DepthStencil * depthStencil)
{
	vector<ID3D11RenderTargetView*> rtvs;
	for (UINT i = 0; i < count; i++)
	{
		ID3D11RenderTargetView* rtv = targets[i]->RTV();
		rtvs.push_back(rtv);

		D3D::GetDC()->ClearRenderTargetView(rtv, Color(0, 0, 0, 1));

	}
	D3D::GetDC()->ClearDepthStencilView(depthStencil->DSV(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	D3D::GetDC()->OMSetRenderTargets(rtvs.size(), &(rtvs[0]), depthStencil->DSV());
	//두번째 인수가 더블 포인터임
}
