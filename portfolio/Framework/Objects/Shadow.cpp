#include "Framework.h"
#include "Shadow.h"

Shadow::Shadow(Shader * shader, Vector3 & position, float radius, UINT width, UINT height)
	: shader(shader), position(position), radius(radius), width(width), height(height)
{
	renderTarget = new RenderTarget(width, height);
	depthStencil = new DepthStencil(width, height);
	viewport = new Viewport((float)width, (float)height);

	desc.MapSize = Vector2((float)width, (float)height);

	buffer = new ConstantBuffer(&desc, sizeof(Desc));
	sBuffer = shader->AsConstantBuffer("CB_Shadow");
	sShadowMap = shader->AsSRV("ShadowMap");

	//Create Sampler State
	{
		D3D11_SAMPLER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SAMPLER_DESC));
		desc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //비교값이 작거나 같은가
		desc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		desc.MaxAnisotropy = 1;
		desc.MaxLOD = FLT_MAX;

		Check(D3D::GetDevice()->CreateSamplerState(&desc, &pcfSampler));
		sPcfSampler = shader->AsSampler("ShadowSampler");
	}
}

Shadow::~Shadow()
{
	SafeDelete(renderTarget);
	SafeDelete(depthStencil);
	SafeDelete(viewport);

	SafeDelete(buffer);

}

void Shadow::PreRender()
{
	ImGui::InputInt("Quality", (int*)&desc.Quality);
	desc.Quality %= 3;
	//1PASS
	ImGui::SliderFloat3("Light Direction", Context::Get()->Direction(), -1, +1);
	ImGui::SliderFloat("Bias", &desc.Bias, -0.0001f,0.01f, "%.4f");
	renderTarget->PreRender(depthStencil);
	viewport->RSSetViewport();

	CalcViewProjection();

	buffer->Render();
	sBuffer->SetConstantBuffer(buffer->Buffer());

	sShadowMap->SetResource(depthStencil->SRV());
	sPcfSampler->SetSampler(0, pcfSampler);
}
/// 그림자가 그려질 공간
void Shadow::CalcViewProjection()
{
	Vector3 up = Vector3(0, 1, 0);
	Vector3 direction = Context::Get()->Direction();
	Vector3 position = direction * radius * -2.0f;
	//dirction은 normalize 되서 1 크기
	//radius * -2 : 그림자 공간 반지를 * 2 * -1(빛이 들어오는 방향으로 변경)

	D3DXMatrixLookAtLH(&desc.View, &position, &this->position, &up);

	//원점(this->position)을 view matrix 만큼 옮긴다.
	//즉, 카메라 원점과 일치시킴
	Vector3 origin;
	D3DXVec3TransformCoord(&origin, &this->position, &desc.View);

	float f_left   = origin.x - radius; //왼쪽면
	float f_bottom = origin.y - radius; //
	float f_near   = origin.z - radius; //

	float f_right = origin.x + radius; //
	float f_top = origin.y + radius; //
	float f_far = origin.z + radius; //

	//프로젝션 변환
	D3DXMatrixOrthoLH(&desc.Projection, f_right - f_left, f_top - f_bottom, f_near, f_far);
}
