#include "Framework.h"
#include "DynamicCubeMap.h"



DynamicCubeMap::DynamicCubeMap(Shader* shader, UINT width, UINT height)
	:shader(shader),position(0,0,0),width(width),height(height)
{
	//기존까지는 렌터타겟 클래스 객체를 여러개 생성해서 각각의 렌더타겟을 렌더링했다면
	//큐브맵에서는 한번에 6개를 생성
	DXGI_FORMAT rtvFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
	//Create Texture2D - RTV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 6; //cube이므로 6장
		desc.Format = rtvFormat;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE; //TEXTURECUBE로 셋팅
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &rtvTexture));
	}
	//Create RTV
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RENDER_TARGET_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
		//6장 사용하므로 array 설정
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateRenderTargetView(rtvTexture, &desc, &rtv));

	}
	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = rtvFormat;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;//cube map
		desc.TextureCube.MipLevels = 1;

		Check(D3D::GetDevice()->CreateShaderResourceView(rtvTexture, &desc, &srv));
	}


	DXGI_FORMAT dsvFormat = DXGI_FORMAT_D32_FLOAT;
	//Create Texture for DSV
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = width;
		desc.Height = height;
		desc.ArraySize = 6;
		desc.Format = dsvFormat;
		desc.BindFlags = D3D11_BIND_DEPTH_STENCIL; //dsv로 사용하기 위해
		desc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &dsvTexture));
	}

	//CreateDSV
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_DEPTH_STENCIL_VIEW_DESC));
		desc.Format = dsvFormat;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.ArraySize = 6;

		Check(D3D::GetDevice()->CreateDepthStencilView(dsvTexture, &desc, &dsv));
	}

	viewport = new Viewport((float)width, (float)height);

	buffer = new ConstantBuffer(&desc_const_buffer_dynamicCube, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_DynamicCube");
}

DynamicCubeMap::~DynamicCubeMap()
{
}

void DynamicCubeMap::PreRender(Vector3 & position, Vector3 & scale, float zNear, float zFar, float fov)
{
	this->position = position;

	//Create Views
	{
		float x = position.x;
		float y = position.y;
		float z = position.z;

		struct LookAt
		{
			Vector3 LookAt;
			Vector3 Up;
		} lookAt[6];

		lookAt[0] = { Vector3(x + scale.x, y, z), Vector3(0, 1, 0) }; //왼쪽
		lookAt[1] = { Vector3(x - scale.x, y, z), Vector3(0, 1, 0) }; //오른쪽
		lookAt[2] = { Vector3(x, y + scale.y, z), Vector3(0, 0, -1) }; //위쪽
		//(오른손 좌표계이므로, z 손가락이 내쪽으로 오게됨, 즉 up벡터는 -z)
		lookAt[3] = { Vector3(x, y - scale.y, z), Vector3(0, 0, +1) }; //아래쪽
			//(오른손 좌표계이므로, z 손가락이 바깥쪽으로 가게됨, 즉 up벡터는 +z)
		lookAt[4] = { Vector3(x, y, z + scale.z), Vector3(0, 1, 0) }; //정면
		lookAt[5] = { Vector3(x, y, z - scale.z), Vector3(0, 1, 0) }; //뒷면

		//카메라 변환->결과 : 뷰행렬
		for (UINT i = 0; i < 6; i++)
			D3DXMatrixLookAtLH(&desc_const_buffer_dynamicCube.Views[i], &position, &lookAt[i].LookAt, &lookAt[i].Up);
	}
	//Perspective(화면비율 1:1)
	//fov : 기본값 0.5 이므로 90도
	perspective = new Perspective(1, 1, zNear, zFar, Math::PI * fov);
	perspective->GetMatrix(&desc_const_buffer_dynamicCube.Projection);

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());


	D3D::Get()->SetRenderTarget(rtv, dsv);
	D3D::Get()->Clear(Color(0, 0, 0, 1), rtv, dsv);

	viewport->RSSetViewport();
}
