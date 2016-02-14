#ifndef __GameScene_H__
#define __GameScene_H__

#include "cocos2d.h"
#include "Config.h"

class GameScene : public cocos2d::Layer
{
private:
	/*! 端末の向き */
	DeviceOrientation _deviceOrientation;

public:
    static cocos2d::Scene* createScene();
    virtual bool init();
    CREATE_FUNC(GameScene);
	void reArrangeChildrens();
};


#endif // __GameScene_H__
