#pragma once

#include "../SceneBase.h"
#include "../Source/Utility/SavePosition.h"

/**
 * タイトルシーン
 */
class ClearScene : public SceneBase
{
public:
	ClearScene();
	virtual ~ClearScene() {}

public:
	//~ Begin CSceneBase interface
	virtual void Initialize() override;
	virtual SceneType Update(float delta_seconds) override;
	virtual void Draw() override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::CLEAR_SCENE; }
	//~ End SceneBase interface

private:
	SavePosition* savePosition;
	unsigned int Cr;
};