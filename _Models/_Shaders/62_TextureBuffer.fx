Texture2DArray<float4> Input;
RWTexture2DArray<float4> Output;

[numthreads(32,32,1)]
void CS(uint3 id : SV_DispatchThreadID)
{
    float4 color = Input.Load(int4(id, 0)); //w?? 0
    
    Output[id] = 1.0f - color; 
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}
