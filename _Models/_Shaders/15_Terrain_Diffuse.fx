matrix World;
matrix View;
matrix Projection;
Texture2D Map; 
Texture2D Map2; 
//����
//�ؽ��ĸ� ������ �θ�
float3 Direction; //�������
struct VertexInput
{
    float4 Position : Position; //float4 : ��ġ (w ���� ��ġ�� ����ϴµ� ���, w���� 1�� ����)
    float3 Normal : Normal;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float3 Normal : Normal;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    
    output.Normal = mul(input.Normal, (float3x3) World); //��� ���͵� world ��ȯ ���ٰ�!
    return output;
}
 

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float light = -Direction; //������ ����Ʈ ������ �����´�
    return float4(1, 1, 1, 1) * dot(light, normal);
}
RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

technique11 T0
{
    pass P0
    {
        //SetRasterizerState(FillMode_WireFrame);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
  
}