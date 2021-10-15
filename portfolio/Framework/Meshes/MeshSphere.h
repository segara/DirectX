#pragma once
class MeshSphere :
	public Mesh
{
public:
	MeshSphere(float radius, UINT stackCount = 10 , UINT sliceCount = 10);
	~MeshSphere();
private:
	void Create() override;
	float radius;
	UINT stackCount;
	UINT sliceCount;
};

