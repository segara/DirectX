#pragma once
#include "Camera.h"
class MoveCamera :
	public Camera
{
public:
	MoveCamera();
	~MoveCamera();

	void Update() override;
	void Speed(float move, float rotation = 2);
private : 
	float move_speed = 20.0f;
	float rotate_speed = 2.0f;
};

