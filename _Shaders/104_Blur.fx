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

uint BlurCount = 1;

//SV_Target: ����Ÿ������ ������
float4 PS_Blur(VertexOutput input) : SV_Target
{
    float2 arr[9] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1),
        float2(-1, 0), float2(0, 0), float2(1, 0),
        float2(-1, 1), float2(0, 1), float2(1, 1)
    };
    float3 color = 0;
   
    //BlurCount ī��Ʈ�� �ܰ躰�� �÷����� ����
    //blur���� �ö󰡸鼭 �� �� �ؽ��� �ȼ����� ���´�  
    for (int blur = 0; blur < BlurCount; blur++)
    {
        for (int i = 0; i < 9; ++i)
        {
            float x = arr[i].x * (float) blur * PixelSize.x;
            float y = arr[i].y * (float) blur * PixelSize.y;
            
            float2 uv = input.Uv + float2(x, y);
            color += DiffuseMap.Sample(LinearSampler, uv).rgb;
        }

    }
    color /= (BlurCount * 9);
    return float4(color.rgb, 1);
}

uint RadialBlurCount = 8;
float RadialBlurRadius = 0.0f;
float RadialBlurAmount = 0.04f;
float2 RadialCenter = float2(0.5f, 0.5f);

float4 PS_RadialBlur(VertexOutput input):SV_Target
{
    float2 radius = input.Uv - RadialCenter;//uv��ǥ���� uv�߽�����ŭ ����(�ٽ�) 
    float r = length(radius) + 1e-6f;
    radius /= r; //������x,y
    
    r = 2 * r / RadialBlurRadius;
    r = saturate(r);
    
    float2 delta = radius * r * r * RadialBlurAmount / RadialBlurCount;
    delta = -delta;
    
    float4 color = 0;
    for (uint i = 0; i < RadialBlurCount; i++)
    {
        color += DiffuseMap.Sample(LinearSampler, input.Uv);
        input.Uv += delta;
    }
    color /= RadialBlurCount;
    return float4(color.rgb, 1);

}

const int GaussBlurCount = 6;
//������ �Լ�
static const float Weight[13] =
{
    0.0561f, 0.1353f, 0.2780f, 0.4868f, 0.7261f, 0.9231f,
    1.0f,
    0.9231f, 0.7261f, 0.4868f, 0.2780f, 0.1353f, 0.0561f
};
//����Ÿ���� 3�� �����
//1 : ����
//2 :���� blur
//3 : ���� blur
float4 PS_GaussianBlurX(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    float u = PixelSize.x;
    float sum = 0;
    float4 color = 0;
    
    for (int i = -GaussBlurCount; i <= GaussBlurCount; i++)
    {
        float2 temp = uv + float2(u * (float) i, 0.0f);
        color += Weight[6 + i] * DiffuseMap.Sample(LinearSampler, temp);
        sum += Weight[6 + i];
    }
    color /= sum;
    return float4(color.rgb, 1.0f);
}

float4 PS_GaussianBlurY(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    float v = PixelSize.y;
    float sum = 0;
    float4 color = 0;
    
    for (int i = -GaussBlurCount; i <= GaussBlurCount; i++)
    {
        float2 temp = uv + float2(0.0f,v * (float) i);
        color += Weight[6 + i] * DiffuseMap.Sample(LinearSampler, temp);
        sum += Weight[6 + i];
    }
    color /= sum;
    return float4(color.rgb, 1.0f);
}

technique11 T0
{
     P_VP(P0, VS, PS_Diffuse)

   P_VP(P1, VS, PS_Blur)
   P_VP(P2, VS, PS_RadialBlur)
   P_VP(P3, VS, PS_GaussianBlurX)
   P_VP(P4, VS, PS_GaussianBlurY)
  
}