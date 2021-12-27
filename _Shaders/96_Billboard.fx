#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

//struct VertexInput
//{
//    float4 Position : Position;
//    float2 Uv : Uv;
//    float2 Scale : Scale;
//};

struct VertexBillboard
{
    float4 Position : Position;
    float2 Scale : Scale;
    uint MapIndex : MapIndex; //어떤 텍스쳐를 사용할지 인덱스
    //uint VertexIndex : SV_VertexID; //해당정점의 인덱스 : 시스템에서 부여함
}; 

struct VertexOutput
{
    float4 Position : Position;
    float2 Scale : Scale;
    uint MapIndex : MapIndex;
};

VertexOutput VS(VertexBillboard input)
{
    VertexOutput output;
    
    output.Position = WorldPosition(input.Position);
    output.Scale = input.Scale;
    output.MapIndex = input.MapIndex;
    
    return output;
}

struct GeometryOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    uint MapIndex : MapIndex;
};

[maxvertexcount(4)]//지오메트리에서 출력될 정점의 최대개수
//point:1개의 점에대해 처리 , triangle:3개의 점에대해 처리
//함수 return 값은 void이며 리턴할 값을TriangleStream에 넣어줘서 처리
//input[1] : 1개 array
void GS_Billboard(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    //float3 forward = float3(0, 0, 1);
    float3 forward = input[0].Position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));
    
    float2 size = input[0].Scale * 0.5f;
    
    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up ,1); //좌하단
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up ,1); //좌상단
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up ,1); //우하단
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up ,1); //우상단
    
    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };
    
    GeometryOutput output;
    
    //triangle strip 으로 그림 : 정점이 4개일때 사각형하나(index없음)
    [unroll(4)]
    for (int i = 0; i < 4;i++)
    {
        output.Position = ViewProjection(position[i]);
        output.Uv = uv[i];
        output.MapIndex = input[0].MapIndex;
        
        stream.Append(output);
    }
    
}
[maxvertexcount(8)]
void GS_Cross(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    float3 forward = float3(0, 0, 1);
   float3 right = normalize(cross(up, forward));
    
    float2 size = input[0].Scale * 0.5f;
    
    float4 position[8];
    position[0] = float4(input[0].Position.xyz - (size.x * right) - (size.y * up), 1); //좌하단
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1); //좌상단
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1); //우하단
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1); //우상단
    
    position[4] = float4(input[0].Position.xyz - size.x * forward - size.y * up, 1); //좌하단
    position[5] = float4(input[0].Position.xyz - size.x * forward + size.y * up, 1); //좌상단
    position[6] = float4(input[0].Position.xyz + size.x * forward - size.y * up, 1); //우하단
    position[7] = float4(input[0].Position.xyz + size.x * forward + size.y * up, 1); //우상단
    
    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };
    
    GeometryOutput output;
    
    //triangle strip 으로 그림 : 정점이 4개일때 사각형하나(index없음)
    [unroll(8)]
    for (int i = 0; i < 8; i++)
    {
        output.Position = ViewProjection(position[i]);
        output.Uv = uv[i%4];
        output.MapIndex = input[0].MapIndex;
        
        stream.Append(output);
        
        [flatten]
        if(i==3)
            stream.RestartStrip(); //strip을 끊고 새로 시작하라

    }
    
}
Texture2DArray BillboardMap; //빌보드 텍스쳐의 array
float4 PS_Billboard(GeometryOutput input) : SV_Target
{
    return BillboardMap.Sample(LinearSampler, float3(input.Uv, input.MapIndex)) * 1.75f;
}


float4 PS(MeshOutput input) : SV_Target
{
    return PS_AllLight(input);
}

technique11 T0
{
   P_VP(P0, VS_Mesh, PS)
   P_VP(P1, VS_Model, PS)
   P_VP(P2, VS_Animation, PS)
   P_BS_VGP(P3,AlphaBlend, VS, GS_Billboard, PS_Billboard)
   P_RS_BS_VGP(P4, CullMode_None, AlphaBlend_AlphaToCoverageEnable, VS, GS_Cross, PS_Billboard)
}