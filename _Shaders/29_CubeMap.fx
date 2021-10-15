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
 
    output.Normal = WorldNormal(input.Normal); //��� ����(ȸ��,,) ��İ� �븻�� �����ش�
    
    output.oPosition = input.Position.xyz; 
    //���庯ȯ�ϱ� ���� ������ǥ�� ť��ʿ��� �ش���ǥ�� ������ ����
    //������ ��ȭ ���̹Ƿ� ����� ����
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