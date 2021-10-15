#pragma once
class VertexBuffer
{
public:
	VertexBuffer(void* data, UINT count, UINT stride, UINT slot =0,bool bCpuWrite = false, bool bGpuWrite = false);
	~VertexBuffer();

public:
	void Render();
	UINT Count() { return count; }
	UINT Stride() { return stride; }
	ID3D11Buffer* Buffer() { return buffer; }
private:
	ID3D11Buffer* buffer;

	void* data; //��������
	UINT count;
	UINT stride;//�����ϳ��� ũ��
	UINT slot;

	bool bCpuWrite;
	bool bGpuWrite;

};

class IndexBuffer
{
public:
	IndexBuffer(void* data, UINT count);
	~IndexBuffer();

public:
	void Render();
	UINT Count() { return count; }
	ID3D11Buffer* Buffer() { return buffer; }
private:
	ID3D11Buffer* buffer;

	void* indices; //��������
	UINT count;

};

class ConstantBuffer
{
public :
	//ConstantBuffer();
	ConstantBuffer(void* data,UINT dataSize);
	~ConstantBuffer();
public:
	ID3D11Buffer* Buffer() { return buffer; }
	void Render();
private:
	ID3D11Buffer* buffer;
	void* data;
	UINT dataSize;
};

//////////////Compute Shader///////////////

class CsResource
{
public:
	CsResource();
	virtual ~CsResource();
protected:
	virtual void CreateInput() {}
	virtual void CreateSRV() {}

	virtual void CreateOutput() {}
	virtual void CreateUAV() {}

	virtual void CreateResult() {}

	void CreateBuffer();

public:
	ID3D11ShaderResourceView* SRV() { return srv; }
	ID3D11UnorderedAccessView* UAV() { return uav; }
protected: 
	ID3D11Resource* input = NULL;
	ID3D11ShaderResourceView* srv = NULL;//input

	ID3D11Resource* output = NULL;
	ID3D11UnorderedAccessView* uav = NULL; //Output : uav unorderd access view

	ID3D11Resource* result = NULL; //gpu�� ������� ������ �� ���� ������ �����ؼ� ���
};
////////////////////////////////////////////////////////

class RawBuffer : public CsResource
{
public:
	RawBuffer(void* inputData, UINT inputByte, UINT outputByte);
	~RawBuffer();
private:
	void CreateInput() override;
	void CreateSRV() override;

	void CreateOutput() override;
	void CreateUAV() override;

	void CreateResult() override;
public :
	void CopyToInput(void* data);
	void CopyFromOuput(void* data);
private:
	void* inputData; //shader�� �ѱ�� ������
	UINT inputByte;  //�󸶸�ŭ ���� ����
	UINT outputByte;

};
//////////////////////////////////////////////////////////

class TextureBuffer : public CsResource
{
public:
	TextureBuffer(ID3D11Texture2D* src);
	~TextureBuffer();

private:
	void CreateSRV() override;

	void CreateOutput() override;
	void CreateUAV() override;

	void CreateResult() override;

public:
	UINT Width() { return width; }
	UINT Height() { return height; }
	UINT ArraySize() { return arraySize; }

	ID3D11Texture2D* Output() { return (ID3D11Texture2D *)output; }
	ID3D11ShaderResourceView* OutputSRV() { return outputSRV; }

	void CopyToInput(ID3D11Texture2D* texture);
	ID3D11Texture2D* CopyFromOutput();
	ID3D11Texture2D* Result() { return (ID3D11Texture2D*)result; }
private:
	UINT width, height, arraySize;
	DXGI_FORMAT format;

	ID3D11ShaderResourceView* outputSRV;
};
///////////////////////////////////////////////////////////////////

class StructuredBuffer : public CsResource
{
public:
	//inputStride : ����ü�ϳ��� ũ��
	StructuredBuffer(void* inputData, UINT inputStride, UINT inputCount, UINT outputStride = 0, UINT outputCount = 0);
	~StructuredBuffer();

private:
	void CreateInput() override;
	void CreateSRV() override;
	void CreateOutput() override;
	void CreateUAV() override;

	void CreateResult() override;

public :
	UINT InputByteWidth() { return inputStride * inputCount; }
	UINT OutputByteWidth() { return outputStride * outputCount; }

	void CopyToInput(void* data);
	void CopyFromOutput(void* data);

private:
	void* inputData;

	UINT inputStride;
	UINT inputCount;

	UINT outputStride;
	UINT outputCount;

};