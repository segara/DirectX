struct VertexMesh
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    uint InstanceID : SV_InstanceID;
    matrix Transform : Inst1_Transform; //Inst:instancing
    float4 Color : Inst2_Color;
};

/////////////////////////////////////////////////////////////////////////////////

#define VS_GENERATE \
output.oPosition = input.Position.xyz; \
\
output.Position = WorldPosition(input.Position); \
output.wPosition = output.Position.xyz; \
output.Position = ViewProjection(output.Position); \
output.wvpPosition = output.Position; \
output.wvpPosition_Sub = output.Position; \
\
output.Normal = WorldNormal(input.Normal); \
output.Tangent = WorldTangent(input.Tangent); \
\
output.Uv = input.Uv;\
output.Color = input.Color;

////////////////////////////////////////////////////////////////////////////////

void SetMeshWorld(inout matrix world, VertexMesh input)
{
    world = input.Transform;
}
MeshOutput VS_Mesh(VertexMesh input)
{
    MeshOutput output;
    SetMeshWorld(World, input);
    VS_GENERATE
    
    return output;
}
////////////////////////////////////////////////////////////////////////////////
struct VertexModel
{
    float4 Position : Position;
    float2 Uv : Uv;
    float3 Normal : Normal;
    float3 Tangent : Tangent;
    float4 BlendIndices : BlendIndices;
    float4 BlendWeights : BlendWeights;
    
    uint InstanceID : SV_InstanceID;
    
    matrix Transform : Inst1_Transform; //Inst:instancing
    float4 Color : Inst2_Color;
};

Texture2DArray TransformsMap;
#define MAX_MODEL_TRANSFORMS 250

cbuffer CB_Bone
{
    uint BoneIndex;
};

void SetModelWorld(inout matrix world, VertexModel input)
{
    float4 m0 = TransformsMap.Load(int4(BoneIndex * 4 + 0, input.InstanceID, 0, 0));
    float4 m1 = TransformsMap.Load(int4(BoneIndex * 4 + 1, input.InstanceID, 0, 0));
    float4 m2 = TransformsMap.Load(int4(BoneIndex * 4 + 2, input.InstanceID, 0, 0));
    float4 m3 = TransformsMap.Load(int4(BoneIndex * 4 + 3, input.InstanceID, 0, 0));
    
    matrix transform = matrix(m0, m1, m2, m3);
    world = mul(transform, input.Transform);
}
MeshOutput VS_Model(VertexModel input)
{
    MeshOutput output;
    
    SetModelWorld(World, input);
    VS_GENERATE
    
    return output;
}
/////////////////////////////////////////////////////////////////////////////////////

#define MAX_MODEL_KEYFRAMES 500
#define MAX_MODEL_INSTANCE 500


struct AnimationFrame
{
    int Clip;

    uint CurrFrame;
    uint NextFrame;

    float Time;
    float Running;

    float3 Padding;
};

struct TweenFrame
{
    float TakeTime;
    float TweenTime;
    float RunningTime;
    float Padding;

    AnimationFrame Curr;
    AnimationFrame Next;
};

cbuffer CB_TweenFrame
{
    TweenFrame TweenFrames[MAX_MODEL_INSTANCE];
};

//struct VertexOutput
//{
//    float4 Position : SV_Position;
//    float3 Normal : Normal;
//    float2 Uv : Uv;
//};

void SetAnimationTweenWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };
    
    
    int clip[2];
    int currFrame[2];
    int nextFrame[2];
    float time[2];
    
    clip[0] = TweenFrames[input.InstanceID].Curr.Clip;
    currFrame[0] = TweenFrames[input.InstanceID].Curr.CurrFrame;
    nextFrame[0] = TweenFrames[input.InstanceID].Curr.NextFrame;
    time[0] = TweenFrames[input.InstanceID].Curr.Time;
    
    clip[1] = TweenFrames[input.InstanceID].Next.Clip;
    currFrame[1] = TweenFrames[input.InstanceID].Next.CurrFrame;
    nextFrame[1] = TweenFrames[input.InstanceID].Next.NextFrame;
    time[1] = TweenFrames[input.InstanceID].Next.Time;
    
    
    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0, next = 0;
    matrix currAnim = 0;
    matrix nextAnim = 0;
    
    matrix transform = 0;
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[0], clip[0], 0));
        c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[0], clip[0], 0));
        c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[0], clip[0], 0));
        c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[0], clip[0], 0));
        curr = matrix(c0, c1, c2, c3);
        
        n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[0], clip[0], 0));
        n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[0], clip[0], 0));
        n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[0], clip[0], 0));
        n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[0], clip[0], 0));
        next = matrix(n0, n1, n2, n3);
        
        currAnim = lerp(curr, next, time[0]);
        
        
        [flatten]
        if (clip[1] > -1)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, currFrame[1], clip[1], 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, currFrame[1], clip[1], 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, currFrame[1], clip[1], 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, currFrame[1], clip[1], 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, nextFrame[1], clip[1], 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, nextFrame[1], clip[1], 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, nextFrame[1], clip[1], 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, nextFrame[1], clip[1], 0));
            next = matrix(n0, n1, n2, n3);
        
            nextAnim = lerp(curr, next, time[1]);
            
            currAnim = lerp(currAnim, nextAnim, TweenFrames[input.InstanceID].TweenTime);
        }
        
        
        transform += mul(weights[i], currAnim);
    }
    
    world = mul(transform, input.Transform);
}

struct BlendFrame
{
    uint Mode;
    float Alpha;
    float2 Padding;
    AnimationFrame Clip[3];
};

cbuffer CB_BlendFrame
{
    BlendFrame BlendFrames[MAX_MODEL_INSTANCE];
};

void SetAnimationBlendWorld(inout matrix world, VertexModel input)
{
    float indices[4] = { input.BlendIndices.x, input.BlendIndices.y, input.BlendIndices.z, input.BlendIndices.w };
    float weights[4] = { input.BlendWeights.x, input.BlendWeights.y, input.BlendWeights.z, input.BlendWeights.w };

    
    float4 c0, c1, c2, c3;
    float4 n0, n1, n2, n3;
    
    matrix curr = 0, next = 0;
    matrix currAnim[3];
    
    matrix anim = 0;
    matrix transform = 0;
    
    BlendFrame frame = BlendFrames[input.InstanceID];
    
    [unroll(4)]
    for (int i = 0; i < 4; i++)
    {
        [unroll(3)]
        for (int k = 0; k < 3; k++)
        {
            c0 = TransformsMap.Load(int4(indices[i] * 4 + 0, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            c1 = TransformsMap.Load(int4(indices[i] * 4 + 1, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            c2 = TransformsMap.Load(int4(indices[i] * 4 + 2, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            c3 = TransformsMap.Load(int4(indices[i] * 4 + 3, frame.Clip[k].CurrFrame, frame.Clip[k].Clip, 0));
            curr = matrix(c0, c1, c2, c3);
        
            n0 = TransformsMap.Load(int4(indices[i] * 4 + 0, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            n1 = TransformsMap.Load(int4(indices[i] * 4 + 1, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            n2 = TransformsMap.Load(int4(indices[i] * 4 + 2, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            n3 = TransformsMap.Load(int4(indices[i] * 4 + 3, frame.Clip[k].NextFrame, frame.Clip[k].Clip, 0));
            next = matrix(n0, n1, n2, n3);
        
            currAnim[k] = lerp(curr, next, frame.Clip[k].Time);
        }
           
        //clipA,clipB = 0,1,2 
        int clipA = (int) frame.Alpha;
        int clipB = clipA + 1;
        clipB %= 3;
        
        float alpha = frame.Alpha;
        if (alpha >= 1.0f)
        {
            alpha = frame.Alpha - 1.0f; // (0~1사이로 alpha 값이 조정되어야 하기 때문에
            
            if (frame.Alpha >= 2.0f)
            {
                clipA = 1;
                clipB = 2;
            }
        }
        
        anim = lerp(currAnim[clipA], currAnim[clipB], alpha);
        transform += mul(weights[i], anim);
    }
    
    world = mul(transform, input.Transform);
}

MeshOutput VS_Animation(VertexModel input)
{
    MeshOutput output;
    
    //World = mul(BoneTransforms[BoneIndex], World);
    if (BlendFrames[input.InstanceID].Mode == 0)
    {
        SetAnimationTweenWorld(World, input);
    }
    else
    {
        SetAnimationBlendWorld(World, input);

    }
    VS_GENERATE
    return output;
}
/////////////////////////////////////////////////////////////////
float4 PS_Sky(MeshOutput input) : SV_Target
{
    return SkyCubeMap.Sample(LinearSampler, input.oPosition);
}