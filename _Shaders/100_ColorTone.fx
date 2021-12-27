#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"

float2 PixelSize;

struct VertexOutput
{
    float4 Postion : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(float4 Position : Position)
{
    VertexOutput output;
    output.Postion = Position; //����ȭ�� ��ǥ��� �Ѱ������(ndc :ȭ�� ��ǥ)
    output.Uv.x = Position.x * 0.5f + 0.5f; //-1~1��0~1�� ��ȯ
    output.Uv.y = -Position.y * 0.5f + 0.5f; //-1~1��0~1�� ��ȯ, �� ����0,�Ʒ���1�̹Ƿ� 
    return output;
}

float4 PS_Diffuse(VertexOutput input):SV_Target
{
    return DiffuseMap.Sample(LinearSampler, input.Uv);
}
float4 PS_Inverse(VertexOutput input) : SV_Target
{
    return float4(1.0f - DiffuseMap.Sample(LinearSampler, input.Uv).rgb, 1);
}
float4 PS_GrayScale(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    float average = (color.r + color.g + color.b) / 3.0f;
    return float4(average, average, average,1.0f);
}
float4 PS_GrayScale2(VertexOutput input) : SV_Target
{
    float4 color = float4(DiffuseMap.Sample(LinearSampler, input.Uv));
    float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    //���� a��b = x1*x2 + y1*y2 + z1*z2
    float average = dot(color.rgb, grayscale);
    
    return float4(average, average, average, 1.0f);
}

float Sharpness = 0;
  
float4 PS_Sharpness(VertexOutput input) : SV_Target
{
    float4 center = DiffuseMap.Sample(LinearSampler, input.Uv);
    //uv������ �ȼ� ������ ��ŭ �̵��Ѵ�
    float4 top = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, -PixelSize.y));
    float4 bottom = DiffuseMap.Sample(LinearSampler, input.Uv + float2(0, +PixelSize.y));
    float4 left = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-PixelSize.x, 0));
    float4 right = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+PixelSize.x, 0));
    
    //center*4���� �ֺ��ȼ����� ���Ƿ� �ֺ��ȼ����� �����̸� ���´�
    float edge = center * 4 - top - bottom - left - right;
    
    return (center + Sharpness) * edge;
}

float Saturation = 0;
//Saturation = 0 : grayscale (���)
//0 < Saturation < 1 : desaturation
//Saturation = 1 : original (������)
//Saturation > 1 : satuaration (����)

float4 PS_Saturation(VertexOutput input) : SV_Target
{
    float4 color = float4(DiffuseMap.Sample(LinearSampler, input.Uv));
    float3 grayscale = float3(0.2627f, 0.6780f, 0.0593f);
    
    float temp = dot(color.rgb, grayscale);
    
    color.rgb = lerp(temp, color.rgb, Saturation);
    color.a = 1.0f;
    
    return color;
}

//color corrction : sepia

float4x4 ColorToSepiaMatrix = float4x4
	(
		0.393, 0.769, 0.189, 0,
		0.349, 0.686, 0.168, 0,
		0.272, 0.534, 0.131, 0,
		0, 0, 0, 1
	);

float4 PS_Sepia(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    return mul(ColorToSepiaMatrix, color);
}

float Power = 2; //1 : Linear, 1 > Non-Linear
float2 Scale = float2(2, 2);

float4 PS_Vignette(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    
    float radius = length(((input.Uv - 0.5f) * 2) / Scale);
    //ȭ�� �߾��̶�� input.Uv�� 0.5�� 0�̰� �ᱹ ���������� ǥ��
    
    float vignette = pow(abs(radius + 0.0001f), Power);
    //0.0001f : radius�� 0�϶��� ���� ����
    
    return saturate(1 - vignette) * color;
}

  
//3D red, green color divide
float LensPower = 1;
float3 Distortion = -0.02f;

float4 PS_LensDistortion(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv * 2 - 1;
    
    float2 vpSize = float2(1.0f / PixelSize.x, 1.0f / PixelSize.y);
    float aspect = vpSize.x / vpSize.y;
    float radiusSquared = aspect * aspect + uv.x * uv.x + uv.y * uv.y;
    float radius = sqrt(radiusSquared);

    float3 f = Distortion * pow(abs(radius + 1e-6f), LensPower) + 1;
    
    float2 r = (f.r * uv + 1) * 0.5f;
    float2 g = (f.g * uv + 1) * 0.5f;
    float2 b = (f.b * uv + 1) * 0.5f;
    
    float4 color = 0;
    color.r = DiffuseMap.Sample(LinearSampler, r).r;
    color.ga = DiffuseMap.Sample(LinearSampler, g).ga;
    color.b = DiffuseMap.Sample(LinearSampler, b).b;
    
    return color;
}
//tv ������
float Strength = 1.0f;
int interaceValue = 2;

float4 PS_Interace(VertexOutput input) : SV_Target
{
    float4 color = DiffuseMap.Sample(LinearSampler, input.Uv);
    float height = 1.0f / PixelSize.y;
    
    //floor :����, ceil:�ø� , round :�ݿø�
    int value = (int) ((floor(input.Uv.y * height) % interaceValue) / (interaceValue / 2));
    
    [flatten]
    if (value)
    {
        float3 grayScale = float3(0.2126f, 0.7152f, 0.0722f);
        float luminance = dot(color.rgb, grayScale);
        
        luminance = min(0.999f, luminance);
        
        color.rgb = lerp(color.rgb, color.rgb * luminance, Strength);
    }
    return color;
}

float2 ScaleSourceSize;
float4 PS_Blur(VertexOutput input) : SV_Target
{
    float2 size = 1.0f / ScaleSourceSize;
    
    //ScaleSourceSize�� Ŭ���� blur�� ���� 
    //�����¿� 4�ȼ��� ��ġ�� ������ ScaleSourceSize�� �۾������� �׸��� �밢��4��������
    //�и��Ǵ� �����̳�
    float4 s0 = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-size.x, -size.y));
    float4 s1 = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+size.x, -size.y));
    float4 s2 = DiffuseMap.Sample(LinearSampler, input.Uv + float2(-size.x, +size.y));
    float4 s3 = DiffuseMap.Sample(LinearSampler, input.Uv + float2(+size.x, +size.y));
    
    return (s0 + s1 + s2 + s3) / 4;
}
//�ﷷ��
float2 WiggleOffset = float2(10, 10);
float2 WiggleAmount = float2(0.01f, 0.01f);
float4 PS_Wiggle(VertexOutput input) : SV_Target
{
    float2 uv = input.Uv;
    uv.x += sin(Time + uv.x * WiggleOffset.x) * WiggleAmount.x;
    uv.y += cos(Time + uv.y * WiggleOffset.y) * WiggleAmount.y;
    
    return DiffuseMap.Sample(LinearSampler, uv);
}

/// http://3dapi.com/blc/����

technique11 T0
{
   P_VP(P0, VS, PS_Diffuse)
   P_VP(P1, VS, PS_Inverse)
   P_VP(P2, VS, PS_GrayScale)
   P_VP(P3, VS, PS_GrayScale2)
   P_VP(P4, VS, PS_Sharpness)
   P_VP(P5, VS, PS_Saturation)
   P_VP(P6, VS, PS_Sepia)
   P_VP(P7, VS, PS_Vignette)
   P_VP(P8, VS, PS_LensDistortion)
   P_VP(P9, VS, PS_Interace)
   P_VP(P10, VS, PS_Blur)
}