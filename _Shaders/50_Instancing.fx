#include "00_Global.fx"

float3 Direction = float3(-1, -1, 1);
struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    
    matrix Transform : Inst1; //Inst:instancing 1: 1번슬롯
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    World = input.Transform; //1000개의 위치정보
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
    
    output.Normal = WorldNormal(input.Normal);
    
   
    output.Uv = input.Uv;
    
    return output;
}


float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float light = -Direction; //내적전 라이트 방향을 뒤집는다
    return DiffuseMap.Sample(LinearSampler, input.Uv)* dot(light, normal);
}


technique11 T0
{
    P_VP(P0, VS,PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)
    //pass P0
    //{
    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS()));
    //}
    //pass P1
    //{
    //    SetRasterizerState(FillMode_WireFrame);
    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS()));
    //}
}