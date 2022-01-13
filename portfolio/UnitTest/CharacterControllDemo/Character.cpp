#include "stdafx.h"
#include "Character.h"

Character::~Character()
{
}

void Character::Update()
{
	Super::Update();
	Transform* transform = NULL;

	transform = GetTransform(0);

	Vector3 position = Vector3(0, 0, 0);

	transform->Position(&position);

	if (Keyboard::Get()->Press(VK_RIGHT)) {
		position.x += move_speed * Time::Delta();

		RotateCharacter(transform, Vector3(0, 0, -1), Vector3(1, 0, 0));
	}
		
	if (Keyboard::Get()->Press(VK_LEFT)) {
		position.x -= move_speed * Time::Delta();

		RotateCharacter(transform, Vector3(0, 0, -1), Vector3(-1, 0, 0));
	}
		
	if (Keyboard::Get()->Press(VK_UP)) {
		position.z += move_speed * Time::Delta();
		RotateCharacter(transform, Vector3(0, 0, -1), Vector3(0, 0, 1));
	}
		
	if (Keyboard::Get()->Press(VK_DOWN)) {
		position.z -= move_speed * Time::Delta();
		RotateCharacter(transform, Vector3(0, 0, -1), Vector3(0, 0, -1));
	}
	
	transform->Position(position);
	UpdateTransforms();

	float length = D3DXVec3Length(&D3DXVECTOR3(position.x - old_position.x, position.y - old_position.y, position.z - old_position.z));

	ANIMATION_CLIP cur_Ani = ANIMATION_CLIP::IDLE;
	if (length == 0)
	{
		cur_Ani = ANIMATION_CLIP::IDLE;
	}
	else 
	{
		cur_Ani = ANIMATION_CLIP::WALKING;
	}

	if (cur_Ani != Ani_Type) {
		PlayTweenMode(0, (UINT)cur_Ani, ani_speed, tweenTime);
		Ani_Type = cur_Ani;
	}
	

	old_position = position;
	
}

void Character::RotateCharacter(Transform* transform, Vector3 from, Vector3 to)
{
	from.y = 0;
	to.y = 0;
	
	float dot = D3DXVec3Dot(D3DXVec3Normalize(&from, &from), D3DXVec3Normalize(&to, &to));
	float AcosAngle = acosf(dot);
	float angle = Math::ToDegree(AcosAngle);
	
	//¿Þ¼Õ ÁÂÇ¥°è
	D3DXVECTOR3 crossed;
	D3DXVec3Cross(&crossed, &from, &to);
	if (crossed.y < 0)
	{
		angle = 360 - angle;
	}
	transform->RotationDegree(Vector3(0,angle,0));
	Gui::Get()->RenderText(5, 40, 1, 1, 1, std::to_string(angle));
}


