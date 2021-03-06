matrix World;
matrix View;
matrix Projection;
Texture2D Map; 
Texture2D Map2; 
//설명
//텍스쳐를 맵으로 부름

struct VertexInput
{
    float4 Position : Position;
};

struct VertexOutput
{
    float4 Position : SV_Position;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    return output;
}
 

float4 PS(VertexOutput input) : SV_Target
{
    return float4(1, 1, 1, 1);
}
RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

technique11 T0
{
    pass P0
    {
        SetRasterizerState(FillMode_WireFrame);

        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
  
}