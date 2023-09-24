#pragma once

#include "../SceneBase.h"

/**
 * タイトルシーン
 */
class TitleScene : public SceneBase
{
public:
	TitleScene();
	virtual ~TitleScene() {}

public:
	//~ Begin CSceneBase interface
	virtual void Initialize() override;
	virtual SceneType Update(float delta_seconds) override;
	virtual void Draw() override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::TITLE_SCENE; }
	//~ End SceneBase interface

private:
	int loaded_title_handle;
	int loaded_chara_handle;
	unsigned int Cr;
};