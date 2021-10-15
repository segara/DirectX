#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"


//struct VertexOutput
//{
//    float4 Position : SV_Position;
//    float3 oPosition : Position1;
//};

//VertexOutput VS(Vertex input)
//{
//    VertexOutput output;
//    output.Position = WorldPosition(input.Position);
//    output.Position = ViewProjection(output.Position);
//    output.oPosition = input.Position.xyz; 
//    //월드변환하기 전의 로컬좌표로 큐브맵에서 해당좌표의 색상을 추출
//    //스케일 변화 전이므로 사이즈가 맞음
//    return output;
//}

RasterizerState FrontCounterClockWise_True
{
    FrontCounterClockWise = true;
};

DepthStencilState Depth_Enable_False
{
    DepthEnable = false;
};

//float4 PS(VertexOutput input) : SV_Target
//{ 
//    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
//}

float4 PS(MeshOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}

technique11 T0
{
    P_RS_DSS_VP(P0, FrontCounterClockWise_True, DepthEnable_False, VS_Mesh, PS)
    //pass P0
    //{
    //    SetRasterizerState(FrontCounterClockWise_True);
    //    SetDepthStencilState(Depth_Enable_False,0); //깊이 순서로 렌더링되지 않고 코드 순서로 렌더링
    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS()));
    //}
   
}