#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float2 PixelSize;

struct VertexOutput
{
    float4 Postion : SV_Position;
    float2 Uv : Uv;
};


//멀티 렌더 타겟의 사용
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
    output.Postion = Position; //정규화된 좌표계로 넘겨줘야함(ndc :화면 좌표)
    output.Uv.x = Position.x * 0.5f + 0.5f; //-1~1을0~1로 변환
    output.Uv.y = -Position.y * 0.5f + 0.5f; //-1~1을0~1로 변환, 단 위가0,아래가1이므로 
    return output;
}

PixelOutput PS_MRT(VertexOutput input)
{
    PixelOutput output;
    //원본
    output.Color0 = DiffuseMap.Sample(LinearSampler, input.Uv);
    //반전
    output.Color1 = 1- DiffuseMap.Sample(LinearSampler, input.Uv);
    //회색
    float3 color = DiffuseMap.Sample(LinearSampler, input.Uv).rgb;
    float average = (color.r + color.g + color.b) / 3.0f;
    output.Color2 = float4(average, average, average, 1.0f);
    
    return output;
}
//SV_Target: 렌더타겟으로 보낸다
float4 PS(VertexOutput input) : SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}
technique11 T0
{
   P_VP(P0, VS, PS_MRT)
   P_VP(P1, VS, PS)
  
}