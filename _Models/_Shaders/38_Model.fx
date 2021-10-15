#include "00_Global.fx"
float3 Direction = float3(-1, -1, 1);
struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
};

#define MAX_MODEL_TRANSFORMS 250
cbuffer CB_Bone
{
    matrix BoneTransforms[MAX_MODEL_TRANSFORMS];
    uint boneIndex;
};

cbuffer CB_Material
{
    float3 Ambient;
    float3 Diffuse;
    float3 Specular;
    float3 Emissive;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    //float3 oPosition : Position1;
    float3 Normal : Normal;
    float2 Uv : Uv;
     
};

VertexOutput VS(VertexModel input)
{

    VertexOutput output;
    
    //world(s*r*v)행렬에 미리 본 트랜스폼을 곱해준다
    //왜냐하면 현재 버텍스 포지션은 본에 붙어있는 로컬좌표이기 때문이다
    //따라서 본의 좌표를 구해준후
    //본의 좌표에다 버텍스 좌표를 mul해준다
    
    World = mul(BoneTransforms[boneIndex], World);
   
    output.Position = WorldPosition(input.Position);
    output.Position = ViewProjection(output.Position);

    output.Normal = WorldNormal(input.Normal);
    output.Uv = input.Uv;
  

    
    return output;
}

RasterizerState FrontCounterClockWise_True
{
    FrontCounterClockWise = true;
};

DepthStencilState Depth_Enable_False
{
    DepthEnable = false;
};

float4 PS(VertexOutput input) : SV_Target
{ 
   
    float NdotL = dot(normalize(input.Normal), -Direction);
    
    return DiffuseMap.Sample(LinearSampler, input.Uv) * NdotL;
}



technique11 T0
{
        P_VP(P0, VS,PS)
        P_RS_VP(P1, FillMode_WireFrame, VS, PS)
   
}