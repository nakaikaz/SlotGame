#ifndef __Startup_Scene_H__
#define __Startup_Scene_H__

#include "cocos2d.h"

class StartupScene : public cocos2d::LayerColor
{
private:
	int _numberOfLoaded;
	int _numberOfLoadies;

public:
	static cocos2d::Scene* createScene();
	virtual bool init();
	CREATE_FUNC(StartupScene);

	void loadingCallback(cocos2d::Texture2D* texture);

};

#endif // __Startup_Scene_H__