#pragma once

#include "../SceneBase.h"
#include "../../GameObject/SampleObject/SampleObject.h"
#include "../../GameObject/Enemy/Enemy_Purple.h"

/**
 * サンプルシーン
 * サンプル用に簡易的に実装
 */
class SampleScene : public SceneBase
{
public:
	SampleScene();
	virtual ~SampleScene() {}

public:
	//~ Begin CSceneBase interface
	virtual void Initialize() override;	
	virtual SceneType Update(float delta_seconds) override;
	virtual void Draw() override;
	virtual void Finalize() override;
	virtual SceneType GetSceneType() const override { return SceneType::SAMPLE_SCENE; }
	//~ End SceneBase interface

public:
	static SampleObject* sampleObject;
	static SampleScene* sampleScene;
	static Enemy_Purple* enemy_Purple;

public:
	static SampleObject* GetSampleObject();
	static SampleScene* GetSampleScene();
};