#include "stdafx.h"
#include "Converter.h"
#include "Types.h"
#include "Utilities/BinaryFile.h"
#include "Utilities/Xml.h"
Converter::Converter()
{
	importer = new Assimp::Importer();
}


Converter::~Converter()
{
	SafeDelete(importer);
}

void Converter::ReadFile(wstring file)
{
	this->file = L"../../_Assets/"+file;
	scene = importer->ReadFile(
		String::ToString(this->file),
		aiProcess_ConvertToLeftHanded
		| aiProcess_Triangulate
		| aiProcess_GenUVCoords
		| aiProcess_GenNormals
		| aiProcess_CalcTangentSpace

	);
	assert(scene != NULL);
}

void Converter::ExportMesh(wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".mesh";
	ReadBoneData(scene->mRootNode, -1, -1);
	ReadSkinData();

	//Write CSV file
	{
		FILE* file;
		fopen_s(&file, "../Vertices.csv", "w");

		for (asBone* bone : bones)
		{
			string name = bone->Name;
			fprintf(file, "%d,%s\n", bone->Index, bone->Name.c_str());

		}

		fprintf(file, "\n");
		for (asMesh* mesh : meshes)
		{
			string name = mesh->Name;
			printf("%s\n", name.c_str());

			for (UINT i = 0; i < mesh->Vertices.size(); i++)
			{
				Vector3 p = mesh->Vertices[i].Position;
				Vector4 indices = mesh->Vertices[i].BlendIndices;
				Vector4 weights = mesh->Vertices[i].BlendWeights;

				fprintf(file, "%f,%f,%f,", p.x, p.y, p.z);
				fprintf(file, "%f,%f,%f,%f,", indices.x, indices.y, indices.z, indices.w);
				fprintf(file, "%f,%f,%f,%f\n", weights.x, weights.y, weights.z, weights.w);
			}
		}
		fclose(file);
	}
	WriteMeshData(savePath);
}

void Converter::ClipList(vector<wstring>* list)
{
	for (UINT i = 0; i < scene->mNumAnimations; i++)
	{
		aiAnimation* anim = scene->mAnimations[i];
		list->push_back(String::ToWString(anim->mName.C_Str()));
	}

}

void Converter::ExportAnimClip(UINT index, wstring savePath)
{
	savePath = L"../../_Models/" + savePath + L".clip";

	//assimp의 animation을 asClip으로 변환 후 저장
	asClip* clip = ReadClipData(scene->mAnimations[index]);
	WriteClipData(clip, savePath);
}



void Converter::ReadKeyframeData(asClip * clip, aiNode * node, vector<struct asClipNode>& aiNodeInfos)
{
	asKeyframe* keyframe = new asKeyframe();
	keyframe->BoneName = node->mName.C_Str();

	for (UINT i = 0; i < clip->FrameCount; i++)
	{
		asClipNode* asClipNode = NULL;
		for (UINT n = 0; n < aiNodeInfos.size(); n++)
		{
			if (aiNodeInfos[n].Name == node->mName) 
			{
				asClipNode = &aiNodeInfos[n];
				break;
			}
		}
		asKeyframeData frameData;
		if (asClipNode == NULL)
		{
			//본과 애니메이션 클립의 본이름이 같은것이 없을때
			Matrix transform(node->mTransformation[0]);
			D3DXMatrixTranspose(&transform, &transform);
			frameData.Time = (float)i;
			D3DXMatrixDecompose(&frameData.Scale, &frameData.Rotation, &frameData.Translation, &transform);
		}
		else {
			frameData = asClipNode->Keyframe[i];
		}
		keyframe->Transforms.push_back(frameData);
	}
	clip->Keyframes.push_back(keyframe);
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		ReadKeyframeData(clip, node->mChildren[i], aiNodeInfos);
	}
}

void Converter::WriteClipData(asClip * clip, wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->String(clip->Name);
	w->Float(clip->Duration);
	w->Float(clip->FrameRate);
	w->UInt(clip->FrameCount);

	w->UInt(clip->Keyframes.size());
	for (asKeyframe* keyframe : clip->Keyframes)
	{
		w->String(keyframe->BoneName);
		w->UInt(keyframe->Transforms.size());
		w->Byte(&keyframe->Transforms[0], sizeof(asKeyframeData)* keyframe->Transforms.size());

		SafeDelete(keyframe);
	}
	w->Close();
	SafeDelete(w);
}

void Converter::ExportMaterial(wstring savePath, bool bOverwrite)
{
	savePath = L"../../_Textures/" + savePath + L".material";
	if (bOverwrite == false)
	{
		if (Path::ExistFile(savePath) == true)
			return;
	}
	
	ReadMaterialData();
	WriteMaterialData(savePath);
}

void Converter::ReadMaterialData()
{
	for (UINT i = 0; i < scene->mNumMaterials; ++i) {
		aiMaterial* srcMaterial = scene->mMaterials[i];
		asMaterial* material = new asMaterial();

		material->Name = srcMaterial->GetName().C_Str();

		aiColor3D color;

		srcMaterial->Get(AI_MATKEY_COLOR_AMBIENT, color);
		material->Ambient = Color(color.r, color.g, color.b, 1.0f);
		
		srcMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, color);
		material->Diffuse = Color(color.r, color.g, color.b, 1.0f);
		
		srcMaterial->Get(AI_MATKEY_COLOR_SPECULAR, color);
		material->Specular = Color(color.r, color.g, color.b, 1.0f);

		srcMaterial->Get(AI_MATKEY_SHININESS, material->Specular.a);

		srcMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, material->Specular.a);
		material->Emissive = Color(color.r, color.g, color.b, 1.0f);

		aiString file;

		srcMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &file);
		material->DiffuseFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_SPECULAR, 0, &file);
		material->SpecularFile = file.C_Str();

		srcMaterial->GetTexture(aiTextureType_NORMALS, 0, &file);
		material->NormalFile = file.C_Str();

		materials.push_back(material);
	}
}

void Converter::WriteMaterialData(wstring savePath)
{
	string folder = String::ToString(Path::GetDirectoryName(savePath));
	string file = String::ToString(Path::GetFileName(savePath));

	Path::CreateFolders(folder);

	Xml::XMLDocument* document = new Xml::XMLDocument();
	Xml::XMLDeclaration* declare = document->NewDeclaration();
	document->LinkEndChild(declare);

	Xml::XMLElement* root = document->NewElement("Materials");
	root->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
	root->SetAttribute("xmlns:xsd", "http://www.w3.org/2001/XMLSchema");
	document->LinkEndChild(root);

	for (asMaterial* material : materials)
	{
		Xml::XMLElement* node = document->NewElement("Material");
		root->LinkEndChild(node);

		Xml::XMLElement* element = NULL;

		element = document->NewElement("Name");
		element->SetText(material->Name.c_str());
		node->LinkEndChild(element);

		element = document->NewElement("DiffuseFile");
		element->SetText(WriteTexture(folder, material->DiffuseFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("SpecularFile");
		element->SetText(WriteTexture(folder, material->SpecularFile).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("NormalFile");
		element->SetText(WriteTexture(folder, material->NormalFile ).c_str());
		node->LinkEndChild(element);

		element = document->NewElement("Ambient");
		element->SetAttribute("R", material->Ambient.r);
		element->SetAttribute("G", material->Ambient.g);
		element->SetAttribute("B", material->Ambient.b);
		element->SetAttribute("A", material->Ambient.a);
		node->LinkEndChild(element);

		element = document->NewElement("Diffuse");
		element->SetAttribute("R", material->Diffuse.r);
		element->SetAttribute("G", material->Diffuse.g);
		element->SetAttribute("B", material->Diffuse.b);
		element->SetAttribute("A", material->Diffuse.a);
		node->LinkEndChild(element);

		element = document->NewElement("Specular");
		element->SetAttribute("R", material->Specular.r);
		element->SetAttribute("G", material->Specular.g);
		element->SetAttribute("B", material->Specular.b);
		element->SetAttribute("A", material->Specular.a);
		node->LinkEndChild(element);

		element = document->NewElement("Emissive");
		element->SetAttribute("R", material->Emissive.r);
		element->SetAttribute("G", material->Emissive.g);
		element->SetAttribute("B", material->Emissive.b);
		element->SetAttribute("A", material->Emissive.a);
		node->LinkEndChild(element);

		SafeDelete(material);

	}
	document->SaveFile((folder + file).c_str());
	SafeDelete(document);

}

string Converter::WriteTexture(string saveFolder, string file)
{
	if (file.length() < 1) return "";
	string fileName = Path::GetFileName(file);
	const aiTexture* texture = scene->GetEmbeddedTexture(file.c_str()); //fbx에 내장된 텍스쳐
	string path = "";
	if (texture != NULL)
	{
		path = saveFolder + fileName;
		if (texture->mHeight < 1)
		{
			//데이터가 한줄로 바이트파일(png) 형식으로 써져있다.즉 그대로 저장하면 이미지 파일이 됨
			BinaryWriter w;
			w.Open(String::ToWString(path));
			w.Byte(texture->pcData, texture->mWidth);
			w.Close();
		}
		else 
		{
			//실제 텍스쳐를 읽어서 저장(잘 안쓰임)
			D3D11_TEXTURE2D_DESC destDesc;
			ZeroMemory(&destDesc, sizeof(D3D11_TEXTURE2D_DESC));
			destDesc.Width = texture->mWidth;
			destDesc.Height = texture->mHeight;
			destDesc.MipLevels = 1;
			destDesc.ArraySize = 1;
			destDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			destDesc.SampleDesc.Count = 1;
			destDesc.SampleDesc.Quality = 0;
			destDesc.Usage = D3D11_USAGE_IMMUTABLE;

			D3D11_SUBRESOURCE_DATA subResource = { 0 };
			subResource.pSysMem = texture->pcData;


			ID3D11Texture2D* dest;

			HRESULT hr;
			hr = D3D::GetDevice()->CreateTexture2D(&destDesc, &subResource, &dest);
			assert(SUCCEEDED(hr));

			D3DX11SaveTextureToFileA(D3D::GetDC(), dest, D3DX11_IFF_PNG, saveFolder.c_str());
		}
	}
	else //텍스쳐 파일이 fbx와 따로 있는경우 : Texture폴더에 다시 저장
	{
		string directory = Path::GetDirectoryName(String::ToString(this->file));
		string origin = directory + file;
		String::Replace(&origin, "\\", "/");

		if (Path::ExistFile(origin) == false)
			return "";

		path = saveFolder + fileName;
		CopyFileA(origin.c_str(), path.c_str(), FALSE);

		String::Replace(&path, "../../_Textures/", "");
	}
	
	//텍스쳐 파일 정보가 비어있는 경우도 있다
	//그럴때는 텍스쳐 파일 정보를 xml에 직접 기입하고 텍스처도 따로 복사해서 넣어줌
	//이후에는 xml파일을 재생성 하지 않도록 ExportMaterial - overwrite:false 처리해줌
	return Path::GetFileName(path);
}

void Converter::ReadBoneData(aiNode * node, int index, int parent)
{
	//본정보 읽기
	asBone* bone = new asBone();
	bone->Index = index;
	bone->Parent = parent;
	bone->Name = node->mName.C_Str();

	//float* test_input_value = node->mTransformation[0];
	
	Matrix transform(node->mTransformation[0]); //0번째원소부터 16개를 읽음

	D3DXMatrixTranspose(&bone->Transform, &transform);
	//전치행렬 변환:directx는 반대여서
	
	Matrix matrixParent;
	if (parent < 0)
	{
		D3DXMatrixIdentity(&matrixParent);
	}
	else
	{
		matrixParent = bones[parent]->Transform;
	}
	//부모본의 matrix와 현재 자식본의 matrix를 곱해줌
	bone->Transform = bone->Transform * matrixParent;
	bones.push_back(bone);
	
	//메시 정보 읽기

	ReadMeshData(node, index);

	for (UINT i = 0; i < node->mNumChildren; ++i)
	{
		ReadBoneData(node->mChildren[i], bones.size(), index);
	}
}

void Converter::ReadMeshData(aiNode * node, int bone)
{
	if (node->mNumMeshes < 1) return;

	asMesh* mesh = new asMesh();
	mesh->Name = node->mName.C_Str();
	mesh->BoneIndex = bone;

	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		UINT index = node->mMeshes[i];
		aiMesh* srcMesh = scene->mMeshes[index];

		aiMaterial* material = scene->mMaterials[srcMesh->mMaterialIndex];
		mesh->MaterialName = material->GetName().C_Str();

		UINT startVertex = mesh->Vertices.size();
		for (UINT v = 0; v < srcMesh->mNumVertices; v++)
		{
			Model::ModelVertex vertex;
			memcpy(&vertex.Position, &srcMesh->mVertices[v], sizeof(Vector3));

			if (srcMesh->HasTextureCoords(0))
			{
				memcpy(&vertex.Uv, &srcMesh->mTextureCoords[0][v], sizeof(Vector2));
			}

			if (srcMesh->HasNormals())
			{
				memcpy(&vertex.Normal, &srcMesh->mNormals[v], sizeof(Vector3));
			}

			if (srcMesh->HasTangentsAndBitangents())
			{
				memcpy(&vertex.Tangent, &srcMesh->mTangents[v], sizeof(Vector3));
			}

			mesh->Vertices.push_back(vertex);
			
		}
		//face:(면데이터 , index 처리)
		for (UINT f = 0; f < srcMesh->mNumFaces; f++)
		{
			aiFace& face = srcMesh->mFaces[f];
			for (UINT k = 0; k < face.mNumIndices; k++)
			{
				mesh->Indices.push_back(face.mIndices[k] + startVertex);
			}
		}
		meshes.push_back(mesh);
	}
}

void Converter::ReadSkinData()
{
	// 메쉬의 버텍스 / 메쉬에 연결된 본을 토대로
	
	for (UINT i = 0; i < scene->mNumMeshes; ++i)
	{
		aiMesh* aiMesh = scene->mMeshes[i];
		if (aiMesh->HasBones() == false) continue;

		asMesh* mesh = meshes[i];

		vector<asBoneWeights> boneWeights;
		boneWeights.assign(mesh->Vertices.size(), asBoneWeights());

		for (UINT b = 0; b < aiMesh->mNumBones; b++)
		{
			//aiBone : assimp bone
			//asBone : Types 에서 정의한 본
			
			aiBone* aiMeshBone = aiMesh->mBones[b];
			UINT boneIndex = 0;
			//asBone 의 인덱스를 알기 위해 index를 검색
			for (asBone* bone : bones)
			{
				if (bone->Name == (string)aiMeshBone->mName.C_Str())
				{
					boneIndex = bone->Index;
					break;
				}
			}
			
			for (UINT w = 0; w < aiMeshBone->mNumWeights; w++)
			{
				//본 당 영향을 미치는 버텍스 / weight 값
				UINT index = aiMeshBone->mWeights[w].mVertexId;
				float weight = aiMeshBone->mWeights[w].mWeight;

				//boneWeight vector에 값을 넣어줌 
				//boneWeight vector는 버텍스를 기준으로 버텍스에 영향을 주는 본과 그 값을 넣어줄것임
				//버텍스당 웨이트 총합은 1이 될것임
				boneWeights[index].AddWeights(boneIndex, weight);
				//boneWeight[index].Normalize();
			}
		}

		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			boneWeights[w].Normalize();
		}

		for (UINT w = 0; w < boneWeights.size(); w++)
		{
			//boneWeights[i].Normalize();
			asBlendWeight blendWeight;
			boneWeights[w].GetBlendWeights(blendWeight);
			//boneWeight에 버텍스 인덱스를 인덱스로 값을 넣어주었기 때문에 mesh->Vertices[w]
			mesh->Vertices[w].BlendIndices = blendWeight.Indices;
			mesh->Vertices[w].BlendWeights = blendWeight.Weights;
		}
		
	}
}

void Converter::WriteMeshData(wstring savePath)
{
	Path::CreateFolders(Path::GetDirectoryName(savePath));

	BinaryWriter* w = new BinaryWriter();
	w->Open(savePath);

	w->UInt(bones.size());
	for (asBone* bone : bones)
	{
		w->Int(bone->Index);
		w->String(bone->Name);
		w->Int(bone->Parent);
		w->Matrix(bone->Transform);

		SafeDelete(bone);
	}

	w->UInt(meshes.size());
	for (asMesh* meshData : meshes)
	{
		w->String(meshData->Name);
		w->Int(meshData->BoneIndex);

		w->String(meshData->MaterialName);

		w->UInt(meshData->Vertices.size());
		w->Byte(&meshData->Vertices[0], sizeof(Model::ModelVertex)* meshData->Vertices.size());

		w->UInt(meshData->Indices.size());
		w->Byte(&meshData->Indices[0], sizeof(UINT) * meshData->Indices.size());

		SafeDelete(meshData);

	}
	w->Close();
	SafeDelete(w);
}
asClip * Converter::ReadClipData(aiAnimation * animation)
{
	asClip* clip = new asClip();
	clip->Name = animation->mName.C_Str();
	clip->FrameRate = (float)animation->mTicksPerSecond;
	clip->FrameCount = (UINT)animation->mDuration + 1;

	vector<asClipNode> aniClipNodeInfos;
	for (UINT i = 0; i < animation->mNumChannels; i++) //
	{
		aiNodeAnim* aniNode = animation->mChannels[i];

		asClipNode aniClipNodeInfo;
		aniClipNodeInfo.Name = aniNode->mNodeName;

		//가장 많은 키카운트값으로 작업
		UINT keyCount = max(aniNode->mNumPositionKeys, aniNode->mNumScalingKeys);
		keyCount = max(keyCount, aniNode->mNumRotationKeys);

		for (UINT k = 0; k < keyCount; k++) {
			asKeyframeData frameData;

			bool bFound = false;
			UINT t = aniClipNodeInfo.Keyframe.size();

			//fabsf : 절대값
			//D3DX_16F_EPSILON : 2byte float의 오차값
			if (fabsf((float)aniNode->mPositionKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mPositionKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Translation, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));
				bFound = true;
			}
			if (fabsf((float)aniNode->mRotationKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				//쿼터니온
				aiQuatKey key = aniNode->mRotationKeys[k];
				frameData.Time = (float)key.mTime;

				frameData.Rotation.x = key.mValue.x;
				frameData.Rotation.y = key.mValue.y;
				frameData.Rotation.z = key.mValue.z;
				frameData.Rotation.w = key.mValue.w;

				bFound = true;
			}
			if (fabsf((float)aniNode->mScalingKeys[k].mTime - (float)t) <= D3DX_16F_EPSILON)
			{
				aiVectorKey key = aniNode->mScalingKeys[k];
				frameData.Time = (float)key.mTime;
				memcpy_s(&frameData.Scale, sizeof(Vector3), &key.mValue, sizeof(aiVector3D));
				bFound = true;
			}
			if (bFound == true)
			{
				aniClipNodeInfo.Keyframe.push_back(frameData);
			}
		}

		//빈 부분을 쭉 복사
		if (aniClipNodeInfo.Keyframe.size() < clip->FrameCount)
		{
			UINT count = clip->FrameCount - aniClipNodeInfo.Keyframe.size();
			asKeyframeData keyFrame = aniClipNodeInfo.Keyframe.back();

			for (UINT n = 0; n < count; n++)
				aniClipNodeInfo.Keyframe.push_back(keyFrame);
		}
		clip->Duration = max(clip->Duration, aniClipNodeInfo.Keyframe.back().Time);
		aniClipNodeInfos.push_back(aniClipNodeInfo);
	}
	ReadKeyframeData(clip, scene->mRootNode, aniClipNodeInfos);
	return clip;
}