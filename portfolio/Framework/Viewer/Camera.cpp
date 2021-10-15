#include "Framework.h"
#include "Camera.h"


Camera::Camera()
{
	D3DXMatrixIdentity(&matrix_rotation);
	D3DXMatrixIdentity(&matrix_view);
	
	Rotation();
	Move();
}

Camera::~Camera()
{
}

void Camera::Move()
{
	View();
}

void Camera::Rotation()
{
	//각도에 맞는 회전 행렬 설정
	Matrix X, Y, Z;
	D3DXMatrixRotationX(&X, rotation.x);
	D3DXMatrixRotationY(&Y, rotation.y);
	D3DXMatrixRotationZ(&Z, rotation.z);

	matrix_rotation = X * Y * Z;

	//각 축을 회전
	D3DXVec3TransformNormal(&forward, &Vector3(0, 0, 1), &matrix_rotation);
	D3DXVec3TransformNormal(&up, &Vector3(0, 1, 0), &matrix_rotation);
	D3DXVec3TransformNormal(&right, &Vector3(1, 0, 0), &matrix_rotation);

}

void Camera::View()
{
	D3DXMatrixLookAtLH(&matrix_view, &position, &(position + forward), &up);
}

void Camera::Position(float x, float y, float z)
{
	Position(Vector3(x, y, z));
}

void Camera::Position(Vector3 & vec)
{
	position = vec;
	Move();
}

void Camera::Position(Vector3 * vec)
{
	//vec = &position;
	*(vec) = position; // *(포인터) : 값에 접근
}

void Camera::Rotation(float x, float y, float z)
{
	Rotation(Vector3(x, y, z));
}

void Camera::Rotation(Vector3 & vec)
{
	rotation = vec;
	Rotation();
}

void Camera::Rotation(Vector3 * vec)
{
	*(vec) = rotation;
}

void Camera::RotationDegree(float x, float y, float z)
{
	RotationDegree(Vector3(x, y, z));

}

void Camera::RotationDegree(Vector3 & vec)
{
	rotation = vec * 0.0174532f; //Math::PI / 180.0f
	Rotation();
}

void Camera::RotationDegree(Vector3 * vec)
{
	*vec = rotation * 57.29577f; //180.0f / Math::PI
}

void Camera::GetMatrix(Matrix * matrix)
{
	memcpy(matrix, &matrix_view, sizeof(Matrix));
}
