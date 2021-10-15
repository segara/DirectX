#include "00_Global.fx"
TextureCube CubeMap;


struct VertexOutput
{
    float4 Position : SV_Position;
    float3 oPosition : Position1;
    float2 Uv : Uv;
    float3 Normal : Normal;
};

VertexOutput VS(VertexTextureNormal input)
{
    VertexOutput output;
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);
 
    output.Normal = WorldNormal(input.Normal); //모든 계산된(회전,,) 행렬과 노말을 곱해준다
    
    output.oPosition = input.Position.xyz; 
    //월드변환하기 전의 로컬좌표로 큐브맵에서 해당좌표의 색상을 추출
    //스케일 변화 전이므로 사이즈가 맞음
    output.Uv = input.Uv;
    
    return output;
}


float4 PS(VertexOutput input) : SV_Target
{ 
    return CubeMap.Sample(LinearSampler, input.oPosition);
}


technique11 T0
{
    P_VP(P0, VS, PS)
    P_RS_VP(P1, FillMode_WireFrame, VS, PS)
}