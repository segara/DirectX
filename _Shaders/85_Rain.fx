#include "00_Global.fx"
#include "00_Light.fx"

cbuffer CB_Rain
{
    float4 Color;

    float3 Velocity;
    float DrawDistance;

    float3 Origin;
    float CB_Rain_Padding;

    float3 Extent;
};

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
    float Alpha : Alpha;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    //Velocity 에 비내리는 방향으로 지정
    float3 displace = Velocity;
    
    displace.xz /= input.Scale.y * 0.1f; //y scale이 커질수록 x, z 이동이 느려지게
    displace *= Time;
    
    input.Position.xyz = Origin + (Extent + (input.Position.xyz + displace) % Extent) % Extent - (Extent * 0.5f);
    //Extent 나머지 값이므로 계속 순환됨
    //Extent 나머지 값을 한번 더 계산하는 이유 : origin으로 포지션값이 변했으므로 한번 더 나눈값
    //- (Extent * 0.5f) : origin 이동이후 중심점으로 위치시킴
    
    float4 position = WorldPosition(input.Position);
    
    float3 up = normalize(-Velocity);
    //Velocity의 반대
    //float3 forward = float3(0, 0, 1);
    float3 forward = position.xyz - ViewPosition();
    
    float3 right = normalize(cross(up, forward));//(1,0,0)
    
    position.xyz += (input.Uv.x - 0.5f) * right * input.Scale.x;
    position.xyz += ((1 - input.Uv.y) - 0.5f) * up * input.Scale.y; //(1 - input.Uv.v) 2D 좌표는 위에서 아래이므로
    position.w = 1.0f;
  
    output.Position = ViewProjection(position);
    output.Uv = input.Uv;
    
    float4 view = mul(position, View); //카메라 변환
    //1-1 : 멀수록 투명해진다.
    //*0.5f:큰의미 없음(보기 좋게)
    output.Alpha = saturate(1 - (view.z / DrawDistance)) * 0.5f;

    
    return output;
    
}
float4 PS(VertexOutput input) : SV_Target
{
    float4 diffuse = DiffuseMap.Sample(LinearSampler, input.Uv);
    ////clip(diffuse.a - 0.3) 0보다 작으면
    //if (diffuse.a < 0.3)
    //    discard; //OM 까지 안가고 넘어감
    //2.0f, 1.5f : magic number
    diffuse.rgb *= Color.rgb * input.Alpha * 2.0f;
    diffuse.a *= Color.rgb * input.Alpha * 1.5f;
    
   return diffuse;
}

technique11 T0
{
    P_BS_VP(P0,AlphaBlend, VS, PS)

    //P_BS_VP(P0, AdditiveBlend, VS, PS)
    //P_BS_VP(P1, AdditiveBlend_AlphaToCoverageEnable, VS, PS)

}