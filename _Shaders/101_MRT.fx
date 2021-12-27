#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float2 PixelSize;

struct VertexOutput
{
    float4 Postion : SV_Position;
    float2 Uv : Uv;
};


//��Ƽ ���� Ÿ���� ���
//SV_Target0 , SV_Target1, SV_Target2...
struct PixelOutput
{
    float4 Color0 : SV_Target0;
    float4 Color1 : SV_Target1;
    float4 Color2 : SV_Target2;
};

VertexOutput VS(float4 Position : Position)
{
    VertexOutput output;
    output.Postion = Position; //����ȭ�� ��ǥ��� �Ѱ������(ndc :ȭ�� ��ǥ)
    output.Uv.x = Position.x * 0.5f + 0.5f; //-1~1��0~1�� ��ȯ
    output.Uv.y = -Position.y * 0.5f + 0.5f; //-1~1��0~1�� ��ȯ, �� ����0,�Ʒ���1�̹Ƿ� 
    return output;
}

PixelOutput PS_MRT(VertexOutput input)
{
    PixelOutput output;
    //����
    output.Color0 = DiffuseMap.Sample(LinearSampler, input.Uv);
    //����
    output.Color1 = 1- DiffuseMap.Sample(LinearSampler, input.Uv);
    //ȸ��
    float3 color = DiffuseMap.Sample(LinearSampler, input.Uv).rgb;
    float average = (color.r + color.g + color.b) / 3.0f;
    output.Color2 = float4(average, average, average, 1.0f);
    
    return output;
}
//SV_Target: ����Ÿ������ ������
float4 PS(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}
technique11 T0
{
   P_VP(P0, VS, PS_MRT)
   P_VP(P1, VS, PS)
  
}