#pragma once

// ModelRender : 하나의fbx가 로딩될때 생성하는 Object
// 멤버인 Model에서 데이터 로드 처리
// ModelMesh : 실제메쉬를 렌더링하는 클래스
// 렌더링은 셰이더 단위로 이루어진다. 즉 material 클래스에서는 현재 쓰고있는 셰이더에 각종 리소스를 밀어주는 역할
// 
//  ModelRender->ReadMesh()
//	
//	로드 순서(in Model Class)
//
//	ReadMesh
//		-본데이터 read
//		-메시 데이터 read
//			-BindBone : parent 설정
//			-ReadMaterial
//				-BindMesh : mesh에 본셋팅
//					-ModelMesh::Binding : material 생성
class ModelRender
{
public:
	ModelRender(Shader* shader);
	~ModelRender();

public :
	void Update();
	void Render();
public :
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	Model* GetModel() { return model; }

	Transform* AddTransform();
	Transform* GetTransform(UINT index) { return transforms[index]; }
	void UpdateTransforms();//전체 정보를 gpu로 보냄

	void Pass(UINT pass);

	void UpdateTransform(UINT instanceId, UINT boneIndex, Transform& transform); //본정보를 계산
private :
	void CreateTexture();
private:

	Shader* shader;
	Model* model;

	vector<Transform *> transforms;
	Matrix worlds[MAX_MODEL_INSTANCE];

	VertexBuffer* instanceBuffer;

	Matrix boneTransforms[MAX_MODEL_INSTANCE][MAX_MODEL_TRANSFORMS];

	ID3D11Texture2D* texture = NULL;
	ID3D11ShaderResourceView* srv;
};

