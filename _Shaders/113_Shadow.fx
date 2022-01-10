#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float4 PS(MeshOutput input):SV_Target
{
    //float NdotL = normalize(input.Normal) * -GlobalLight.Direction;
    //float4 color = float4(1, 1, 1, 1) * NdotL;
    
    float4 color = PS_AllLight(input);
    
    float4 position = input.sPosition; //light 기준 wvp변환된 포지션
    
    position.xyz /= position.w;
    //프로젝션 영역이 아니라면 제외
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
        depth = ShadowMap.Sample(LinearSampler, position.xy).r; //스텐실 텍스쳐 포맷이 R만씀
        //depth >= z (depth텍스쳐의 z가 같거나 더큰경우(같거나 더 먼경우) : 밝은부분)
        //depth < z (현재 빛과 vertex.z 거리가 depth텍스쳐의 z보다 더큰경우(먼경우) : 어두운부분)
        factor = (float) (depth >= z);
    }
    else if (ShadowQuality == 1)
    {
        depth = position.z;
        //SampleCmpLevelZero : mipmap사용하지 않음
        //sampler 생성시
        //desc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL; //비교값이 작거나 같은가
        //내부 depth_value >= position.z
        //결국  if(ShadowQuality == 0) 처리와 동일
        
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
    
  
    factor = saturate(factor + depth); //depth : 조명과 멀어질수록 값이 커지므로 밝아진다.
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