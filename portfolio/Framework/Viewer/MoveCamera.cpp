#include "Framework.h"
#include "MoveCamera.h"


MoveCamera::MoveCamera()
{
}


MoveCamera::~MoveCamera()
{
}

void MoveCamera::Update()
{
	Vector3 forward = GetForward();
	Vector3 up = GetUp();
	Vector3 right = GetRight();

	//Move
	{
		Vector3 position;
		Position(&position); //현재 위치로 세팅

		if (Keyboard::Get()->Press('W'))
		{
			position = position + forward * move_speed * Time::Delta();
		}
		if (Keyboard::Get()->Press('S'))
		{
			position = position - forward * move_speed * Time::Delta();
		}
		if (Keyboard::Get()->Press('A'))
		{
			position = position - right * move_speed * Time::Delta();
		}
		if (Keyboard::Get()->Press('D'))
		{
			position = position + right * move_speed * Time::Delta();
		}
		if (Keyboard::Get()->Press('Q'))
		{
			position = position + up * move_speed * Time::Delta();
		}
		if (Keyboard::Get()->Press('E'))
		{
			position = position - up * move_speed * Time::Delta();
		}

		Position(position); //변경후 값 저장

	}
	if (Mouse::Get()->Press(1)) {
		//Rotation
		{
			Vector3 RotateVec;
			Rotation(&RotateVec);

			Vector3 val = Mouse::Get()->GetMoveValue(); //mouse move delta
			float mouse_move_y = val.y;
			RotateVec.x = RotateVec.x + mouse_move_y * rotate_speed * Time::Delta(); //RotateVec.x : x축으로한 회전 셋팅을 위해, 즉 Pitch

			float mouse_move_x = val.x;
			RotateVec.y = RotateVec.y + mouse_move_x * rotate_speed * Time::Delta(); //RotateVec.y : y축으로한 회전 셋팅을 위해, 즉 Yaw

			RotateVec.z = 0.0f;

			Rotation(RotateVec);
		}
	}
	
}

void MoveCamera::Speed(float move, float rotation)
{
	this->move_speed = move;
	this->rotate_speed = rotation;
}
