#include "00_Global.fx"

Texture2D ParticleMap;

struct ParticleDesc
{
    float4 MinColor;
    float4 MaxColor;
    
    float3 Gravity;
    float EndVelocity;
    
    float2 StartSize;
    float2 EndSize;
    
    float2 RotateSpeed;
    float ReadyTime;
    float ReadyRandomTime;
    
    float ColorAmount;
    float CurrentTime;
};

cbuffer CB_Particle
{
    ParticleDesc Particle;
};

//c++ code : VertexParticle
struct VertexInput
{
    float4 Position : Position;
    float2 Corner : Uv;
    float3 Velocity : Velocity;
    float4 Random : Random; //x : 주기, y - 크기, z - 회전, w - 색상
    float Time : Time;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float4 Color : Color;
    float2 Uv : Uv;
};


float4 ComputePosition(float3 position, float3 velocity, float age, float normalizedAge)
{
    float start = length(velocity); //속도
    float end = start * Particle.EndVelocity;
    
    float amount = start * normalizedAge + ((end - start) * normalizedAge) / 2; 
    // v      |            
    //  end   |         /  
    //        |     /
    //  start |/_____________________
    //        |
    //        |
    //        ______________________
    //             (normalize age) t
    
    position += normalize(velocity) * amount * Particle.ReadyTime;
    position += Particle.Gravity * age * normalizedAge;
    
    //여기에서는 local - world 그대로 쓴다고 보면 됨 
    //따라서 world변환을 안하고 vp만 실행
    return ViewProjection(float4(position, 1));
}

float ComputeSize(float value, float normalizedAge)
{
    //value:크기의 랜덤값
    
    float start = lerp(Particle.StartSize.x, Particle.StartSize.y, value); //x:min y:max
    float end = lerp(Particle.EndSize.x, Particle.EndSize.y, value);
    
    return lerp(start, end, normalizedAge);
}

//z축 회전만 : position 계산에서 vp변환을 해주기 때문에 2d공간이기 때문임

float2x2 ComputeRotation(float value, float age)
{
    float angle = lerp(Particle.RotateSpeed.x, Particle.RotateSpeed.y, value);
    float radian = angle * age;
    
    float c = cos(radian);
    float s = sin(radian);
    
    return float2x2(c, -s, s, c);
}

float4 ComputeColor(float value, float normalizedAge)
{
    float4 color = lerp(Particle.MinColor, Particle.MaxColor, value) * normalizedAge;
    
    return color * Particle.ColorAmount;
}

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    
     //Random x : 주기, y - 크기, z - 회전, w - 색상
    
    float age = Particle.CurrentTime - input.Time;  //현재시간 - 시작시간
    age *= input.Random.x * Particle.ReadyRandomTime + 1; //Particle.ReadyRandomTime :랜덤대기시간 / +1 : 최소시간
    
    float normalizedAge = saturate(age / Particle.ReadyTime); //시간 비율
    
    output.Position = ComputePosition(input.Position.xyz, input.Velocity, age, normalizedAge);
    
    float size = ComputeSize(input.Random.y, normalizedAge);
    float2x2 rotation = ComputeRotation(input.Random.z, age);
    
    //c++코드에서 corner는 -1,-1 ~ 1,1 까지 4개의 모서리이다.
    //output.Position.xy는 하나의 파티클에 대한 위치값이므로 각 모서리에대한 값을 더한다.단 사각형형태이므로
    //각 변의 절반 값으로 곱해준다. 
    output.Position.xy += mul(input.Corner, rotation) * (size * 0.5f);
    
    output.Uv = (input.Corner + 1.0f) * 0.5f;
    output.Color = ComputeColor(input.Random.w, normalizedAge);

    return output;
}

float4 PS(VertexOutput input) : SV_Target
{
    return ParticleMap.Sample(LinearSampler, input.Uv) * input.Color;
}

technique11 T0
{
    P_DSS_BS_VP(P0, DepthRead_Particle, OpaqueBlend, VS, PS)
    P_DSS_BS_VP(P1, DepthRead_Particle, AdditiveBlend_Particle, VS, PS)
    P_DSS_BS_VP(P2, DepthRead_Particle,  AlphaBlend, VS, PS)
}