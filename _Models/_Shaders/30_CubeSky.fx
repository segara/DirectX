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
//    //���庯ȯ�ϱ� ���� ������ǥ�� ť��ʿ��� �ش���ǥ�� ������ ����
//    //������ ��ȭ ���̹Ƿ� ����� ����
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
    //    SetDepthStencilState(Depth_Enable_False,0); //���� ������ ���������� �ʰ� �ڵ� ������ ������
    //    SetVertexShader(CompileShader(vs_5_0, VS()));
    //    SetPixelShader(CompileShader(ps_5_0, PS()));
    //}
   
}