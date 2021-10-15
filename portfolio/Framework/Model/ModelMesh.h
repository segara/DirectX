#pragma once


class ModelBone {
public:
	friend class Model;

private:
	ModelBone();
	~ModelBone();


	
private:
	int index;
	wstring name;
	int parentIndex;
	ModelBone* parent;

	Matrix transform;
	vector<ModelBone*> childs;

public:
	int Index() { return index; }
	int ParentIndex() { return parentIndex; }
	ModelBone* Parent() { return parent; }
	wstring Name() { return name; }
	Matrix& Transform() { return transform; }
	void Transform(Matrix& matrix) { transform = matrix; }
	vector<ModelBone*>& Childs() { return childs; }

};

class ModelMesh
{
public:
	friend class Model;
private:
	ModelMesh();
	~ModelMesh();

	void Binding(Model* model);
public :
	void Pass(UINT val) { pass = val; }
	void SetShader(Shader* shader);

	void Update();

	void Render(UINT drawCount);

	wstring Name() { return name; }

	int BoneIndex() { return boneIndex; }
	class ModelBone* Bone(){ return bone; }


	void TransformsSRV(ID3D11ShaderResourceView* srv) { transformsSRV = srv; }
private:
	struct BoneDesc
	{
		//Matrix Transforms[MAX_MODEL_TRANSFORMS];
		UINT index;
		float Padding[3];
	} boneDesc;

private:
	wstring name;

	Shader* shader;
	UINT pass = 0;

	PerFrame* perFrame = NULL;

	wstring materialName = L"";
	Material* material;

	int  boneIndex;
	class ModelBone* bone;

	VertexBuffer* vertexBuffer;
	UINT vertexCount;
	Model::ModelVertex* vertices;

	IndexBuffer* indexBuffer;
	UINT indexCount;
	UINT* indices;

	ConstantBuffer* boneBuffer;
	ID3DX11EffectConstantBuffer* sBoneBuffer;


	ID3D11ShaderResourceView* transformsSRV = NULL;//만들어준 instance transforms 텍스쳐 리소스를 셋팅
	ID3DX11EffectShaderResourceVariable* sTransformsSRV;
};