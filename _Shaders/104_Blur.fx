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
    output.Postion = Position; //정규화된 좌표계로 넘겨줘야함(ndc :화면 좌표)
    output.Uv.x = Position.x * 0.5f + 0.5f; //-1~1을0~1로 변환
    output.Uv.y = -Position.y * 0.5f + 0.5f; //-1~1을0~1로 변환, 단 위가0,아래가1이므로 
    return output;
}


//SV_Target: 렌더타겟으로 보낸다
float4 PS_Diffuse(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv); 
}

uint BlurCount = 1;

//SV_Target: 렌더타겟으로 보낸다
float4 PS_Blur(VertexOutput input) : SV_Target
{
    float2 arr[9] =
    {
        float2(-1, -1), float2(0, -1), float2(1, -1),
        float2(-1, 0), float2(0, 0), float2(1, 0),
        float2(-1, 1), float2(0, 1), float2(1, 1)
    };
    float3 color = 0;
   
    //BlurCount 카운트를 단계별로 올려가며 실행
    //blur값이 올라가면서 더 먼 텍스쳐 픽셀값을 얻어온다  
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
    float2 radius = input.Uv - RadialCenter;//uv좌표에서 uv중심점만큼 뺀값(핵심) 
    float r = length(radius) + 1e-6f;
    radius /= r; //반지름x,y
    
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
//오목한 함수
static const float Weight[13] =
{
    0.0561f, 0.1353f, 0.2780f, 0.4868f, 0.7261f, 0.9231f,
    1.0f,
    0.9231f, 0.7261f, 0.4868f, 0.2780f, 0.1353f, 0.0561f
};
//렌더타겟을 3개 만든다
//1 : 원본
//2 :가로 blur
//3 : 세로 blur
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