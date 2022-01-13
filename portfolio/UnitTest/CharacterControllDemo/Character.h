#pragma once
class Character : public ModelAnimator
{
public:
	Character(Shader* shader) : ModelAnimator(shader) {}
	~Character();
public:
	void Update();
	void RotateCharacter(Transform* transform, Vector3 from, Vector3 to);
private:
	
	float move_speed = 20.0f;
	Vector3 old_position = Vector3(0, 0, 0);

#pragma region animation

	enum class ANIMATION_CLIP
	{
		IDLE = 0,
		WALKING,
		RUNNING,
		JUMPING
	}Ani_Type = ANIMATION_CLIP::IDLE;

	float ani_speed = 1.0f;
	float tweenTime = 0.0f;

	bool blendMode = false;
	float alpha = 0.0f;

#pragma endregion

	
};

