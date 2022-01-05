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
    uint MapIndex : MapIndex; //� �ؽ��ĸ� ������� �ε���
    //uint VertexIndex : SV_VertexID; //�ش������� �ε��� : �ý��ۿ��� �ο���
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

[maxvertexcount(4)] //������Ʈ������ ��µ� ������ �ִ밳��
//point:1���� �������� ó�� , triangle:3���� �������� ó��
//�Լ� return ���� void�̸� ������ ����TriangleStream�� �־��༭ ó��
//input[1] : 1�� array
void GS_Billboard(point VertexOutput input[1], inout TriangleStream<GeometryOutput> stream)
{
    float3 up = float3(0, 1, 0);
    //float3 forward = float3(0, 0, 1);
    float3 forward = input[0].Position.xyz - ViewPosition();
    float3 right = normalize(cross(up, forward));
    
    float2 size = input[0].Scale * 0.5f;
    
    float4 position[4];
    position[0] = float4(input[0].Position.xyz - size.x * right - size.y * up, 1); //���ϴ�
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1); //�»��
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1); //���ϴ�
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1); //����
    
    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };
    
    GeometryOutput output;
    
    //triangle strip ���� �׸� : ������ 4���϶� �簢���ϳ�(index����)
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
    position[0] = float4(input[0].Position.xyz - (size.x * right) - (size.y * up), 1); //���ϴ�
    position[1] = float4(input[0].Position.xyz - size.x * right + size.y * up, 1); //�»��
    position[2] = float4(input[0].Position.xyz + size.x * right - size.y * up, 1); //���ϴ�
    position[3] = float4(input[0].Position.xyz + size.x * right + size.y * up, 1); //����
    
    position[4] = float4(input[0].Position.xyz - size.x * forward - size.y * up, 1); //���ϴ�
    position[5] = float4(input[0].Position.xyz - size.x * forward + size.y * up, 1); //�»��
    position[6] = float4(input[0].Position.xyz + size.x * forward - size.y * up, 1); //���ϴ�
    position[7] = float4(input[0].Position.xyz + size.x * forward + size.y * up, 1); //����
    
    float2 uv[4] =
    {
        float2(0, 1), float2(0, 0), float2(1, 1), float2(1, 0)
    };
    
    GeometryOutput output;
    
    //triangle strip ���� �׸� : ������ 4���϶� �簢���ϳ�(index����)
    [unroll(8)]
    for (int i = 0; i < 8; i++)
    {
        output.Position = ViewProjection(position[i]);
        output.Uv = uv[i % 4];
        output.MapIndex = input[0].MapIndex;
        
        stream.Append(output);
        
        [flatten]
        if (i == 3)
            stream.RestartStrip(); //strip�� ���� ���� �����϶�

    }
    
}
Texture2DArray BillboardMap; //������ �ؽ����� array
float4 PS_Billboard(GeometryOutput input) : SV_Target
{
    return BillboardMap.Sample(LinearSampler, float3(input.Uv, input.MapIndex)) * 1.75f;
}

////////////////////////////////////////////////
//
// DynamicCubeMap
//
////////////////////////////////////////////////
// CB_DynamicCube : ��������ó�� ���
//
cbuffer CB_DynamicCube
{
    uint CubeRenderType = 0;
    float3 CB_DynamicCube_Padding;
    
    matrix CubeViews[6];
    matrix CubeProjection;
};
//�������� ������ �ϹǷ� �ϳ��� �ﰢ���� 6��������
//3*6 = 18
//point:1���� �������� ó�� , triangle:3���� �������� ó��
[maxvertexcount(18)]
void GS_PreRender(triangle MeshOutput input[3], inout TriangleStream<MeshGeometryOutput> stream)
{
    int vertex = 0;
    MeshGeometryOutput output;
    
     [unroll(6)]
    for (int i = 0; i < 6; i++)
    {
        output.TargetIndex = i; //Ÿ�ٵǴ� ��
        
        [unroll(3)]
        for (vertex = 0; vertex < 3; vertex++)
        {
             //�׳� Position�� mvp��ȯ�� ��������
            //�׷��� wPoition�� ����.
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
//sky cubemap ����ó��, 
float4 PS_PreRender_Sky(MeshGeometryOutput input) : SV_Target
{
    //MeshGeometryOutput�� MeshOutput���� ��ȯ �� �ѱ�
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
//cubemap���� ������ : vs->ps �̱⶧���� MeshOutput�� �޴´�.
//�ƹ� ��ü���� ���밡��
//���� ������ ��ġ�� �´� ť����� �ȼ��� ������
float4 PS_CubeMap(MeshOutput input) : SV_Target
{
   
    if (CubeRenderType == 0)
    {
        return DynamicCubeMap.Sample(LinearSampler, input.oPosition);
    }
    else if (CubeRenderType == 1)
    {
        //�ݻ�
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
        
        return DynamicCubeMap.Sample(LinearSampler, reflection);

    }
    else if (CubeRenderType == 2)
    {
        //�ݻ��� ����
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
      
        float3 refraction = refract(view, normal, RefractionAmount);
    
        //�����Լ��� ���� ������ �������� ������� -���ش�
        float4 result = DynamicCubeMap.Sample(LinearSampler, -refraction);
        result.a = RefractionAlpha;
        return result;

    }
    else if (CubeRenderType == 3) //Fresnel�ݻ�(�����)
    {
        float4 diffuse = 0;
        float4 color = 0;
        //�ݻ�
        float3 view = normalize(input.wPosition - ViewPosition());
        float3 normal = normalize(input.Normal);
        float3 reflection = reflect(view, normal);
        
        diffuse = PS_AllLight(input);
        color = DynamicCubeMap.Sample(LinearSampler, reflection);
        
        color.rgb *= (0.15f + diffuse * 0.95f);
        return color;
    }
    else if (CubeRenderType == 4) //Fresnel�ݻ� (��Ȯ)
    {
        float4 diffuse = 0;
        float4 color = 0;
        //�ݻ�
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