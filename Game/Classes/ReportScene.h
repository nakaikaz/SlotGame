#ifndef __ReportScene_H__
#define __ReportScene_H__

#include "cocos2d.h"
#include "Config.h"

class ReportScene : public cocos2d::Layer
{
private:
	std::string _uuid;
	DeviceOrientation _deviceOrientation;
	void reArrangeAllChildrenByLandscape();
	void reArrangeAllChildrenByPortrait();

public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(ReportScene);

	bool init();

	void menuBackCallback(cocos2d::Ref* pSender);

	void touchTicketBodyCallback(cocos2d::Ref* pSender);
	void ticketHeadAnimationCallback(float dt);

};

#endif //__ReportScene_H__