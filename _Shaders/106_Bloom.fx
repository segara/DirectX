#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float2 PixelSize;

struct VertexOutput
{
    float4 Postion : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(float4 Position : Position)
{
    VertexOutput output;
    output.Postion = Position; //����ȭ�� ��ǥ��� �Ѱ������(ndc :ȭ�� ��ǥ)
    output.Uv.x = Position.x * 0.5f + 0.5f; //-1~1��0~1�� ��ȯ
    output.Uv.y = -Position.y * 0.5f + 0.5f; //-1~1��0~1�� ��ȯ, �� ����0,�Ʒ���1�̹Ƿ� 
    return output;
}


//SV_Target: ����Ÿ������ ������
float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}

float Threshold = 0.6f;
float4 PS_Luminosity(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    return saturate((color - Threshold) / (1 - Threshold));
    //color �ִ밪�� 1�̱� ������ (1-Threshold)���ش�
    //�� ������� 0~1���� ���� ��
}

#define MAX_SAMPLE_COUNT 33
int SampleCount = 15;

float2 Offsets[MAX_SAMPLE_COUNT];
float Weights[MAX_SAMPLE_COUNT];

float4 PS_Blur(VertexOutput input) : SV_Target
{
    float4 color = 0;
    
    for (int i = 0; i < SampleCount; i++)
    {
        if ((input.Uv.y + Offsets[i].y) < 0)
        {
            color += DiffuseMap.Sample(LinearSampler, input.Uv) * Weights[i];
            continue;
        }
        color += DiffuseMap.Sample(LinearSampler, (input.Uv + Offsets[i])) * Weights[i];

    }

    return color;
}
Texture2D LuminosityMap;
Texture2D BlurMap;
float4 PS_Composite(VertexOutput input) : SV_Target
{
    float4 luminosity = LuminosityMap.Sample(LinearSampler, input.Uv);
    float4 blur = BlurMap.Sample(LinearSampler, input.Uv);
    
    //luminosity �� ���� ����
    luminosity *= (1.0f - saturate(blur));
    
    return float4((luminosity + blur).rgb, 1.0f);
}

technique11 T0
{
   P_VP(P0, VS, PS_Diffuse)
   P_VP(P1, VS, PS_Luminosity)
   P_VP(P2, VS, PS_Blur)
   P_VP(P3, VS, PS_Composite)
}