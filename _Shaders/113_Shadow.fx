#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input):SV_Target
{
    //float NdotL = normalize(input.Normal) * -GlobalLight.Direction;
    //float4 color = float4(1, 1, 1, 1) * NdotL;
    
    float4 color = PS_AllLight(input);
    
    float4 position = input.sPosition; //light ���� wvp��ȯ�� ������
    
    position.xyz /= position.w;
    //�������� ������ �ƴ϶�� ����
    [flatten]
    if(position.x < -1.0f || position.x > 1.0f ||
        position.y <-1.0f || position.y > 1.0f ||
        position.z < 0.0f || position.z > 1.0)
    {
        return color; 
    }
    
    position.x = position.x * 0.5f + 0.5f;
    position.y = -position.y * 0.5f + 0.5f;
    
    float depth = 0;
    float z = position.z - ShadowBias;
    float factor = 0;
    
    if(ShadowQuality == 0)
    {
        depth = ShadowMap.Sample(LinearSampler, position.xy).r; //���ٽ� �ؽ��� ������ R����
        //depth >= z (depth�ؽ����� z�� ���ų� ��ū���(���ų� �� �հ��) : �����κ�)
        //depth < z (���� ���� vertex.z �Ÿ��� depth�ؽ����� z���� ��ū���(�հ��) : ��ο�κ�)
        factor = (float) (depth >= z);
    }
    else if (ShadowQuality == 1)
    {
        depth = position.z;
        //SampleCmpLevelZero : mipmap������� ����
        //sampler ������
        //desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //�񱳰��� �۰ų� ������
        //���� depth_value >= position.z
        //�ᱹ  if(ShadowQuality == 0) ó���� ����
        
        factor = ShadowMap.SampleCmpLevelZero(ShadowSampler, position.xy, depth).r;
    }
    else if (ShadowQuality == 2) //PCF + Blur
    {
        depth = position.z;
        
        float2 size = 1.0f / ShadowMapSize;
        float2 offsets[] =
        {
            float2(-size.x, -size.y), float2(0.0f, -size.y), float2(+size.x, -size.y),
            float2(-size.x, 0.0f), float2(0.0f, 0.0f), float2(+size.x, 0.0f),
            float2(-size.x, +size.y), float2(0.0f, +size.y), float2(+size.x, +size.y),
        };
        
        
        float2 uv = 0;
        float sum = 0;
        
        [unroll(9)]
        for (int i = 0; i < 9; i++)
        {
            uv = position.xy + offsets[i];
            sum += ShadowMap.SampleCmpLevelZero(ShadowSampler, uv, depth).r;
        }
        
        factor = sum / 9.0f;
    }
    
  
    factor = saturate(factor + depth); //depth : ����� �־������� ���� Ŀ���Ƿ� �������.
    return color * factor;
}

technique11 T0
{
 
    //1pass : Depth Rendering
    P_RS_VP(P0,FrontCounterClockwise_True, VS_Depth_Mesh, PS_Depth)
    P_RS_VP(P1,FrontCounterClockwise_True, VS_Depth_Model, PS_Depth)
    P_RS_VP(P2,FrontCounterClockwise_True, VS_Depth_Animation, PS_Depth)

    //2pass : add shadow
    P_VP(P3, VS_Mesh, PS)
    P_VP(P4, VS_Model, PS)
    P_VP(P5, VS_Animation, PS)

}