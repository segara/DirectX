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

void ComputePointLights(inout MaterialDesc output, float3 normal, float3 wPosition)
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
        
        float NdotL = dot(light, normalize(normal)); //조명방향과 노멀의 dot계산
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
        float att = temp * temp * (1.0f / max(1.0f - PointLights[i].intensity, 1e-8f));
        
        //result.Ambient * temp; <= 특별한 의미는 없음
        output.Ambient += result.Ambient * temp;
        output.Diffuse += result.Diffuse * att;
        output.Specular += result.Specular * att;
        output.Emissive += result.Emissive * att;
    }

}