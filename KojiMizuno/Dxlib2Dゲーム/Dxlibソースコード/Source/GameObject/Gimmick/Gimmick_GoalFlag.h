#pragma once

#include "../GameObject.h"
#include "../Ground/Ground.h"
#include "../Source/Utility/BoxCollisionParams.h"
#include "../SampleObject/SampleObject.h"
#include <string>

/**
 * �T���v���I�u�W�F�N�g
 * �T���v���p�ɊȈՓI�Ɏ���
 */
class Gimmick_GoalFlag : public GameObject
{
public:
	Gimmick_GoalFlag();
	virtual ~Gimmick_GoalFlag();

public:
	//~ Begin GameObject interface
	virtual void Initialize() override;
	virtual void Update(float delta_seconds) override;
	virtual void Draw(const Vector2D& screen_offset) override;
	virtual void Finalize() override;
	//~ End GameObject interface

private:
	SampleObject* sampleObject_;
	BoxCollisionParams* bcp;
	
	int loaded_goal_flag;
};