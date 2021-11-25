#include "00_Global.fx"
#include "00_Light.fx"

struct VertexInput
{
    float4 Position : Position;
    float2 Uv : Uv;
    float2 Scale : Scale;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
    float4 position = WorldPosition(input.Position);
    
    float3 up = float3(0, 1, 0);
    //float3 forward = float3(0, 0, 1);
    float3 forward = position.xyz - ViewPosition();
    
    float3 right = normalize(cross(up, forward));//(1,0,0)
    
    //forward벡터가 시선방향으로 향하므로 
    //항상 카메라를 보게됨
    
    position.xyz += (input.Uv.x - 0.5f) * right * input.Scale.x; //x성분의 벡터
    position.xyz += ((1 - input.Uv.y) - 0.5f) * up * input.Scale.y; //(1 - input.Uv.v) 2D 좌표는 위에서 아래이므로
    position.w = 1.0f;
  
    output.Position = ViewProjection(position);
    output.Uv = input.Uv;
    
    return output;
    
}
float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    //clip(diffuse.a - 0.3) 0보다 작으면
    if (diffuse.a < 0.3)
        discard; //OM 까지 안가고 넘어감
    return diffuse;
}

technique11 T0
{
    P_VP(P0, VS, PS)
}