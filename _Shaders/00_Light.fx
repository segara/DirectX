struct LightDesc
{
    float4 Ambient;
    float4 Specular;
    float3 Direction;
    float Padding;
    float3 Position;
};

cbuffer CB_Light
{
    LightDesc GlobalLight;
};


Texture2D DiffuseMap;
Texture2D SpecularMap;
Texture2D NormalMap;
TextureCube SkyCubeMap;
Texture2D ShadowMap;

SamplerComparisonState ShadowSampler;

struct MaterialDesc
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
};

cbuffer CB_Material
{
    MaterialDesc Material;
};

//초기화 함수
MaterialDesc MakeMaterial()
{
    MaterialDesc output;
    output.Ambient = float4(0, 0, 0, 0);
    output.Diffuse = float4(0, 0, 0, 0);
    output.Specular = float4(0, 0, 0, 0);
    output.Emissive = float4(0, 0, 0, 0);

    return output;
}

void AddMaterial(inout MaterialDesc result, MaterialDesc val)
{
    result.Ambient += val.Ambient;
    result.Diffuse += val.Diffuse;
    result.Specular += val.Specular;
    result.Emissive += val.Emissive;
}

float3 MaterialToColor(MaterialDesc result)
{
    return (result.Ambient + result.Diffuse + result.Specular + result.Emissive).rgb;
}

///////////////////////////////////////////////////////////////////////////////

void Texture(inout float4 color, Texture2D t, float2 uv, SamplerState samp)
{
    float4 sampling = t.Sample(samp, uv);
    
    color = color * sampling;
}

void Texture(inout float4 color, Texture2D t, float2 uv)
{
    Texture(color, t, uv, LinearSampler);
}

void ComputeLight(out MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    
    float3 direction = -GlobalLight.Direction;
    float NdotL = dot(direction, normalize(normal));
    
    output.Ambient = GlobalLight.Ambient * Material.Ambient; //color
    float3 Eye = normalize(ViewPosition() - wPosition); //eye 방향 벡터 :물체로부터 카메라 방향
    
    [flatten]
    if (NdotL > 0.0f)
    {
        output.Diffuse = Material.Diffuse * NdotL;
        
        [flatten]
        if(Material.Specular.a > 0.0f) //alpha 에 강도값이 들어감
        {
            float3 R = normalize(reflect(-direction, normal)); //reverse
            float RdotE = saturate(dot(R, Eye)); //0~1
            
            float specular = pow(RdotE, Material.Specular.a);
            output.Specular = Material.Specular * specular * GlobalLight.Specular;
        }
    }
     [flatten]
    if (Material.Emissive.a > 0.0f) //alpha 에 강도값이 들어감
    {
        float NdotE = dot(Eye, normalize(normal));
        //smoothstep : lerp와 같은 계산방식이지만 보간이 곡선으로 표현됨
        //Emissive.a값이 높을수록 외각선이 진해짐(넓어짐) :
        //smoothstep(min,max,X) : X가 min, max 사이에 있어야지만 적용됨 
        
        float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
        output.Emissive = Material.Emissive * emissive;
    }

}
#define MAX_POINT_LIGHTS 256
struct PointLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float intensity;
    float3 Padding;
};

cbuffer CB_PointLights
{
    uint PointLightCount;
    float3 CB_PointLights_Padding;
    
    PointLight PointLights[MAX_POINT_LIGHTS];
};

void ComputePointLight(inout MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    MaterialDesc result = MakeMaterial();
    
  
    for (uint i = 0; i < PointLightCount; i++)
    {
        float3 light = PointLights[i].Position - wPosition;
        float dist = length(light);
        
        [flatten]
        if (dist > PointLights[i].Range)
            continue;
        
        light /= dist; //방향벡터를 크기로 나눈다. normalize
        
        
        result.Ambient = PointLights[i].Ambient * Material.Ambient;
        
        float NdotL = dot(light, normalize(normal)); //조명위치와 vertex노멀이 이루는 벡터, vertex노멀벡터 dot계산
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (NdotL > 0.0f)
        {
            result.Diffuse = Material.Diffuse * NdotL * PointLights[i].Diffuse;
        
            [flatten]
            if (Material.Specular.a > 0.0f)
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));
            
                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * PointLights[i].Specular;
            }
        }
    
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(E, normalize(normal));
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
        
            result.Emissive = Material.Emissive * emissive * PointLights[i].Emissive;
        }
        //조명의 강도
        //(dist / PointLights[i].Range) : 실제거리/조명범위
        //1.0f/(거리비율) : 가까울수록 값이 크고  멀어질수록 값이 작아지게
        float temp = 1.0f / saturate(dist / PointLights[i].Range);
        float att = temp * temp * (1.0f / max(1.0f - PointLights[i].intensity, 1e-8f)); // 1e-8f : 0에 최대한 가까운수
        
        //result.Ambient * temp; <= 특별한 의미는 없음
        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }

}

#define MAX_SPOT_LIGHTS 256
struct SpotLight
{
    float4 Ambient;
    float4 Diffuse;
    float4 Specular;
    float4 Emissive;
    
    float3 Position;
    float Range;
    
    float3 Direction;
    float Angle;
    
    float intensity;
    float3 Padding;
};

cbuffer CB_SpotLights
{
    uint SpotLightCount;
    float3 CB_SpotLights_Padding;
    
    SpotLight SpotLights[MAX_SPOT_LIGHTS];
};

void ComputeSpotLight(inout MaterialDesc output, float3 normal, float3 wPosition)
{
    output = MakeMaterial();
    MaterialDesc result = MakeMaterial();
    
  
    for (uint i = 0; i < SpotLightCount; i++)
    {
        float3 light = SpotLights[i].Position - wPosition;
        float dist = length(light);
        
        [flatten]
        if (dist > SpotLights[i].Range)
            continue;
        
        light /= dist; //방향벡터를 크기로 나눈다. normalize
        
        
        result.Ambient = SpotLights[i].Ambient * Material.Ambient;
        
        float NdotL = dot(light, normalize(normal)); //조명위치와 vertex노멀이 이루는 벡터, vertex노멀벡터 dot계산
        float3 E = normalize(ViewPosition() - wPosition);

        [flatten]
        if (NdotL > 0.0f)
        {
            result.Diffuse = Material.Diffuse * NdotL * SpotLights[i].Diffuse;
        
            [flatten]
            if (Material.Specular.a > 0.0f)
            {
                float3 R = normalize(reflect(-light, normal));
                float RdotE = saturate(dot(R, E));
            
                float specular = pow(RdotE, Material.Specular.a);
                result.Specular = Material.Specular * specular * SpotLights[i].Specular;
            }
        }
    
        [flatten]
        if (Material.Emissive.a > 0.0f)
        {
            float NdotE = dot(E, normalize(normal));
            float emissive = smoothstep(1.0f - Material.Emissive.a, 1.0f, 1.0f - saturate(NdotE));
        
            result.Emissive = Material.Emissive.a * emissive * SpotLights[i].Emissive;
        }
        //spot light
        //point light에서 라이트의 빛이 퍼지는 모양이 원형태에서 원뿔형태로 바뀜
        // 조명과 vertex가 만드는 vector, 조명의 vector 를 내적 : 두vector 가 멀어질수록 감소
        // point light에서는 조명위치-vertex노멀 벡터와 vertex노멀벡터를 내적해줬지만
        // spot light에서는 (조명의 direction 벡터)와 (조명위치-vertex노멀 벡터)를 내적
        // pow를 사용하면 빛의 감소가 곡선형태로 일어난다
        //http://telnet.or.kr/directx/graphics/programmingguide/fixedfunction/lightsandmaterials/spotlightmodel.htm 그래프 참조
        float temp = pow(saturate(dot(-light, SpotLights[i].Direction)), SpotLights[i].Angle);
        float att = temp * (1.0f / max(1.0f - SpotLights[i].intensity, 1e-8f)); // 1e-8f : 0에 최대한 가까운수
        
        //result.Ambient * temp; <= 특별한 의미는 없음
        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }

}

void NormalMapping(float2 uv, float3 normal, float3 tangent, SamplerState sample)
{
    float4 map = NormalMap.Sample(sample, uv);
    [flatten]
    if (any(map.rgb) == false) //모든 변수의 값이 0이면 false, 하나라도 0보다 크면 true 
    {
        return;
    }
    
    float3 coord = map.rgb * 2.0f - 1.0f; //-1~1 사이값으로 변환
    
    //탄젠트 공간
    float3 Normal = normalize(normal); //vertex normal : Z에 맵핑
    float3 Tangent = normalize(tangent - dot(tangent, Normal) * Normal); //X에 맵핑
    float3 BiNormal = cross(Normal, Tangent); //Y에 맵핑
    
    float3x3 TBN = float3x3(Tangent, BiNormal, Normal);
    
    coord = mul(coord, TBN);
    
    Material.Diffuse *= saturate(dot(-GlobalLight.Direction, coord)); 
}

void NormalMapping(float2 uv, float3 normal, float3 tangent)
{
    NormalMapping(uv, normal, tangent, LinearSampler);
}
//////////////////////////////////////////////////////////////////////////////////////////////////////

float4 PS_AllLight(MeshOutput input)
{
    NormalMapping(input.Uv, input.Normal, input.Tangent);
    
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc output = MakeMaterial();
    MaterialDesc result = MakeMaterial();
    
    ComputeLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    ComputePointLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    ComputeSpotLight(output, input.Normal, input.wPosition);
    AddMaterial(result, output);
    
    return float4(MaterialToColor(result).rgb, 1.0f);
}
Texture2D ProjectorMap;

struct ProjectorDesc
{
    matrix View;
    matrix Projection;
    
    float4 Color;
};

cbuffer CB_Projector
{
    ProjectorDesc Projector;
};

void VS_Projector(inout float4 wvp, float4 oPosition)
{
    wvp = WorldPosition(oPosition);
    wvp = mul(wvp, Projector.View);
    wvp = mul(wvp, Projector.Projection);
}

void PS_Projector(inout float4 color, float4 wvp)
{
    float3 uvw = 0;
    uvw.x = wvp.x / wvp.w * 0.5f + 0.5f;
    uvw.y = -wvp.y / wvp.w * 0.5f + 0.5f; // - wvp.y : uv direction
    uvw.z = wvp.z / wvp.w;
    
    [flatten]
    if (saturate(uvw.x) == uvw.x && saturate(uvw.y) == uvw.y && saturate(uvw.z) == uvw.z)
    {
        float4 map = ProjectorMap.Sample(LinearSampler, uvw.xy);
        
        map.rgb *= Projector.Color.rgb;
        color = lerp(color, map, map.a);
    }
}

////////////////////////////////////////////////////////////////////////////

cbuffer CB_Shadow
{
    matrix ShadowView;
    matrix ShadowProjection;
    
    float2 ShadowMapSize;
    float ShadowBias;
    
    uint ShadowQuality;
};