ByteAddressBuffer Input; //ByteAddressBuffer: SRV 연결
RWByteAddressBuffer Output; //RW: UAV 연결

struct Group
{
    uint3 GroupID;
    uint3 GroupThreadID;
    uint3 DispatchThreadID;
    uint GroupIndex;
    float ReturnValue;
};

struct ComputeInput
{
    uint3 GroupID : SV_GroupID;
    uint3 GroupThreadID : SV_GroupThreadID;
    uint3 DispatchThreadID  : SV_DispatchThreadID;
    uint GroupIndex : SV_GroupIndex;
};

// numthreads(8,8,1) 이라면8 * 8 * 1 = 64 개의 스레드를 병렬로 돌리겠다 라는 의미이다.
//c++ 함수 처럼 GPU도 함수라는 개념을 가지고 있는데 이는 커널 이라는 용어를 사용한다.
//요 하나의 커널(아래의 함수 : CS)이 여러개의 스레드를 한꺼번에(병렬로) 돌릴 수 있으며
//위에 명시한 numthreads 크기만큼의 스레드들을 가질 수 있다.
//여기에 추가로 이 커널 하나를 실행하는 단위를 그룹이라고 명명하기도 하며, 
//이 그룹까지도 병렬로 처리가 가능하다는것이다.

[numthreads(32,32,1)] //쓰레드 갯수를 결정 : 아래의 함수가 쓰레드 마다 돌게 됨
void CS(ComputeInput input)
{
    Group group;
    group.GroupID = asuint(input.GroupID);
    group.GroupThreadID = asuint(input.GroupThreadID);
    group.DispatchThreadID = asuint(input.DispatchThreadID);
    group.GroupIndex = asuint(input.GroupIndex);
    
    //쓸 주소를 결정
    uint index = (input.GroupID.x * (32 * 32 * 1)) + input.GroupIndex;
    
    uint outAddress = index * 11 * 4;
    //10: 구조체 멤버가 uint3 * 3개 + uint * 1개 +float *1개 = 11
    //4 : uint  4byte 이므로
    
    uint inAddress = index * 4; //4byte
    group.ReturnValue = asfloat(Input.Load(inAddress));
    
    //Store : 데이터를 내보낼때
    Output.Store3(outAddress + 0, asuint(group.GroupID));//4 * uint3 = 12만큼 씀
    Output.Store3(outAddress + 12, asuint(group.GroupThreadID)); //4 * uint3 = 12만큼 씀
    Output.Store3(outAddress + 24, asuint(group.DispatchThreadID)); //4 * uint3 = 12만큼 씀
    Output.Store3(outAddress + 36, asuint(group.GroupIndex)); //4 * uint = 4만큼 씀
    Output.Store3(outAddress + 40, asuint(group.ReturnValue)); //4 * uint = 4만큼 씀
}

technique11 T0
{
    pass P0
    {
        SetVertexShader(NULL);
        SetPixelShader(NULL);

        SetComputeShader(CompileShader(cs_5_0, CS()));
    }
}
