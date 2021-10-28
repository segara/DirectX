#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float light = GlobalLight.Direction; //������ ����Ʈ ������ �����´�
    return DiffuseMap.Sample(LinearSampler, input.Uv)* dot(light, normal);
}


technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    //P_RS_VP(P1, FillMode_WireFrame, VS, PS)
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