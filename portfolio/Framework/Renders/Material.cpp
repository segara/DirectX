#include "Framework.h"
#include "Material.h"


Material::Material()
{
	Initialize();
}

Material::Material(Shader * shader)
{
	Initialize();
	SetShader(shader);
	
}


Material::~Material()
{
	SafeDelete(shader);
	SafeDelete(diffuseMap);
	SafeDelete(specularMap);
	SafeDelete(normalMap);

	SafeDelete(buffer);

	SafeRelease(sBuffer);
	SafeRelease(sDiffuseMap);
	SafeRelease(sSpecularMap);
	SafeRelease(sNormalMap);
}

void Material::Initialize()
{
	name = L"";

	diffuseMap = NULL;
	specularMap = NULL;
	normalMap = NULL;

	buffer = new ConstantBuffer(&colorDesc, sizeof(ColorDesc));
	
}


void Material::SetShader(Shader * shader)
{
	this->shader = shader;

	sBuffer = shader->AsConstantBuffer("CB_Material");

	sDiffuseMap = shader->AsSRV("DiffuseMap");
	sNormalMap = shader->AsSRV("NormalMap");
	sSpecularMap = shader->AsSRV("SpecularMap");
}

void Material::CopyFrom(Material * material)
{
	name = material->name;
	colorDesc = material->colorDesc;

	if (material->shader != NULL)
		SetShader(material->shader);

	if (material->diffuseMap != NULL)
		diffuseMap = new Texture(material->diffuseMap->GetFile());

	if (material->specularMap != NULL)
		specularMap = new Texture(material->specularMap->GetFile());

	if (material->normalMap != NULL)
		normalMap = new Texture(material->normalMap->GetFile());
}

void Material::Ambient(Color & color)
{
	colorDesc.Ambient = color;
}

void Material::Ambient(float r, float g, float b, float a)
{
	Ambient(Color(r, g, b, a));
}

void Material::Diffuse(Color & color)
{
	colorDesc.Diffuse = color;
}

void Material::Diffuse(float r, float g, float b, float a)
{
	Diffuse(Color(r, g, b, a));
}

void Material::Emissive(Color & color)
{
	color = colorDesc.Emissive;
}

void Material::Emissive(float r, float g, float b, float a)
{
	Emissive(Color(r, g, b, a));
}

void Material::Specular(Color & color)
{
	colorDesc.Specular = color;
}

void Material::Specular(float r, float g, float b, float a)
{
	Specular(Color(r, g, b, a));
}

void Material::DiffuseMap(string file)
{
	DiffuseMap(String::ToWString(file));
}

void Material::DiffuseMap(wstring file)
{
	SafeDelete(diffuseMap);
	diffuseMap = new Texture(file);
}

void Material::SpecularMap(string file)
{
	SpecularMap(String::ToWString(file));
}

void Material::SpecularMap(wstring file)
{
	SafeDelete(specularMap);
	specularMap = new Texture(file);
}

void Material::NormalMap(string file)
{
	NormalMap(String::ToWString(file));
}

void Material::NormalMap(wstring file)
{
	SafeDelete(normalMap);
	normalMap = new Texture(file);
}

void Material::Render()
{
	buffer->Render(); //constant buffer 생성시 넣어줬던 구조체를 버퍼에 셋팅
	sBuffer->SetConstantBuffer(buffer->Buffer()); //윗줄의 셋팅된 버퍼를 셰이더의 Constant Buffer에 셋팅
	
	//SetShader 함수에서 설정된 셰이더 변수들에 텍스쳐 셋팅
	if (diffuseMap != NULL)
		sDiffuseMap->SetResource(diffuseMap->SRV());
	else
		sDiffuseMap->SetResource(NULL);
	if(specularMap != NULL)
		sSpecularMap->SetResource(specularMap->SRV());
	else
		sSpecularMap->SetResource(NULL);
	if(normalMap != NULL)
		sNormalMap->SetResource(normalMap->SRV());
	else
		sNormalMap->SetResource(NULL);
}

