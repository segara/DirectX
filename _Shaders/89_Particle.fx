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
    float4 Random : Random; //x : �ֱ�, y - ũ��, z - ȸ��, w - ����
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
    float start = length(velocity); //�ӵ�
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
    
    //���⿡���� local - world �״�� ���ٰ� ���� �� 
    //���� world��ȯ�� ���ϰ� vp�� ����
    return ViewProjection(float4(position, 1));
}

float ComputeSize(float value, float normalizedAge)
{
    //value:ũ���� ������
    
    float start = lerp(Particle.StartSize.x, Particle.StartSize.y, value); //x:min y:max
    float end = lerp(Particle.EndSize.x, Particle.EndSize.y, value);
    
    return lerp(start, end, normalizedAge);
}

//z�� ȸ���� : position ��꿡�� vp��ȯ�� ���ֱ� ������ 2d�����̱� ������

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
    
     //Random x : �ֱ�, y - ũ��, z - ȸ��, w - ����
    
    float age = Particle.CurrentTime - input.Time;  //����ð� - ���۽ð�
    age *= input.Random.x * Particle.ReadyRandomTime + 1; //Particle.ReadyRandomTime :�������ð� / +1 : �ּҽð�
    
    float normalizedAge = saturate(age / Particle.ReadyTime); //�ð� ����
    
    output.Position = ComputePosition(input.Position.xyz, input.Velocity, age, normalizedAge);
    
    float size = ComputeSize(input.Random.y, normalizedAge);
    float2x2 rotation = ComputeRotation(input.Random.z, age);
    
    //c++�ڵ忡�� corner�� -1,-1 ~ 1,1 ���� 4���� �𼭸��̴�.
    //output.Position.xy�� �ϳ��� ��ƼŬ�� ���� ��ġ���̹Ƿ� �� �𼭸������� ���� ���Ѵ�.�� �簢�������̹Ƿ�
    //�� ���� ���� ������ �����ش�. 
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