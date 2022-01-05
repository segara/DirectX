#pragma once
class Camera
{
public:
	Matrix matrix_view;
private:
	Vector3 position = Vector3(0, 0, 0);
	Vector3 rotation = Vector3(0, 0, 0);
	Vector3 forward = Vector3(0, 0, 1);
	Vector3 up = Vector3(0, 0, 1);
	Vector3 right = Vector3(1, 0, 0);

	Matrix matrix_rotation;

public:
	Camera();
	virtual ~Camera();
	virtual void Update() = 0;

protected:
	virtual void Move();
	virtual void Rotation();
protected:
	virtual void View();

public:
	void Position(float x, float y, float z);
	void Position(Vector3& vec);
	void Position(Vector3* vec);

	void Rotation(float x, float y, float z);
	void Rotation(Vector3& vec);
	void Rotation(Vector3* vec);

	void RotationDegree(float x, float y, float z);
	void RotationDegree(Vector3& vec);
	void RotationDegree(Vector3* vec);

	void GetMatrix(Matrix* matrix); //값의 복사가 일어나지 않기 위해 포인터로 리턴

	Vector3 GetForward() { return forward; }
	Vector3 GetUp() { return up; }
	Vector3 GetRight() { return right; }

};

