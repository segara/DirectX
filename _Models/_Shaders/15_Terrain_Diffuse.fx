matrix World;
matrix View;
matrix Projection;
Texture2D Map; 
Texture2D Map2; 
//설명
//텍스쳐를 맵으로 부름
float3 Direction; //조명방향
struct VertexInput
{
    float4 Position : Position; //float4 : 위치 (w 값이 위치를 계산하는데 사용, w값은 1로 들어옴)
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
    
    output.Normal = mul(input.Normal, (float3x3) World); //노멀 벡터도 world 변환 해줄것!
    return output;
}
 

float4 PS(VertexOutput input) : SV_Target
{
    float3 normal = normalize(input.Normal);
    float light = -Direction; //내적전 라이트 방향을 뒤집는다
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