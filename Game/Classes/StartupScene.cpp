#include "StartupScene.h"
#include "Name.h"
#include "Config.h"
#include "GameScene.h"
#include "RegisterScene.h"

USING_NS_CC;

Scene* StartupScene::createScene()
{
	auto scene = Scene::create();
	auto layer = StartupScene::create();
	scene->addChild(layer);
	return scene;
}

bool StartupScene::init()
{
	if (!LayerColor::initWithColor(Color4B::WHITE))
	{
		return false;
	}

	_numberOfLoadies = 0;
	_numberOfLoaded = 0;

	Size visibleSize = Director::getInstance()->getVisibleSize();

	auto logo = Sprite::create("logo.png");
	logo->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2 + 180));
	addChild(logo);

	auto copyright = Sprite::create("copyright.png");
	copyright->setPosition(visibleSize.width / 2, 160);
	addChild(copyright);


#if defined(COCOS2D_DEBUG)
	auto conf = TTFConfig("fonts/NGO_____.ttf", 36);
	auto label = Label::createWithTTF(conf, "0%");
	label->setColor(Color3B::BLACK);
	label->setPosition(visibleSize.width / 2, visibleSize.height / 2 - 100);
	label->setName("percentage");
	addChild(label);
#endif

	std::for_each(name::resource::all.begin(), name::resource::all.end(), [&](const std::string& png){
		//CCLOG("png: %s", png.c_str());
		Director::getInstance()->getTextureCache()->addImageAsync(png, CC_CALLBACK_1(StartupScene::loadingCallback, this));
		_numberOfLoadies++;
	});

	return true;
}

void StartupScene::loadingCallback(cocos2d::Texture2D* texture)
{
	++_numberOfLoaded;

#if defined(COCOS2D_DEBUG)
	auto percentage = static_cast<float>(_numberOfLoaded) / _numberOfLoadies * 100;
	static_cast<Label*>(getChildByName("percentage"))->setString(StringUtils::format("%3.1f%%", percentage));
#endif

	if (_numberOfLoadies == _numberOfLoaded) {
		// uuidをチェックして、値が無ければ登録シーンへ、在ればゲームシーンへ
		if (UserDefault::getInstance()->getStringForKey(config::user::UUID, "").empty()) {
			Director::getInstance()->replaceScene(TransitionFade::create(0.4f, RegisterScene::createScene(), Color3B::WHITE));
		}
		else {
			Director::getInstance()->replaceScene(TransitionFade::create(0.4f, GameScene::createScene(), Color3B::WHITE));
		}
	}
}
