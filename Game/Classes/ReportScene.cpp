#include "ReportScene.h"
#include "Name.h"
#include "GameValue.h"
//#include "Config.h"
#include "SimpleAudioEngine.h"
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
#include "platform\win32.h"
#endif

USING_NS_CC;

Scene* ReportScene::createScene()
{
	auto scene = Scene::create();
	auto layer = ReportScene::create();
	scene->addChild(layer);
	return scene;
}

bool ReportScene::init()
{
	if (!Layer::init()) {
		return false;
	}

	_deviceOrientation = DeviceOrientation::Portrait;

	Size visibleSize = Director::getInstance()->getVisibleSize();
	auto gameValue = GameValue::getInstance();

	auto cache = Director::getInstance()->getTextureCache();
	// チケット
	auto tex = cache->getTextureForKey(name::resource::POINT_TICKET_HEAD);
	auto ticketHead = Sprite::createWithTexture(tex);
	ticketHead->setName(name::scene::report::POINT_TICKET_HEAD);
	ticketHead->setAnchorPoint(Vec2::ZERO);
	ticketHead->setPosition(144, 805);
	addChild(ticketHead);

#if 0
	tex = cache->getTextureForKey(name::resource::POINT_TICKET_BODY);
	auto ticketBody = Sprite::createWithTexture(tex);
	ticketBody->setName(name::POINT_TICKET_BODY);
	ticketBody->setAnchorPoint(Vec2::ZERO);
	ticketBody->setPosition(144, 340);
	addChild(ticketBody);
#endif


	tex = cache->getTextureForKey(name::resource::POINT_TICKET_BODY);
	auto ticketBody = MenuItemImage::create("", "", CC_CALLBACK_1(ReportScene::touchTicketBodyCallback, this));
	ticketBody->setNormalImage(Sprite::createWithTexture(tex));
	ticketBody->setSelectedImage(Sprite::createWithTexture(tex));
	cache->removeTexture(tex);
	ticketBody->setName(name::scene::report::POINT_TICKET_BODY);
	ticketBody->setAnchorPoint(Vec2::ZERO);
	ticketBody->setPosition(144, 340);

	// IDを表示
#if 1
	auto labelId = Label::createWithSystemFont(gameValue->getUuid().c_str(), config::font::Default, 32, Size(240, 36), TextHAlignment::RIGHT);
	labelId->setName(name::scene::report::LABEL_ID);
	labelId->setColor(Color3B::BLACK);
	labelId->setAnchorPoint(Vec2::ZERO);
	labelId->setPosition(Vec2(145, 235));
	ticketBody->addChild(labelId);
	// ポイントを表示
	auto labelPoint = Label::createWithSystemFont(StringUtils::format("%d", gameValue->getTotalScore()), config::font::Default, 32, Size(100, 36), TextHAlignment::RIGHT);
	labelPoint->setName(name::scene::report::LABEL_POINT);
	labelPoint->setColor(Color3B::BLACK);
	labelPoint->setAnchorPoint(Vec2::ZERO);
	labelPoint->setPosition(Vec2(280, 170));
	ticketBody->addChild(labelPoint);
#else
	TTFConfig ttfConf("fonts/NGO_____.ttf", 32);
	auto labelId = Label::createWithTTF(ttfConf, gameValue->getUuid(), TextHAlignment::RIGHT);
	labelId->setColor(Color3B::BLACK);
	labelId->setAnchorPoint(Vec2(1.0f, 0));
	labelId->setPosition(Vec2(380, 235));
	ticketBody->addChild(labelId);

	auto labelPoint = Label::createWithTTF(ttfConf, StringUtils::format("%d", gameValue->getTotalScore()), TextHAlignment::RIGHT);
	labelPoint->setColor(Color3B::BLACK);
	labelPoint->setAnchorPoint(Vec2(1.0f, 0));
	labelPoint->setPosition(Vec2(380, 170));
	ticketBody->addChild(labelPoint);
#endif

	auto backButton = MenuItemImage::create("", "", CC_CALLBACK_1(ReportScene::menuBackCallback, this));
	auto backNormalTex = cache->getTextureForKey(name::resource::BACK_BUTTON);
	auto backSelectedTex = cache->getTextureForKey(name::resource::BACK_BUTTON_SELECTED);
	backButton->setNormalImage(Sprite::createWithTexture(backNormalTex));
	backButton->setSelectedImage(Sprite::createWithTexture(backSelectedTex));
	cache->removeTexture(backNormalTex);
	cache->removeTexture(backSelectedTex);
	backButton->setName(name::scene::report::TO_MAIN_BUTTON);
	backButton->setAnchorPoint(Vec2::ZERO);
	backButton->setPosition(34, 1000);

	// create menu, it's an autorelease object
	auto menuItems = Vector<MenuItem*>();
	menuItems.pushBack(ticketBody);
	menuItems.pushBack(backButton);
	auto menu = Menu::createWithArray(menuItems);
	menu->setName(name::scene::report::REPORT_SCENE_MENU);
	menu->setAnchorPoint(Vec2::ZERO);
	menu->setPosition(Vec2::ZERO);
	addChild(menu);

	// スクロールしてレポートシーンが消えたことを検知するイベント
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::ScrollToRightCompletionNotification, [this](EventCustom* evt){
		// チケットヘッドを元に戻す
		auto ticketHead = getChildByName(name::scene::report::POINT_TICKET_HEAD);
		if (DeviceOrientation::Portrait == _deviceOrientation)
			ticketHead->setPosition(144, 805);
		else if (DeviceOrientation::Landscape == _deviceOrientation)
			ticketHead->setPosition(423, 528);
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
			_deviceOrientation = DeviceOrientation::Landscape;
			reArrangeAllChildrenByLandscape();
		}
		else{
			_deviceOrientation = DeviceOrientation::Portrait;
			reArrangeAllChildrenByPortrait();
		}
	});

	return true;
}

void ReportScene::reArrangeAllChildrenByPortrait()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// チケットヘッド
	getChildByName(name::scene::report::POINT_TICKET_HEAD)->setPosition(144, 805);
	// チケットボディ
	auto ticketBody = getChildByName(name::scene::report::REPORT_SCENE_MENU)->getChildByName(name::scene::report::POINT_TICKET_BODY);
	ticketBody->setPosition(144, 340);

	// ID
	ticketBody->getChildByName(name::scene::report::LABEL_ID)->setPosition(145, 235);
	// ポイント
	ticketBody->getChildByName(name::scene::report::LABEL_POINT)->setPosition(280, 170);

	// 戻るボタン
	getChildByName(name::scene::report::REPORT_SCENE_MENU)->getChildByName(name::scene::report::TO_MAIN_BUTTON)->setPosition(34, 1000);
}

void ReportScene::reArrangeAllChildrenByLandscape()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// チケットヘッド
	getChildByName(name::scene::report::POINT_TICKET_HEAD)->setPosition(423, 528);
	// チケットボディ
	auto ticketBody = getChildByName(name::scene::report::REPORT_SCENE_MENU)->getChildByName(name::scene::report::POINT_TICKET_BODY);
	ticketBody->setPosition(423, 62);

	// ID
	ticketBody->getChildByName(name::scene::report::LABEL_ID)->setPosition(145, 235);
	// ポイント
	ticketBody->getChildByName(name::scene::report::LABEL_POINT)->setPosition(280, 170);

	// 戻るボタン
	getChildByName(name::scene::report::REPORT_SCENE_MENU)->getChildByName(name::scene::report::TO_MAIN_BUTTON)->setPosition(53, 620);
}

void ReportScene::touchTicketBodyCallback(Ref* pSender)
{
	if (GameValue::getInstance()->getTotalScore() < 1)
		return;

	auto ticketBody = static_cast<Sprite*>(pSender);
	if (ticketBody){
		scheduleOnce(CC_SCHEDULE_SELECTOR(ReportScene::ticketHeadAnimationCallback), 0);
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(config::audio::TICKET_EFFECT);
		GameValue::getInstance()->clearTotalScore();
		static_cast<Label*>(ticketBody->getChildByName(name::scene::report::LABEL_POINT))->setString("0");
	}
}


void ReportScene::ticketHeadAnimationCallback(float dt)
{
	//getChildByName(name::scene::report::POINT_TICKET_HEAD)->runAction(RotateBy::create(0.4f, -30));
	auto action = Sequence::createWithTwoActions(MoveBy::create(0.6f, Vec2(-40, 0)), MoveBy::create(0.3f, Vec2(1000, 0)));
	getChildByName(name::scene::report::POINT_TICKET_HEAD)->runAction(action);
}

void ReportScene::menuBackCallback(Ref* pSender)
{
	//Director::getInstance()->getEventDispatcher()->removeEventListenersForTarget(this);
	//runAction(MoveBy::create(0.4f, Vec2(720, 0)));

	//auto data = ValueMap();
	//data["w"] = w;
	//data["h"] = h;
	auto evtDispatcher = Director::getInstance()->getEventDispatcher();
	evtDispatcher->dispatchCustomEvent(config::event::ScrollToRightNotification/*, &data*/);
}