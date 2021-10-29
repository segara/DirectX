#include "00_Global.fx"
#include "00_Light.fx"
#include "00_Render.fx"


float4 PS(MeshOutput input) : SV_Target
{
    Texture(Material.Diffuse, DiffuseMap, input.Uv);
    Texture(Material.Specular, SpecularMap, input.Uv);
    
    MaterialDesc outputDesc = MakeMaterial();
    MaterialDesc result = MakeMaterial();
    
    ComputeLight(outputDesc, input.Normal, input.wPosition);
    AddMaterial(result, outputDesc);
    
    ComputePointLights(outputDesc, input.Normal, input.wPosition); //outputDesc은 내부에서 초기화됨
    AddMaterial(result, outputDesc);
    
    ComputeSpotLights(outputDesc, input.Normal, input.wPosition); //outputDesc은 내부에서 초기화됨
    AddMaterial(result, outputDesc);
    
    return float4(MaterialToColor(result), 1.0f);
}

technique11 T0
{
    P_VP(P0, VS_Mesh, PS)
    P_VP(P1, VS_Model, PS)
    P_VP(P2, VS_Animation, PS)
   
}