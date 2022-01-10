#include "Framework.h"
#include "Buffer.h"


VertexBuffer::VertexBuffer(void* data,
	UINT count,
	UINT stride,
	UINT slot,
	bool bCpuWrite, 
	bool bGpuWrite)
	:data(data), count(count), stride(stride), slot(slot),bCpuWrite(bCpuWrite), bGpuWrite(bGpuWrite)
{
	//desc 설정
	D3D11_BUFFER_DESC desc;
	//zero memory
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = stride* count;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	if (bCpuWrite == false && bGpuWrite == false)
	{
		desc.Usage = D3D11_USAGE_IMMUTABLE; // gpu읽기만 가능 / 최초생성시 입력한 데이터를 수정할 수 없음 : 일반적인 모델 / 속도빠름
	}
	else if (bCpuWrite == true && bGpuWrite ==false)
	{
		desc.Usage = D3D11_USAGE_DYNAMIC; // cpu쓰기, gpu읽기 :지형등의 편집
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if (bCpuWrite == false && bGpuWrite == true)
	{
		desc.Usage = D3D11_USAGE_DEFAULT; // cpu쓰기불가 이지만 updatesubresource 로 열어줘서 가능
	}
	else 
	{
		desc.Usage = D3D11_USAGE_STAGING; //가장 느림
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE | D3D11_CPU_ACCESS_READ;
	}

	//sub resource (vertice data) setting
	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = data;

	Check(D3D::GetDevice()->CreateBuffer(&desc, &subResource, &buffer));
}


VertexBuffer::~VertexBuffer()
{
	SafeRelease(buffer);
}

void VertexBuffer::Render()
{
	//IA 설정 : 하드웨어에 만들어준 버퍼를 셋팅
	UINT offset = 0;


	D3D::GetDC()->IASetVertexBuffers(slot, 1, &buffer, &stride, &offset);

	D3D::GetDC()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

IndexBuffer::IndexBuffer(void * indices, UINT count)
	:indices(indices), count(count)
{

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = sizeof(UINT) * count;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA data = { 0 };
	data.pSysMem = indices;
	Check(D3D::GetDevice()->CreateBuffer(&desc, &data, &buffer));
}

IndexBuffer::~IndexBuffer()
{
	SafeRelease(buffer);
}

void IndexBuffer::Render()
{
	D3D::GetDC()->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

//ConstantBuffer::ConstantBuffer()
//{
//	D3D11_BUFFER_DESC desc;
//	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
//	desc.ByteWidth = dataSize;
//	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
//	desc.Usage = D3D11_USAGE_DYNAMIC; // cpu쓰기, gpu읽기 :지형등의 편집
//	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
//
//	//초기 데이터가 없어서 subresource setting 안함
//	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));
//}

ConstantBuffer::ConstantBuffer(void * data, UINT dataSize)
	:data(data), dataSize(dataSize)
{
	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));
	desc.ByteWidth = dataSize;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.Usage = D3D11_USAGE_DYNAMIC; // cpu쓰기, gpu읽기 
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//초기 데이터가 없어서 subresource setting 안함
	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));
}

ConstantBuffer::~ConstantBuffer()
{
	SafeRelease(buffer);
}

void ConstantBuffer::Render()
{
	D3D11_MAPPED_SUBRESOURCE subresource;
	//map : write
	//데이터의 업데이트
	//buffer(gpu) 시작주소 를  subresource.pData 로 넣어줌
	//위의 주소에 data를 밀어넣음
	//이후 작업이 완료된 해당 buffer를 쉐이더에 연결시킴
	//sBuffer = shader->AsConstantBuffer("CB_Projector"); <= 초기화시 미리연결
		
	//sBuffer->SetConstantBuffer(buffer->Buffer());
	//이후 쉐이더에서는 해당정보를 통해 계산에 활용

	D3D::GetDC()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);

	{
		memcpy(subresource.pData, data, dataSize);  
	}

	D3D::GetDC()->Unmap(buffer, 0);
}
//////////////////////////////////////////////////////////////////////
CsResource::CsResource()
{

}

CsResource::~CsResource()
{
	SafeRelease(input);
	SafeRelease(srv);

	SafeRelease(output);
	SafeRelease(uav);

	SafeRelease(result);
}

void CsResource::CreateBuffer()
{
	CreateInput();
	CreateSRV();

	CreateOutput();
	CreateUAV();

	CreateResult();
}
////////////////////////////////////////////////////
RawBuffer::RawBuffer(void * inputData, UINT inputByte, UINT outputByte)
	: CsResource()
	, inputData(inputData), inputByte(inputByte), outputByte(outputByte)
{
	CreateBuffer();
}

RawBuffer::~RawBuffer()
{

}

void RawBuffer::CreateInput()
{
	if (inputByte < 1) return;


	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = inputByte;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
	//바이트 어드레스 버퍼로 사용하겠다고 명시, 즉 rawbuffer
	desc.Usage = D3D11_USAGE_DYNAMIC; //cpu씀, gpu읽음
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE; 

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = inputData;

	Check(D3D::GetDevice()->CreateBuffer(&desc, inputData != NULL ? &subResource : NULL, &buffer));

	input = (ID3D11Resource *)buffer;
}

void RawBuffer::CreateSRV()
{
	if (inputByte < 1) return;


	ID3D11Buffer* buffer = (ID3D11Buffer *)input;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_R32_TYPELESS; 
	//32bit -> 4byte 만 사용하겠다고 명시
	//왜냐하면 gpu에서는 float(4byte)만 사용하므로 기본 단위가 4byte이다
	//_TYPELESS : 넘기는 데이터 형식을 모르므로
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.Flags = D3D11_BUFFEREX_SRV_FLAG_RAW;
	//D3D11_BUFFEREX_SRV_FLAG_RAW : raw data로 다룰것임을 명시
	srvDesc.BufferEx.NumElements = desc.ByteWidth / 4;
	//전체 데이터의 갯수

	Check(D3D::GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &srv));
 }

void RawBuffer::CreateOutput()
{
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = outputByte;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;

	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));

	output = (ID3D11Resource *)buffer;
}

void RawBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer *)output;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);


	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_RAW;
	uavDesc.Buffer.NumElements = desc.ByteWidth / 4;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav));
}

void RawBuffer::CreateResult()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc;
	((ID3D11Buffer *)output)->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0; //default
	desc.MiscFlags = 0;

	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));
	result = (ID3D11Resource *)buffer;
}


void RawBuffer::CopyToInput(void * data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(input, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		//data가 gpu 메모리이므로 이것을 subResource.pData 에 씀
		memcpy(subResource.pData, data, inputByte);
	}
	D3D::GetDC()->Unmap(input, 0);
}

void RawBuffer::CopyFromOuput(void * data)
{
	//받아온 output을 result에복사
	D3D::GetDC()->CopyResource(result, output);
	
	D3D11_MAPPED_SUBRESOURCE subResource;
	//result를 subResource로 맵핑하면서 (락)
	D3D::GetDC()->Map(result, 0, D3D11_MAP_READ, 0, &subResource);
	{
		// subResource.pData(result) 를  memory(data)에 씀
		memcpy(data, subResource.pData, outputByte);
	}
	D3D::GetDC()->Unmap(result, 0);
}

///////////////////////////////////////////////////////////////////////////////


TextureBuffer::TextureBuffer(ID3D11Texture2D * src)
{
	D3D11_TEXTURE2D_DESC srcDesc;
	src->GetDesc(&srcDesc);

	width = srcDesc.Width;
	height = srcDesc.Height;
	arraySize = srcDesc.ArraySize;
	format = srcDesc.Format;


	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;

	ID3D11Texture2D* texture = NULL;
	Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));
	D3D::GetDC()->CopyResource(texture, src);

	input = (ID3D11Resource *)texture;

	CreateBuffer();
}

TextureBuffer::~TextureBuffer()
{

}

void TextureBuffer::CreateSRV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D *)input;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = arraySize;

	Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &srv));
}

void TextureBuffer::CreateOutput()
{
	ID3D11Texture2D* texture = NULL;

	D3D11_TEXTURE2D_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = arraySize;
	desc.Format = format;
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	desc.MipLevels = 1;
	desc.SampleDesc.Count = 1;
	Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));

	output = (ID3D11Resource *)texture;


	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = desc.Format;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	srvDesc.Texture2DArray.MipLevels = 1;
	srvDesc.Texture2DArray.ArraySize = arraySize;

	Check(D3D::GetDevice()->CreateShaderResourceView(texture, &srvDesc, &outputSRV));
}

void TextureBuffer::CreateUAV()
{
	ID3D11Texture2D* texture = (ID3D11Texture2D*)output;

	D3D11_TEXTURE2D_DESC desc;
	texture->GetDesc(&desc);


	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2DARRAY;
	uavDesc.Texture2DArray.ArraySize = arraySize;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(texture, &uavDesc, &uav));
}

void TextureBuffer::CreateResult()
{
	ID3D11Texture2D* texture;

	D3D11_TEXTURE2D_DESC desc;
	((ID3D11Texture2D *)output)->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));

	result = (ID3D11Resource *)texture;
}

void TextureBuffer::CopyToInput(ID3D11Texture2D * texture)
{
	D3D::GetDC()->CopyResource(input, texture);
}

ID3D11Texture2D * TextureBuffer::CopyFromOutput()
{
	D3D::GetDC()->CopyResource(result, output);

	return (ID3D11Texture2D *)result;
}

///////////////////////////////////////////////////////////////////////////////

StructuredBuffer::StructuredBuffer(void * inputData, UINT inputStride, UINT inputCount, UINT outputStride, UINT outputCount)
	:CsResource(),
	inputData(inputData),
	inputStride(inputStride),
	inputCount(inputCount),
	outputStride(outputStride),
	outputCount(outputCount)
{
	if (outputStride == 0 || outputCount == 0)
	{
		this->outputStride = inputStride;
		this->outputCount = inputCount;
	}

	CreateBuffer();
}

StructuredBuffer::~StructuredBuffer()
{

}

void StructuredBuffer::CreateInput()
{
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = InputByteWidth();
	desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = inputStride;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA subResource = { 0 };
	subResource.pSysMem = inputData;

	Check(D3D::GetDevice()->CreateBuffer(&desc, inputData != NULL ? &subResource : NULL, &buffer));

	input = (ID3D11Resource*)buffer;
}

void StructuredBuffer::CreateSRV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer *)input;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	ZeroMemory(&srvDesc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.NumElements = inputCount;

	Check(D3D::GetDevice()->CreateShaderResourceView(buffer, &srvDesc, &srv));
}

void StructuredBuffer::CreateOutput()
{
	ID3D11Buffer* buffer = NULL;

	D3D11_BUFFER_DESC desc;
	ZeroMemory(&desc, sizeof(D3D11_BUFFER_DESC));

	desc.ByteWidth = OutputByteWidth();
	desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS;
	desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
	desc.StructureByteStride = outputStride;

	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));

	output = (ID3D11Resource*)buffer;
}

void StructuredBuffer::CreateUAV()
{
	ID3D11Buffer* buffer = (ID3D11Buffer*)output;

	D3D11_BUFFER_DESC desc;
	buffer->GetDesc(&desc);

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
	ZeroMemory(&uavDesc, sizeof(D3D11_UNORDERED_ACCESS_VIEW_DESC));
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.NumElements = outputCount;

	Check(D3D::GetDevice()->CreateUnorderedAccessView(buffer, &uavDesc, &uav));

}

void StructuredBuffer::CreateResult()
{
	ID3D11Buffer* buffer;

	D3D11_BUFFER_DESC desc;
	((ID3D11Buffer *)output)->GetDesc(&desc);
	desc.Usage = D3D11_USAGE_STAGING;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	desc.BindFlags = 0;
	desc.MiscFlags = 0;

	Check(D3D::GetDevice()->CreateBuffer(&desc, NULL, &buffer));

	result = (ID3D11Resource *)buffer;
}

void StructuredBuffer::CopyToInput(void * data)
{
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(input, 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, data, InputByteWidth());
	}
	D3D::GetDC()->Unmap(input, 0);
}

void StructuredBuffer::CopyFromOutput(void * data)
{
	D3D::GetDC()->CopyResource(result, output);

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(result, 0, D3D11_MAP_READ, 0, &subResource);
	{
		memcpy(data, subResource.pData, OutputByteWidth());
	}
	D3D::GetDC()->Unmap(result, 0);
}
