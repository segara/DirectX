#pragma once
class FixCamera : public Camera
{
public:
	FixCamera();
	~FixCamera();

	void Update() override;
	void SetView(Matrix& view) { matrix_view = view; }
};

