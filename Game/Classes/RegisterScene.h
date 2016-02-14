#ifndef __RegisterScene_H__
#define __RegisterScene_H__

#include "cocos2d.h"
#include "ui\UIEditBox\UIEditBox.h"

class RegisterScene : public cocos2d::Layer, public cocos2d::ui::EditBoxDelegate
{
private:
	std::string _uuid;
	cocos2d::EventListenerCustom* _customListener;

public:
	static cocos2d::Scene* createScene();
	CREATE_FUNC(RegisterScene);

	bool init();

	virtual void onExit();

	virtual void editBoxEditingDidBegin(cocos2d::ui::EditBox* editBox);
	virtual void editBoxEditingDidEnd(cocos2d::ui::EditBox* editBox);
	virtual void editBoxTextChanged(cocos2d::ui::EditBox* editBox, const std::string& text);
	virtual void editBoxReturn(cocos2d::ui::EditBox* editBox);

	void menuOkayCallback(cocos2d::Ref* pSender);

	void reArrangeAllChildren();

};

#endif //__RegisterScene_H__