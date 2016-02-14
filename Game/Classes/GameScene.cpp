#include "GameScene.h"
#include "MainScene.h"
#include "ReportScene.h"
#include "Config.h"
#include "Name.h"

USING_NS_CC;

Scene* GameScene::createScene()
{
    auto scene = Scene::create();
    auto layer = GameScene::create();
    scene->addChild(layer);
    return scene;
}

bool GameScene::init()
{
	if (!Layer::init()) {
        return false;
    }

	_deviceOrientation = DeviceOrientation::Portrait;

	auto visibleSize = Director::getInstance()->getVisibleSize();

	//setContentSize(Size(visibleSize.width * 2, visibleSize.height));

	// 背景レイヤーの上にメインシーン、レポートシーンを含めるコンテナシーンを重ねて、コンテナシーンを移動させ、背景は固定されるようにする

	// 背景レイヤー
	auto bkgLayer = Layer::create();
	bkgLayer->setName(name::scene::game::BACKGROUND_LAYER);
	addChild(bkgLayer);

	auto cache = Director::getInstance()->getTextureCache();
	auto tex = cache->getTextureForKey(name::resource::GAME_BACKGROUND);
	auto spr = Sprite::createWithTexture(tex);
	cache->removeTexture(tex);
	//spr->setName(name::GAME_BACKGROUND);
	spr->setAnchorPoint(Vec2::ZERO);
	spr->setPosition(Vec2::ZERO);
	bkgLayer->addChild(spr);
	
	// コンテナレイヤー（メインシーンとレポートシーンを含める）
	auto containerLayer = Layer::create();
	containerLayer->setName(name::scene::game::CONTAINER_LAYER);
	addChild(containerLayer);

	auto mainScene = MainScene::create();
	mainScene->setName(name::scene::MAIN);
	mainScene->setAnchorPoint(Vec2::ZERO);
	mainScene->setPosition(Vec2::ZERO);
	containerLayer->addChild(mainScene);

	auto reportScene = ReportScene::create();
	reportScene->setName(name::scene::REPORT);
	reportScene->setAnchorPoint(Vec2::ZERO);
	reportScene->setPosition(Vec2(720, 0));
	containerLayer->addChild(reportScene);

	// 左スクロールイベント
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::ScrollToLeftNotification, [this](EventCustom* evt){
		auto vec = Vec2::ZERO;
		if (DeviceOrientation::Portrait == _deviceOrientation){
			vec = Vec2(-720, 0);
		}
		else if (DeviceOrientation::Landscape == _deviceOrientation){
			vec = Vec2(-1280, 0);
		}
		getChildByName(name::scene::game::CONTAINER_LAYER)->runAction(MoveBy::create(0.3f, vec));
	});
	// 右スクロールイベント
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::ScrollToRightNotification, [this](EventCustom* evt){
		auto vec = Vec2::ZERO;
		if (DeviceOrientation::Portrait == _deviceOrientation){
			vec = Vec2(720, 0);
		}
		else if (DeviceOrientation::Landscape == _deviceOrientation){
			vec = Vec2(1280, 0);
		}
		// スクロールしてスクロール完了を通知
		auto action = Sequence::createWithTwoActions(MoveBy::create(0.3f, vec), CallFunc::create([this](){
			Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(config::event::ScrollToRightCompletionNotification);
		}));
		getChildByName(name::scene::game::CONTAINER_LAYER)->runAction(action);
	});
	// 端末の向き変更イベント
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::OrientationChangedNotification, [this](EventCustom* evt){
		auto data = static_cast<ValueMap*>(evt->getUserData());
		auto w = data->at("w").asInt();
		auto h = data->at("h").asInt();

		auto glview = Director::getInstance()->getOpenGLView();
		// フレームサイズをセットしてから、デザイン解像度をセット
		glview->setFrameSize(w, h);
		if (w > h) {
			//CCLOG("(landscape) w: %d, h: %d", w, h);
			glview->setDesignResolutionSize(config::resolution::DESIGN_RESOLUTION_LONG_SPAN, config::resolution::DESIGN_RESOLUTION_SHORT_SPAN, config::resolution::DESIGN_POLICY);
			_deviceOrientation = DeviceOrientation::Landscape;
		}
		else{
			//CCLOG("(portrait) w: %d, h: %d", w, h);
			glview->setDesignResolutionSize(config::resolution::DESIGN_RESOLUTION_SHORT_SPAN, config::resolution::DESIGN_RESOLUTION_LONG_SPAN, config::resolution::DESIGN_POLICY);
			_deviceOrientation = DeviceOrientation::Portrait;
		}
		reArrangeChildrens();
	});

#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	auto keyboardListener = EventListenerKeyboard::create();
	keyboardListener->onKeyPressed = [this](EventKeyboard::KeyCode keyCode, Event* event){};
	keyboardListener->onKeyReleased = [this](EventKeyboard::KeyCode keyCode, Event* event){
		if (EventKeyboard::KeyCode::KEY_V != keyCode && EventKeyboard::KeyCode::KEY_H != keyCode)
			return;

		auto data = ValueMap();
		if (EventKeyboard::KeyCode::KEY_V == keyCode){
			data["w"] = 540;
			data["h"] = 960;
		}
		else if (EventKeyboard::KeyCode::KEY_H == keyCode){
			data["w"] = 960;
			data["h"] = 540;
		}
		auto evtDispatcher = Director::getInstance()->getEventDispatcher();
		evtDispatcher->dispatchCustomEvent(config::event::OrientationChangedNotification, &data);
	};
	Director::getInstance()->getEventDispatcher()->addEventListenerWithSceneGraphPriority(keyboardListener, this);
#endif

    return true;
}

void GameScene::reArrangeChildrens()
{
	auto containerLayer = getChildByName(name::scene::game::CONTAINER_LAYER);
	auto visibleSize = Director::getInstance()->getVisibleSize();
	auto origin = Vec2::ZERO;

	auto reportScenePosition = Vec2::ZERO;
	if (DeviceOrientation::Landscape == _deviceOrientation) {
		reportScenePosition = Vec2(1280, 0);
		if (containerLayer->getPosition().x < 0)
			origin = Vec2(-1280, 0);
	}
	else if (DeviceOrientation::Portrait == _deviceOrientation){
		reportScenePosition = Vec2(720, 0);
		if (containerLayer->getPosition().x < 0)
			origin = Vec2(-720, 0);
	}
	containerLayer->setPosition(origin);
	containerLayer->getChildByName(name::scene::MAIN)->setPosition(Vec2::ZERO);
	containerLayer->getChildByName(name::scene::REPORT)->setPosition(reportScenePosition);
}