#include "Framework.h"
#include "ModelAnimator.h"

ModelAnimator::ModelAnimator()
{
}

ModelAnimator::ModelAnimator(Shader* shader) :shader(shader)
{
	model = new Model();

	tweenBuffer = new ConstantBuffer(&tweenDesc, sizeof(TweenDesc) * MAX_MODEL_INSTANCE);
	sTweenBuffer = shader->AsConstantBuffer("CB_TweenFrame");

	blendBuffer = new ConstantBuffer(&blendDesc, sizeof(BlendDesc)* MAX_MODEL_INSTANCE);
	sBlendBuffer = shader->AsConstantBuffer("CB_BlendFrame");

	//instancing
	instanceBuffer = new VertexBuffer(worlds, MAX_MODEL_INSTANCE, sizeof(Matrix), 1, true);

	for (UINT i = 0; i < MAX_MODEL_INSTANCE; i++)
	{
		colors[i] = Color(1, 1, 1, 1);
		colors[i] = Math::RandomColor3();
		colors[i].a = 1.0f;
	}

	instanceBuffer_Color = new VertexBuffer(colors, MAX_MODEL_INSTANCE, sizeof(Color), 2, true);

	//Create Comput Shader
	{
		computeShader = new Shader(L"71_GetMultiBones.fx");
		
		inputWorldBuffer = new StructuredBuffer(worlds, sizeof(Matrix), MAX_MODEL_INSTANCE);
		sInputWorldSRV = computeShader->AsSRV("InputWorlds");

		inputBoneBuffer = new StructuredBuffer(NULL, sizeof(Matrix), MAX_MODEL_TRANSFORMS);
		sInputBoneSRV = computeShader->AsSRV("InputBones");

		//output 용 텍스쳐
		ID3D11Texture2D* texture;
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;//캐릭터수 :4
		desc.Height = MAX_MODEL_INSTANCE;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;

		Check(D3D::GetDevice()->CreateTexture2D(&desc, NULL, &texture));

		outputBuffer = new TextureBuffer(texture); //위 텍스처를 토대로 다시 텍스처버퍼생성
		SafeRelease(texture);

		sOutputUAV = computeShader->AsUAV("Output");

		

		sTransformsSRV = computeShader->AsSRV("TransformsMap");
		sComputeTweenBuffer = computeShader->AsConstantBuffer("CB_TweenFrame");
		sComputeBlendBuffer = computeShader->AsConstantBuffer("CB_BlendFrame");

	}
}
  

ModelAnimator::~ModelAnimator()
{
	SafeDelete(model);

	SafeDeleteArray(clipTransforms);
	SafeRelease(texture);
	SafeRelease(srv);

	SafeDelete(tweenBuffer);
	SafeDelete(blendBuffer);

	SafeDelete(instanceBuffer);
	SafeDelete(instanceBuffer_Color);

	SafeDelete(computeShader);
	SafeDelete(inputWorldBuffer);
	SafeDelete(inputBoneBuffer);

	SafeDelete(outputBuffer);

}

void ModelAnimator::Update()
{
	if (texture == NULL)
	{
		for (ModelMesh* mesh : model->Meshes())
			mesh->SetShader(shader);

		CreateTexture();

		Matrix bones[MAX_MODEL_TRANSFORMS];
		for (UINT i = 0; i < model->BoneCount(); i++)
			bones[i] = model->BoneByIndex(i)->Transform(); //애니메이션 되기전 본 matrix


		inputBoneBuffer->CopyToInput(bones);
	}

	for (UINT i = 0; i < transforms.size(); i++)
	{
		(blendDesc[i].Mode == 0) ? UpdateTweenMode(i) : UpdateBlendMode(i);
	}


	tweenBuffer->Render();
	blendBuffer->Render();

	frameTime += Time::Delta();
	if (frameTime > (1.0f / frameRate))
	{
		sComputeTweenBuffer->SetConstantBuffer(tweenBuffer->Buffer());
		sComputeBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());

		sTransformsSRV->SetResource(srv);
		sInputWorldSRV->SetResource(inputWorldBuffer->SRV());
		sInputBoneSRV->SetResource(inputBoneBuffer->SRV());
		sOutputUAV->SetUnorderedAccessView(outputBuffer->UAV());

		computeShader->Dispatch(0, 0, 1, MAX_MODEL_INSTANCE, 1);
		outputBuffer->CopyFromOutput();//ouptbuffer 로 연산결과를 가져옴
	}
	frameTime = fmod(frameTime, (1.0f / frameRate));
	// fmod() 함수는 x/y의 부동 소수점 나머지를 연산합니다.


	for (UINT i = 0; i < transforms.size(); i++)
	{
		(blendDesc[i].Mode == 0)? UpdateTweenMode(i): UpdateBlendMode(i);
		
		
	}

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Update();
	}
	
}

void ModelAnimator::Render()
{

	sTweenBuffer->SetConstantBuffer(tweenBuffer->Buffer());
	sBlendBuffer->SetConstantBuffer(blendBuffer->Buffer());

	instanceBuffer->Render();
	instanceBuffer_Color->Render();

	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->Render(transforms.size());
	}
}

void ModelAnimator::UpdateTweenMode(UINT instanceIndex)
{
	TweenDesc& desc = tweenDesc[instanceIndex];

	//현재 애니메이션
	{
		ModelClip* clip = model->ClipByIndex(desc.Curr.Clip);

		desc.Curr.RunningTime += Time::Delta();

		float one_frame_time = (1.0f / clip->FrameRate()) / desc.Curr.Speed;

		if (desc.Curr.Time >= 1.0f)
		{
			//다음 프레임
			desc.Curr.RunningTime = 0;
			desc.Curr.CurrFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount(); //desc.CurrFrame에 하나 다음
			desc.Curr.NextFrame = (desc.Curr.CurrFrame + 1) % clip->FrameCount(); //다시 desc.CurrFrame에 +1
		}
		desc.Curr.Time = desc.Curr.RunningTime / one_frame_time;
	}

	if (desc.Next.Clip > -1)
	{
		desc.ChangeTime += Time::Delta();
		desc.TweenTime = desc.ChangeTime / desc.TakeTime;


		if (desc.TweenTime >= 1.0f) //애니메이션 전환 완료
		{
			desc.Curr = desc.Next;

			desc.Next.Clip = -1;
			desc.Next.CurrFrame = 0;
			desc.Next.NextFrame = 0;
			desc.Next.Time = 0;
			desc.Next.RunningTime = 0.0f;

			desc.ChangeTime = 0.0f;
			desc.TweenTime = 0.0f;
		}
		else
		{
			//계속 시간을 진행시키면서 보간시킬 다음 프레임을 구한다

			ModelClip* clip = model->ClipByIndex(desc.Next.Clip);

			desc.Next.RunningTime += Time::Delta();

			float one_frame_time = (1.0f / clip->FrameRate()) / desc.Next.Speed;

			if (desc.Next.Time >= 1.0f)
			{
				//다음 프레임
				desc.Next.RunningTime = 0;
				desc.Next.CurrFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount(); //desc.CurrFrame에 하나 다음
				desc.Next.NextFrame = (desc.Next.CurrFrame + 1) % clip->FrameCount(); //다시 desc.CurrFrame에 +1
			}
			desc.Next.Time = desc.Next.RunningTime / one_frame_time;
		}
	}
}

void ModelAnimator::UpdateBlendMode(UINT instanceIndex)
{
	BlendDesc& desc = blendDesc[instanceIndex];

	for (UINT i = 0; i < 3; ++i)
	{
		ModelClip* clip = model->ClipByIndex(desc.Clip[i].Clip);

		desc.Clip[i].RunningTime += Time::Delta();

		float one_frame_time = (1.0f / clip->FrameRate()) / desc.Clip[i].Speed;

		if (desc.Clip[i].Time >= 1.0f)
		{
			//다음 프레임
			desc.Clip[i].RunningTime = 0;
			desc.Clip[i].CurrFrame = (desc.Clip[i].CurrFrame + 1) % clip->FrameCount(); //desc.CurrFrame에 하나 다음
			desc.Clip[i].NextFrame = (desc.Clip[i].CurrFrame + 1) % clip->FrameCount(); //다시 desc.CurrFrame에 +1
		}
		desc.Clip[i].Time = desc.Clip[i].RunningTime / one_frame_time; 
	}
}

void ModelAnimator::ReadMesh(wstring file)
{
	model->ReadMesh(file);
}

void ModelAnimator::ReadMaterial(wstring file)
{
	model->ReadMaterial(file);
}

void ModelAnimator::ReadClip(wstring file)
{
	model->ReadClip(file);
}

void ModelAnimator::Pass(UINT pass)
{
	for (ModelMesh* mesh : model->Meshes())
		mesh->Pass(pass);
}

void ModelAnimator::PlayTweenMode(UINT instanceId, UINT clip, float speed, float takeTime)
{
	blendDesc[instanceId].Mode = 0;

	tweenDesc[instanceId].Next.Clip = clip;
	tweenDesc[instanceId].Next.Speed = speed;
	tweenDesc[instanceId].TakeTime = takeTime;
}

void ModelAnimator::PlayBlendMode(UINT instanceId, UINT clip1, UINT clip2, UINT clip3)
{
	blendDesc[instanceId].Mode = 1;
	blendDesc[instanceId].Clip[0].Clip = clip1;
	blendDesc[instanceId].Clip[1].Clip = clip2;
	blendDesc[instanceId].Clip[2].Clip = clip3;

}

void ModelAnimator::SetBlendAlpha(UINT instanceId, float alpha)
{
	alpha = Math::Clamp(alpha, 0.0f, 2.0f);
	blendDesc[instanceId].Alpha = alpha;
}

//void ModelAnimator::SetAttachTransform(UINT boneIndex)
//{
//	//attachDesc.BoneIndex = boneIndex;
//}

void ModelAnimator::GetAttachTransform(UINT instance, Matrix* outResult)
{
	ID3D11Texture2D* texture = outputBuffer->Result();

	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(texture, 0, D3D11_MAP_READ, 0, &subResource);
	{
		//텍스처를 만들때 가로 : Bone , 세로 : Instance
		//그런데 실제 만들어진 텍스쳐는 2의배수로 가로 세로 만들어짐으로 subResource.RowPitch곱해줌
		memcpy(outResult, (BYTE*)subResource.pData + (instance * subResource.RowPitch), sizeof(Matrix)*MAX_MODEL_TRANSFORMS);
	}
	D3D::GetDC()->Unmap(texture, 0); 
}


void ModelAnimator::CreateTexture()
{
	clipTransforms = new ClipTransform[model->ClipCount()];
	for (UINT i = 0; i < model->ClipCount(); i++)
	{
		CreateClipTransform(i);
	}
	//Create Texture
	{
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_TEXTURE2D_DESC));
		desc.Width = MAX_MODEL_TRANSFORMS * 4;
		desc.Height = MAX_MODEL_KEYFRAMES;
		desc.ArraySize = model->ClipCount();
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.Usage = D3D11_USAGE_IMMUTABLE; //Texture2d array에는 dynamic 못씀
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.MipLevels = 1;
		desc.SampleDesc.Count = 1;
		
		UINT pageSize = MAX_MODEL_TRANSFORMS * (4 * 16) * MAX_MODEL_KEYFRAMES; //matrix 하나(본의 키 애니메이션 데이터)당 필요한 크기 : float * 16 = 64byte
		//사이즈 예약
		void* p = VirtualAlloc(NULL, pageSize * model->ClipCount(), MEM_RESERVE, PAGE_READWRITE);

	
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			UINT start = c * pageSize;
			for (UINT k = 0; k < MAX_MODEL_KEYFRAMES; k++)
			{
				// (BYTE*)p : 주소 설정
				// 시작주소에서 연산한 사이즈만큼 이동
				
				byte sizeMatrix = sizeof(Matrix); //64
				void* temp =  (BYTE*)p + start + ((MAX_MODEL_TRANSFORMS * k) * sizeMatrix); // MAX_MODEL_TRANSFORMS * k : 뼈 갯수 * 현재 프레임 
				//실제 할당
				VirtualAlloc(temp, MAX_MODEL_TRANSFORMS * sizeof(Matrix), MEM_COMMIT, PAGE_READWRITE);
				memcpy(temp, clipTransforms[c].Transform[k], MAX_MODEL_TRANSFORMS * sizeof(Matrix)); 
			}
		}
		D3D11_SUBRESOURCE_DATA* subResources = new D3D11_SUBRESOURCE_DATA[model->ClipCount()];
		for (UINT c = 0; c < model->ClipCount(); c++)
		{
			void* temp = (BYTE *)p + c * pageSize;
			//이제 메모리에 텍스처를 만드는 작업을 하기위해 주소를 설정 
			subResources[c].pSysMem = temp;
			subResources[c].SysMemPitch = MAX_MODEL_TRANSFORMS * sizeof(Matrix);
			//SysMemPitch : The distance (in bytes) from the beginning of one line of a texture to the next line.
			subResources[c].SysMemSlicePitch = pageSize;
			//The distance (in bytes) from the beginning of one depth level to the next.

		}
		
		Check(D3D::GetDevice()->CreateTexture2D(&desc, subResources, &texture));
		SafeDeleteArray(subResources);
		VirtualFree(p, 0, MEM_RELEASE);
	}
	//Create SRV
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_SHADER_RESOURCE_VIEW_DESC));
		desc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		desc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		desc.Texture2DArray.MipLevels = 1;
		desc.Texture2DArray.ArraySize = model->ClipCount();

		Check(D3D::GetDevice()->CreateShaderResourceView(texture, &desc, &srv));
	}
	for (ModelMesh* mesh : model->Meshes())
	{
		mesh->TransformsSRV(srv);
	}
}
  
void ModelAnimator::CreateClipTransform(UINT index)
{
	//텍스처로 만들 행렬을 준비한다
	Matrix* bones = new Matrix[MAX_MODEL_TRANSFORMS];

	ModelClip* clip = model->ClipByIndex(index);

	for (UINT f = 0; f < clip->FrameCount(); f++)
	{
		for (UINT b = 0; b < model->BoneCount(); b++)
		{
			ModelBone* bone = model->BoneByIndex(b);

			Matrix parent;
			Matrix inverseGlobal = bone->Transform();
			D3DXMatrixInverse(&inverseGlobal, NULL, &inverseGlobal);

			int parentIndex = bone->ParentIndex();
			if (parentIndex < 0)
			{
				D3DXMatrixIdentity(&parent);
			}
			else 
			{
				parent = bones[parentIndex];
			}

			Matrix animation;
			ModelKeyframe* frame = clip->Keyframe(bone->Name());
		
			if (frame != NULL)
			{
				ModelKeyframeData& data = frame->Transforms[f];
				Matrix S, R, T;
				D3DXMatrixScaling(&S, data.Scale.x, data.Scale.y, data.Scale.z);
				D3DXMatrixRotationQuaternion(&R, &data.Rotation);
				D3DXMatrixTranslation(&T, data.Translation.x, data.Translation.y, data.Translation.z);

				animation = S * R * T;
			}
			else
			{
				//부모를 따라서 
				D3DXMatrixIdentity(&animation);
			}
			//1.  현재 애니메이션의 움직임 matrix, 부모(글로벌) 곱하여 현재의 글로벌 좌표를 얻는다.
			bones[b] = animation * parent; //animation : 로컬좌표 / parent : 글로벌좌표

			//2. 다시 (위에서 움직임을 적용한) 로컬 위치 좌표를  얻기위해 역행렬을 곱해준다
			//index : clip , f: frame, b: bone matrix
			clipTransforms[index].Transform[f][b] = inverseGlobal * bones[b];
		}
	}
}



Transform * ModelAnimator::AddTransform()
{
	Transform* transform = new Transform();
	transforms.push_back(transform);
	return transform;
}

void ModelAnimator::UpdateTransforms()
{
	for (UINT i = 0; i < transforms.size(); i++)
	{
		memcpy(worlds[i], transforms[i]->World(), sizeof(Matrix));
	}

	//instanceBuffer에 월드 좌표 복사
	D3D11_MAPPED_SUBRESOURCE subResource;
	D3D::GetDC()->Map(instanceBuffer->Buffer(), 0, D3D11_MAP_WRITE_DISCARD, 0, &subResource);
	{
		memcpy(subResource.pData, worlds, sizeof(Matrix) * MAX_MESH_INSTANCE);
	}
	D3D::GetDC()->Unmap(instanceBuffer->Buffer(), 0);

	//instance들의 월드 좌표를 저장(차후 애니메이션의 모든 본 좌표를 얻어오기 위한 준비과정)
	inputWorldBuffer->CopyToInput(worlds);
}