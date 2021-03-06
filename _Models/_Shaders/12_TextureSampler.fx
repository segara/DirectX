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
    float2 Uv : Uv;
};

struct VertexOutput
{
    float4 Position : SV_Position;
    float2 Uv : Uv;
};

VertexOutput VS(VertexInput input)
{
    VertexOutput output;
    output.Position = mul(input.Position, World);
    output.Position = mul(output.Position, View);
    output.Position = mul(output.Position, Projection);
    output.Uv = input.Uv;
    return output;
}
 
SamplerState Samp;
//설명
//Sample : uv좌표에 따라 버텍스 표면에 바르는 역할
//이때 버텍스 공간상과 이미지 크기를 맞추기 위해 확대 / 축소를 실시함
//확대, 축소의 처리방식을 지정해줌(보간)

float4 PS(VertexOutput input) : SV_Target
{
    if(input.Uv.x < 0.5f)
       return Map2.Sample(Samp, input.Uv);
    return Map.Sample(Samp, input.Uv); 
}

uint Filter;
SamplerState Sampler_Filter_Point
{
    Filter = MIN_MAG_MIP_POINT;
};
SamplerState Sampler_Filter_Linear
{
    Filter = MIN_MAG_MIP_LINEAR;
};
uint Address;
//설명
//D3D11_SAMPLE_DESC 에 매치되는 변수(AddressU/AddressV)존재함
SamplerState Sampler_Address_Wrap
{
    AddressU = Wrap;
    AddressV = Wrap;
};
SamplerState Sampler_Address_Mirror
{
    AddressU = Mirror;
    AddressV = Mirror;
};
SamplerState Sampler_Address_Clamp
{
    AddressU = Clamp;
    AddressV = Clamp;
};
SamplerState Sampler_Address_Border
{
    AddressU = Border;
    AddressV = Border;
    BorderColor = float4(0, 0, 1, 1);

};
float4 PS_Filter(VertexOutput input) : SV_Target
{
    if(Filter == 0)
        return Map.Sample(Sampler_Filter_Point, input.Uv);
    else if(Filter == 1)
        return Map.Sample(Sampler_Filter_Linear, input.Uv);
    return Map.Sample(Samp, input.Uv);
}
float4 PS_Address(VertexOutput input) : SV_Target
{
    if (Address == 0)
        return Map.Sample(Sampler_Address_Wrap, input.Uv);
    else if (Address == 1)
        return Map.Sample(Sampler_Address_Mirror, input.Uv);
    else if (Address == 2)
        return Map.Sample(Sampler_Address_Clamp, input.Uv);
    else if (Address == 3)
        return Map.Sample(Sampler_Address_Border, input.Uv);
    
    return Map.Sample(Samp, input.Uv);
}
RasterizerState FillMode_WireFrame
{
    FillMode = WireFrame;
};

technique11 T0
{
    pass P0
    {
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS()));
    }
    pass P1
    {
        //SetRasterizerState(FillMode_WireFrame);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Filter()));
    }
    pass P2
    {
        //SetRasterizerState(FillMode_WireFrame);
        SetVertexShader(CompileShader(vs_5_0, VS()));
        SetPixelShader(CompileShader(ps_5_0, PS_Address()));
    }
}