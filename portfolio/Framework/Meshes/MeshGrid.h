#pragma once
class MeshGrid :
	public Mesh
{
public:
	MeshGrid(float offset_u = 1.0f, float offset_v = 1.0f);
	~MeshGrid();
private:
	void Create() override;
	float offset_u, offset_v;
};

