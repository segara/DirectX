#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

////////////////////////////////////////////////
//
// Billboard
//
////////////////////////////////////////////////
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


float4 PS(MeshOutput input) : SV_Target
{
    return PS_AllLight(input);
}


struct GeometryOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
    uint MapIndex : MapIndex;
};

[maxvertexcount(4)] //지오메트리에서 출력될 정점의 최대개수
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
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1); //좌하단
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1); //좌상단
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1); //우하단
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1); //우상단
    
    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };
    
    GeometryOutput output;
    
    //triangle strip 으로 그림 : 정점이 4개일때 사각형하나(index없음)
    [unroll(4)]
    for (int i = 0; i < 4; i++)
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
        output.Uv = uv[i % 4];
        output.MapIndex = input[0].MapIndex;
        
        stream.Append(output);
        
        [flatten]
        if (i == 3)
            stream.RestartStrip(); //strip을 끊고 새로 시작하라

    }
    
}
Texture2DArray BillboardMap; //빌보드 텍스쳐의 array
float4 PS_Billboard(GeometryOutput input) : SV_Target
{
    return BillboardMap.Sample(LinearSampler, float3(input.Uv, input.MapIndex)) * 1.75f;
}

////////////////////////////////////////////////
//
// DynamicCubeMap
//
////////////////////////////////////////////////
// CB_DynamicCube : 전역변수처럼 사용
//
cbuffer CB_DynamicCube
{
    uint CubeRenderType = 0;
    float3 CB_DynamicCube_Padding;
    
    matrix CubeViews[6];
    matrix CubeProjection;
};
//육면으로 렌더링 하므로 하나의 삼각형당 6번렌더링
//3*6 = 18
//point:1개의 점에대해 처리 , triangle:3개의 점에대해 처리
[maxvertexcount(18)]
void GS_PreRender(triangle MeshOutput input[3], inout TriangleStream<MeshGeometryOutput> stream)
{
    int vertex = 0;
    MeshGeometryOutput output;
    
     [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i; //타겟되는 면
        
        [unroll(3)]
        for (vertex = 0; vertex < 3; vertex++)
        {
             //그냥 Position은 mvp변환이 끝난상태
            //그래서 wPoition을 쓴다.
            output.Position = mul(float4(input[vertex].wPosition, 1), CubeViews[i]);
            output.Position = mul(output.Position, CubeProjection);
            
            output.oPosition = input[vertex].oPosition;
            output.wPosition = input[vertex].wPosition;
            output.Normal = input[vertex].Normal;
            output.Tangent = input[vertex].Tangent;
            output.Uv = input[vertex].Uv;
            output.Color = input[vertex].Color;
            
            stream.Append(output);
        }
        
        stream.RestartStrip();
    }
}
//sky cubemap 통합처리, 
float4 PS_PreRender_Sky(MeshGeometryOutput input) : SV_Target
{
    //MeshGeometryOutput을 MeshOutput으로 변환 후 넘김
    return PS_Sky(ConvertMeshOutput(input));
}

float4 PS_PreRender(MeshGeometryOutput input) : SV_Target
{
    return PS_AllLight(ConvertMeshOutput(input));
}

TextureCube DynamicCubeMap;

float RefractionAmount = 1;
float RefractionAlpha = 0.5f;

float CubeMapAmount = 1.0f;
float CubeMapBias = 1.0f;
float CubeMapScale = 1.0f;
//cubemap으로 렌더링 : vs->ps 이기때문에 MeshOutput을 받는다.
//아무 물체에나 적용가능
//로컬 정점의 위치에 맞는 큐브맵의 픽셀을 가져옴
float4 PS_CubeMap(MeshOutput input) : SV_Target
{
   
    if (CubeRenderType == 0)
    {
        return DynamicCubeMap.Sample(LinearSampler, input.oPosition);
    }
    else if (CubeRenderType == 1)
    {
        //반사
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
        
        return DynamicCubeMap.Sample(LinearSampler, reflection);

    }
    else if (CubeRenderType == 2)
    {
        //반사후 굴절
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
      
        float3 refraction = refract(view, normal, RefractionAmount);
    
        //굴절함수는 빛이 들어오는 방향으로 계산결과에 -해준다
        float4 result = DynamicCubeMap.Sample(LinearSampler, -refraction);
        result.a = RefractionAlpha;
        return result;

    }
    else if (CubeRenderType == 3) //Fresnel반사(유사식)
    {
        float4 diffuse = 0;
        float4 color = 0;
        //반사
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
        
        diffuse = PS_AllLight(input);
        color = DynamicCubeMap.Sample(LinearSampler, reflection);
        
        color.rgb *= (0.15f + diffuse * 0.95f);
        return color;
    }
    else if (CubeRenderType == 4) //Fresnel반사 (정확)
    {
        float4 diffuse = 0;
        float4 color = 0;
        //반사
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
        
        diffuse = PS_AllLight(input);
        color = DynamicCubeMap.Sample(LinearSampler, reflection);
        
        float4 fresnel = CubeMapBias + (1.0f - CubeMapScale) * pow(abs(1.0f - dot(view, normal)), CubeMapAmount);
        color = CubeMapAmount * diffuse + lerp(diffuse, color, fresnel);
        color.a = 1.0f;
        return color;
    }
    return 0;
}

technique11 T0
{
  //Sky
    P_RS_DSS_VP(P0, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, PS_Sky)

    //Render
    P_VP(P1, VS_Mesh, PS)
    P_VP(P2, VS_Model, PS)
    P_VP(P3, VS_Animation, PS)

    //Billboard
    P_BS_VGP(P4, AlphaBlend, VS, GS_Billboard, PS_Billboard)
    P_RS_BS_VGP(P5, CullMode_None, AlphaBlend_AlphaToCoverageEnable, VS, GS_Cross, PS_Billboard)

    //Dynamic Cube Map PreRender

    P_RS_DSS_VGP(P6, FrontCounterClockwise_True, DepthEnable_False, VS_Mesh, GS_PreRender, PS_PreRender_Sky)
    
    P_VGP(P7, VS_Mesh, GS_PreRender, PS_PreRender)
    P_VGP(P8, VS_Model, GS_PreRender, PS_PreRender)
    P_VGP(P9, VS_Animation, GS_PreRender, PS_PreRender)

    //Dynamic CubeMap Render
    P_BS_VP(P10, AlphaBlend, VS_Mesh, PS_CubeMap)
    P_BS_VP(P11, AlphaBlend, VS_Model, PS_CubeMap)
    P_BS_VP(P12, AlphaBlend, VS_Animation, PS_CubeMap)
}