#pragma once
#define MAX_MODEL_TRANSFORMS 250 //사람의 뼈가 250개를 넘지 않으므로
#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500
class ModelBone;
class ModelMesh;
class ModelClip;

class Model
{
public:
	typedef VertexTextureNormalTangentBlend ModelVertex;

private:
	ModelBone* root;
	vector<ModelBone*> bones;
	vector<ModelMesh*> meshes;
	vector<Material*> materials;
	vector<ModelClip*> clips;
public:
	Model();
	~Model();

public:
	void ReadMesh(wstring file);
	void ReadMaterial(wstring file);
	void ReadClip(wstring file);
private:
	void BindBone();
	void BindMesh();

public :
	UINT BoneCount() { return bones.size(); }
	//vector<ModelBone *>& : Returning values by reference
	vector<ModelBone *>& Bones() { return bones; }
	ModelBone* BoneByIndex(UINT index) { return bones[index]; }
	ModelBone* BoneByName(wstring name);

	UINT MeshCount() { return meshes.size(); }
	vector<ModelMesh*>& Meshes() { return meshes; }
	ModelMesh* MeshByIndex(UINT index) { return meshes[index]; }
	ModelMesh* MeshByName(wstring name);

	UINT MaterialCount() { return materials.size(); }
	//vector<Material *>& : Returning values by reference
	vector<Material *>& Materials() { return materials; }
	Material* MaterialByIndex(UINT index) { return materials[index]; }
	Material* MaterialByName(wstring name);

	ModelClip* ClipByIndex(UINT index) { return clips[index]; }
	ModelClip* ClipByName(wstring name);
	UINT ClipCount() { return clips.size(); }
};

