ByteAddressBuffer Input; //ByteAddressBuffer: SRV ����
RWByteAddressBuffer Output; //RW: UAV ����

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

// numthreads(8,8,1) �̶��8 * 8 * 1 = 64 ���� �����带 ���ķ� �����ڴ� ��� �ǹ��̴�.
//c++ �Լ� ó�� GPU�� �Լ���� ������ ������ �ִµ� �̴� Ŀ�� �̶�� �� ����Ѵ�.
//�� �ϳ��� Ŀ��(�Ʒ��� �Լ� : CS)�� �������� �����带 �Ѳ�����(���ķ�) ���� �� ������
//���� ����� numthreads ũ�⸸ŭ�� ��������� ���� �� �ִ�.
//���⿡ �߰��� �� Ŀ�� �ϳ��� �����ϴ� ������ �׷��̶�� ����ϱ⵵ �ϸ�, 
//�� �׷������ ���ķ� ó���� �����ϴٴ°��̴�.

[numthreads(32,32,1)] //������ ������ ���� : �Ʒ��� �Լ��� ������ ���� ���� ��
void CS(ComputeInput input)
{
    Group group;
    group.GroupID = asuint(input.GroupID);
    group.GroupThreadID = asuint(input.GroupThreadID);
    group.DispatchThreadID = asuint(input.DispatchThreadID);
    group.GroupIndex = asuint(input.GroupIndex);
    
    //�� �ּҸ� ����
    uint index = (input.GroupID.x * (32 * 32 * 1)) + input.GroupIndex;
    
    uint outAddress = index * 11 * 4;
    //10: ����ü ����� uint3 * 3�� + uint * 1�� +float *1�� = 11
    //4 : uint  4byte �̹Ƿ�
    
    uint inAddress = index * 4; //4byte
    group.ReturnValue = asfloat(Input.Load(inAddress));
    
    //Store : �����͸� ��������
    Output.Store3(outAddress + 0, asuint(group.GroupID));//4 * uint3 = 12��ŭ ��
    Output.Store3(outAddress + 12, asuint(group.GroupThreadID)); //4 * uint3 = 12��ŭ ��
    Output.Store3(outAddress + 24, asuint(group.DispatchThreadID)); //4 * uint3 = 12��ŭ ��
    Output.Store3(outAddress + 36, asuint(group.GroupIndex)); //4 * uint = 4��ŭ ��
    Output.Store3(outAddress + 40, asuint(group.ReturnValue)); //4 * uint = 4��ŭ ��
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
