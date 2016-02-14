#include "MainScene.h"
#include "ReportScene.h"
#include "SimpleAudioEngine.h"
#include "Slot.h"
#include "GameValue.h"
#include "Name.h"
#include "Config.h"

#include <time.h>
#include "network/HttpClient.h"

#include "rapidjson/document.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"


USING_NS_CC;


Scene* MainScene::createScene()
{
	auto scene = Scene::create();
	auto layer = MainScene::create();
	scene->addChild(layer);
	return scene;
}

bool MainScene::init()
{
	if (!Layer::init()) {
		return false;
	}

	_canExit = true;
	_isPosted = false;
	_firstPostComplete = false;
	_secondPostComplete = false;
	_thirdPostComplete = false;
	_spinCount = 0;
	_currentLogId = 0;

	_gameState.set(GameState::STOPPED | GameState::GAME_MODE);

	Size visibleSize = Director::getInstance()->getVisibleSize();
	Vec2 origin = Director::getInstance()->getVisibleOrigin();


	// スロット背面
	auto cnt = 0;
	std::vector<std::string> symbolBkgs = { name::scene::main::SYMBOL_BACKGROUND_A, name::scene::main::SYMBOL_BACKGROUND_B, name::scene::main::SYMBOL_BACKGROUND_C };
	std::for_each(symbolBkgs.begin(), symbolBkgs.end(), [this, &cnt](const std::string& key){
		auto back = LayerColor::create(Color4B::WHITE);
		back->setName(key);
		back->setContentSize(Size(200, 300));
		back->setAnchorPoint(Vec2::ZERO);
		back->setPosition(30 + 230 * cnt, 312);
		addChild(back);
		cnt++;
	});

	// スロット
	cnt = 0;
	std::vector<std::string> slots = { config::slot::SLOT_A, config::slot::SLOT_B, config::slot::SLOT_C };
	std::for_each(slots.begin(), slots.end(), [this, &cnt](const std::string& name){
		auto slot = Slot::create(name, config::slot::SLOT_INTERVAL);
		slot->setName(name);
		slot->setAnchorPoint(Vec2::ZERO);
		slot->setPosition(30 + cnt * 230, 362);
		addChild(slot);
		cnt++;
	});

	createSymbolFrame(visibleSize);

	createMessageBoard(visibleSize);

	createMenuButton(visibleSize);

	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(config::audio::SPIN_EFFECT);
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(config::audio::STOP_EFFECT);
	CocosDenshion::SimpleAudioEngine::getInstance()->preloadEffect(config::audio::TICKET_EFFECT);

	// 未送信のログファイルがあれば送信
	sendLogIfLogExists();

	// UserDefault等をチェック。変数の初期化もここ。
	checkUserDefault();

	scheduleOnce(schedule_selector(MainScene::updateLastPlayCountCallback), 0);
	scheduleOnce(schedule_selector(MainScene::updateGameScoreCallback), 0);

	// 隣のスロットシンボル名の通知を受けるイベント
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::DisplayedSymbolNameNotification, [this](EventCustom* evt){
		auto data = static_cast<ValueMap*>(evt->getUserData());
		_targetSymbol.slotName = data->at("where").asString().c_str();
		_targetSymbol.symbolName = data->at("symbol").asString().c_str();
	});

	// 端末の向き変更イベント
	Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::OrientationChangedNotification, [this](EventCustom* evt){
		auto data = static_cast<ValueMap*>(evt->getUserData());
		auto w = data->at("w").asInt();
		auto h = data->at("h").asInt();

		if (w > h) {
			reArrangeAllChildrenByLandscape();
		}
		else{
			reArrangeAllChildrenByPortrait();
		}
	});

	return true;
}

void MainScene::reArrangeAllChildrenByLandscape()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	// スロット背面
	auto cnt = 0;
	std::vector<std::string> symbolBkgs = { name::scene::main::SYMBOL_BACKGROUND_A, name::scene::main::SYMBOL_BACKGROUND_B, name::scene::main::SYMBOL_BACKGROUND_C };
	std::for_each(symbolBkgs.begin(), symbolBkgs.end(), [this, &cnt](const std::string& key){
		getChildByName(key)->setPosition(550 + 230 * cnt, 230);
		cnt++;
	});
	cnt = 0;
	// スロット
	std::vector<std::string> slots = { config::slot::SLOT_A, config::slot::SLOT_B, config::slot::SLOT_C };
	std::for_each(slots.begin(), slots.end(), [this, &cnt](const std::string& name){
		getChildByName(name)->setPosition(550 + cnt * 230, 280);
		cnt++;
	});
	cnt = 0;
	// スロットカバー
	std::vector<std::string> coverNames = { name::scene::main::SYMBOL_COVER_A, name::scene::main::SYMBOL_COVER_B, name::scene::main::SYMBOL_COVER_C };
	std::for_each(coverNames.begin(), coverNames.end(), [this, &cnt](const std::string& key){
		getChildByName(key)->setPosition(550 + 230 * cnt, 230);
		cnt++;
	});
	cnt = 0;
	// フレーム
	std::vector<std::string> frames = { name::scene::main::SYMBOL_FRAME_A, name::scene::main::SYMBOL_FRAME_B, name::scene::main::SYMBOL_FRAME_C };
	std::for_each(frames.begin(), frames.end(), [this, &cnt](const std::string& key){
		getChildByName(key)->setPosition(541 + 230 * cnt, 221);
		cnt++;
	});
	// 黒板
	getChildByName(name::scene::main::landscape::CHALK_BOARD)->setVisible(true);
	getChildByName(name::scene::main::portrait::CHALK_BOARD)->setVisible(false);
	// メニュー
	auto menu = getChildByName(name::scene::main::MENU);
	menu->getChildByName(name::scene::main::EXIT_BUTTON)->setPosition(555, 619);
	//menu->getChildByName(name::scene::main::TOGGLE_ON)->setPosition(795, 619);
	menu->getChildByName(name::scene::main::TICKET_SCENE_BUTTON)->setPosition(1045, 619);
	menu->getChildByName(name::scene::main::SLOT_ACTION_BUTTON)->setPosition(546, 40);
}

void MainScene::reArrangeAllChildrenByPortrait()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();

	auto cnt = 0;
	std::vector<std::string> symbolBkgs = { name::scene::main::SYMBOL_BACKGROUND_A, name::scene::main::SYMBOL_BACKGROUND_B, name::scene::main::SYMBOL_BACKGROUND_C };
	std::for_each(symbolBkgs.begin(), symbolBkgs.end(), [this, &cnt](const std::string& key){
		getChildByName(key)->setPosition(30 + 230 * cnt, 312);
		cnt++;
	});
	cnt = 0;
	// スロット
	std::vector<std::string> slots = { config::slot::SLOT_A, config::slot::SLOT_B, config::slot::SLOT_C };
	std::for_each(slots.begin(), slots.end(), [this, &cnt](const std::string& name){
		getChildByName(name)->setPosition(30 + cnt * 230, 362);
		cnt++;
	});
	cnt = 0;
	// スロットカバー
	std::vector<std::string> coverNames = { name::scene::main::SYMBOL_COVER_A, name::scene::main::SYMBOL_COVER_B, name::scene::main::SYMBOL_COVER_C };
	std::for_each(coverNames.begin(), coverNames.end(), [this, &cnt](const std::string& key){
		getChildByName(key)->setPosition(30 + 230 * cnt, 312);
		cnt++;
	});
	cnt = 0;
	// フレーム
	std::vector<std::string> frames = { name::scene::main::SYMBOL_FRAME_A, name::scene::main::SYMBOL_FRAME_B, name::scene::main::SYMBOL_FRAME_C };
	std::for_each(frames.begin(), frames.end(), [this, &cnt](const std::string& key){
		getChildByName(key)->setPosition(20 + 230 * cnt, 303);
		cnt++;
	});
	// 黒板
	getChildByName(name::scene::main::landscape::CHALK_BOARD)->setVisible(false);
	getChildByName(name::scene::main::portrait::CHALK_BOARD)->setVisible(true);
	// メニュー
	auto menu = getChildByName(name::scene::main::MENU);
	menu->getChildByName(name::scene::main::EXIT_BUTTON)->setPosition(30, 1000);
	//menu->getChildByName(name::scene::main::TOGGLE_ON)->setPosition(280, 1000);
	menu->getChildByName(name::scene::main::TICKET_SCENE_BUTTON)->setPosition(530, 1000);
	menu->getChildByName(name::scene::main::SLOT_ACTION_BUTTON)->setPosition(26, 180);
}

bool MainScene::checkUserDefault()
{
	auto userDefault = UserDefault::getInstance();
	auto gameValue = GameValue::getInstance();

	gameValue->setUuid(userDefault->getStringForKey(config::user::UUID, ""));

	auto playedDate = userDefault->getStringForKey(config::user::LAST_PLAYED_DATE, "");
	auto today = getDate();

	if (today.compare(playedDate) == 0){
		auto remainingPlayCount = userDefault->getIntegerForKey(config::user::REMAINING_PLAY_COUNT);
		auto currentScore = userDefault->getIntegerForKey(config::user::CURRENT_SCORE);
		auto unConfirmedScore = userDefault->getIntegerForKey(config::user::UNCONFIRMED_POINT);
		gameValue->setRemainingPlayCount(remainingPlayCount);
		gameValue->setCurrentScore(currentScore);
		gameValue->setUnConfirmedScore(unConfirmedScore);
		// 残りプレイ回数は０ならば本日は終了
		if (0 == remainingPlayCount) {
			// UserDefaultのプレイ日付が本日の日付と等しく、プレイ回数が3回であれば、今日は既にプレイ済みなので終了
			_gameState.set(GameState::OVER);
			return false;
		}
		else if (0 < remainingPlayCount){
			// プレイ回数が3回になるまでは、プレイ可能
			return true;
		}
	}
	else {
		// UserDefaultのプレイ日付と本日の日付が異なれば、新たにプレイ
		auto currentScore = 0;
		// 昨日までの現在スコアは未確認スコア（ぱんくらぶスタッフに申告されてない）に加算される
		auto unConfirmedScore = userDefault->getIntegerForKey(config::user::UNCONFIRMED_POINT) + userDefault->getIntegerForKey(config::user::CURRENT_SCORE);
		auto remainingPlayCount = 3;
		userDefault->setStringForKey(config::user::LAST_PLAYED_DATE, today);
		userDefault->setIntegerForKey(config::user::REMAINING_PLAY_COUNT, remainingPlayCount);
		userDefault->setIntegerForKey(config::user::CURRENT_SCORE, currentScore);
		userDefault->setIntegerForKey(config::user::UNCONFIRMED_POINT, unConfirmedScore);
		userDefault->flush();
		gameValue->setRemainingPlayCount(remainingPlayCount);
		gameValue->setCurrentScore(currentScore);
		gameValue->setUnConfirmedScore(unConfirmedScore);
		return true;
	}
#if 0
	// UserDefaultから値を読み込み
	auto userDefault = UserDefault::getInstance();
	gameValue->setUuid(userDefault->getStringForKey(UUID, ""));
	gameValue->setLastPlayCount(userDefault->getIntegerForKey(REMAINING_PLAY_COUNT, 3));
	gameValue->setSavedScore(userDefault->getIntegerForKey(UNCONFIRMED_POINT, 0));
	bool alreadyPlayed = userDefault->getBoolForKey(ALREADY_PLAYED_TODAY, false);
	if (alreadyPlayed){
		// @TODO alreadyPlayedがtrueならば、日付を比較
		std::string playedDate = userDefault->getStringForKey(LAST_PLAYED_DATE);
		std::string today = getDate();
		if (today.compare(playedDate) == 0){
			_gameState.set(GameState::OVER);
			return false;
		}
		else {
			// alreadyPlayed==trueでも、記録された日付が今日の日付と異なれば起動できる。
			gameValue->setLastPlayCount(3);
			gameValue->setCurrentScore(0);
		}
	}
#endif 
	return false;
}

std::string MainScene::getDate()
{
	char date[24];
	time_t timer = time(nullptr);
	struct tm* local = localtime(&timer);
	::sprintf(date, "%2d-%02d-%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
	return date;
}

std::string MainScene::getDateTime()
{
	char date[24];
	time_t timer = time(nullptr);
	struct tm* local = localtime(&timer);
	::sprintf(date, "%2d-%02d-%02d %02d:%02d:%02d",
		local->tm_year + 1900, local->tm_mon + 1, local->tm_mday,
		local->tm_hour, local->tm_min, local->tm_sec);
	return date;
}

void MainScene::waitForExitSignal(float dt)
{
	CCLOG("waiting for exit signal");
	if (_canExit){
		//auto alreadyPlayed = UserDefault::getInstance()->getBoolForKey(ALREADY_PLAYED_TODAY, false);
		//if (_gameState.check(GameState::OVER) || 0 == gameValue->getCurrentScore() && !alreadyPlayed || alreadyPlayed){
		unschedule(schedule_selector(MainScene::waitForExitSignal));
		Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		exit(0);
#endif
		//}
	}
}

void MainScene::menuExitCallback(Ref* pSender)
{
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WP8) || (CC_TARGET_PLATFORM == CC_PLATFORM_WINRT)
	MessageBox("You pressed the close button. Windows Store Apps do not implement a close button.", "Alert");
	return;
#endif

	if (_gameState.check(GameState::ANIMATION))
		return;

	auto endActions = Vector<FiniteTimeAction*>();
	endActions.pushBack(CallFunc::create([=](){
		auto userDefault = UserDefault::getInstance();
		auto gameValue = GameValue::getInstance();
		std::string date = getDate();
		userDefault->setStringForKey(config::user::LAST_PLAYED_DATE, date);
		userDefault->setIntegerForKey(config::user::REMAINING_PLAY_COUNT, gameValue->getRemainingPlayCount());
		userDefault->setIntegerForKey(config::user::CURRENT_SCORE, gameValue->getCurrentScore());
		userDefault->setIntegerForKey(config::user::UNCONFIRMED_POINT, gameValue->getUnConfirmedScore());
		userDefault->flush();
	}));

	// 未送信ログファイル名の配列をファイルに書き出し
	endActions.pushBack(CallFunc::create([this](){
		ValueMap map = { { config::log::UNSENT_LOGS_KEY, Value(_unsentLogNames) } };
		std::string filepath = FileUtils::getInstance()->getWritablePath() + config::log::UNSENT_LOGS_FILENAME;
		FileUtils::getInstance()->writeToFile(map, filepath);
	}));
	// フェードアウトしてから終了（実際には、ブラックレイヤーの透明度を０にして終了）
	endActions.pushBack(FadeIn::create(1.0f));
	// 送信完了シグナルを待って終了
	endActions.pushBack(CallFunc::create([=](){
		schedule(schedule_selector(MainScene::waitForExitSignal), 0.2f);
	}));
#if 0
	auto alreadyPlayed = UserDefault::getInstance()->getBoolForKey(ALREADY_PLAYED_TODAY, false);
	if (_gameState.check(GameState::OVER) || 0 == gameValue->getCurrentScore() && !alreadyPlayed || alreadyPlayed){
		endActions.pushBack(CallFunc::create([](){
			Director::getInstance()->end();

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
			exit(0);
#endif
		}));
	}
#endif

	auto overlay = LayerColor::create(Color4B(0, 0, 0, 0));
	overlay->setPosition(Vec2::ZERO);
	addChild(overlay);
	overlay->runAction(Sequence::create(endActions));

}

void MainScene::createMenuButton(const Size& visibleSize)
{
	auto menuItems = Vector<MenuItem*>();
	auto cache = Director::getInstance()->getTextureCache();

	// 終了ボタン
	auto texNormal = cache->getTextureForKey(name::resource::EXIT_BUTTON);
	auto texSelected = cache->getTextureForKey(name::resource::EXIT_BUTTON_SELECTED);
	auto exitButton = MenuItemImage::create();
	exitButton->setName(name::scene::main::EXIT_BUTTON);
	exitButton->setNormalImage(Sprite::createWithTexture(texNormal));
	exitButton->setSelectedImage(Sprite::createWithTexture(texSelected));
	exitButton->setCallback(CC_CALLBACK_1(MainScene::menuExitCallback, this));
	exitButton->setAnchorPoint(Vec2::ZERO);
	exitButton->setPosition(30, visibleSize.height - 280);
	cache->removeTexture(texNormal);
	cache->removeTexture(texSelected);

	menuItems.pushBack(exitButton);

	// トグルボタン
	auto toggleOnNormalTex = cache->getTextureForKey(name::resource::TOGGLE_ON);
	auto toggleOnSelectedTex = cache->getTextureForKey(name::resource::TOGGLE_ON_SELECTED);
	auto toggleOffNormalTex = cache->getTextureForKey(name::resource::TOGGLE_OFF);
	auto toggleOffSelectedTex = cache->getTextureForKey(name::resource::TOGGLE_OFF_SELECTED);

	auto toggle = MenuItemImage::create();
	toggle->setNormalImage(Sprite::createWithTexture(toggleOnNormalTex));
	toggle->setSelectedImage(Sprite::createWithTexture(toggleOnSelectedTex));
	auto offItem = MenuItemImage::create();
	offItem->setNormalImage(Sprite::createWithTexture(toggleOffNormalTex));
	offItem->setSelectedImage(Sprite::createWithTexture(toggleOffSelectedTex));

	auto toggleButton = MenuItemToggle::createWithCallback(CC_CALLBACK_1(MainScene::toggleGameModeCallback, this), toggle, offItem, nullptr);
	toggleButton->setName(name::scene::main::TOGGLE_ON);
	toggleButton->setAnchorPoint(Vec2::ZERO);
	toggleButton->setPosition(280, visibleSize.height - 280);

	//menuItems.pushBack(toggleButton);

	cache->removeTexture(toggleOnNormalTex);
	cache->removeTexture(toggleOnSelectedTex);
	cache->removeTexture(toggleOffNormalTex);
	cache->removeTexture(toggleOffSelectedTex);

	// チケットシーンへのボタン
	texNormal = cache->getTextureForKey(name::resource::TICKET_SCENE_BUTTON);
	texSelected = cache->getTextureForKey(name::resource::TICKET_SCENE_BUTTON_SELECTED);
	auto ticketSceneButton = MenuItemImage::create();
	ticketSceneButton->setName(name::scene::main::TICKET_SCENE_BUTTON);
	ticketSceneButton->setNormalImage(Sprite::createWithTexture(texNormal));
	ticketSceneButton->setSelectedImage(Sprite::createWithTexture(texSelected));
	ticketSceneButton->setCallback([=](Ref* sender){
		if (_gameState.check(GameState::ANIMATION | GameState::PLAYING))
			return;
		//this->runAction(MoveBy::create(0.4f, Vec2(-720, 0)));
		//Director::getInstance()->pushScene(TransitionFade::create(0.2f, ReportScene::createScene(), Color3B::WHITE));
		//Director::getInstance()->pushScene(TransitionMoveInR::create(0.4f, ReportScene::createScene()));
		Director::getInstance()->getEventDispatcher()->dispatchCustomEvent(config::event::ScrollToLeftNotification);
	});
	ticketSceneButton->setAnchorPoint(Vec2::ZERO);
	ticketSceneButton->setPosition(Vec2(530, visibleSize.height - 280));

	menuItems.pushBack(ticketSceneButton);

	// スロットスピン・ストップボタン
	texNormal = cache->getTextureForKey(name::resource::SLOT_ACTION_BUTTON);
	texSelected = cache->getTextureForKey(name::resource::SLOT_ACTION_BUTTON_SELECTED);
	auto slotActionButton = MenuItemImage::create();
	slotActionButton->setName(name::scene::main::SLOT_ACTION_BUTTON);
	slotActionButton->setNormalImage(Sprite::createWithTexture(texNormal));
	slotActionButton->setSelectedImage(Sprite::createWithTexture(texSelected));
	slotActionButton->setCallback(CC_CALLBACK_1(MainScene::menuSlotCallback, this));
	slotActionButton->setAnchorPoint(Vec2::ZERO);
	slotActionButton->setPosition(26, 180);
	cache->removeTexture(texNormal);
	cache->removeTexture(texSelected);

	menuItems.pushBack(slotActionButton);

	auto menu = Menu::createWithArray(menuItems);
	menu->setName(name::scene::main::MENU);
	menu->setAnchorPoint(Vec2::ZERO);
	menu->setPosition(Vec2::ZERO);
	addChild(menu);
}

void MainScene::toggleGameModeCallback(Ref* pSender)
{
	if (_gameState.check(GameState::GAME_MODE)) {
		_gameState.reset(GameState::GAME_MODE);
	}
	else {
		_gameState.set(GameState::GAME_MODE);
	}
#if defined(COCOS2D_DEBUG)
	if (_gameState.check(GameState::GAME_MODE))
		CCLOG("set Game Mode...");
	else
		CCLOG("set Chance Mode...");
#endif
}

void MainScene::createSymbolFrame(const Size& visibleSize)
{
	auto cache = Director::getInstance()->getTextureCache();

	std::vector<std::string> frames = { name::scene::main::SYMBOL_FRAME_A, name::scene::main::SYMBOL_FRAME_B, name::scene::main::SYMBOL_FRAME_C };
	auto cnt = 0;
	auto tex = cache->getTextureForKey(name::resource::SYMBOL_FRAME);
	std::for_each(frames.begin(), frames.end(), [this, &cnt, tex](const std::string& key){
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(key);
		sp->setAnchorPoint(Vec2::ZERO);
		sp->setPosition(20 + 230 * cnt, 303);
		addChild(sp);
		cnt++;
	});
	cache->removeTexture(tex);

	cnt = 0;
	// @TODO スロットシンボルにかぶせるカバー・・・は、保留
	std::vector<std::string> coverNames = { name::scene::main::SYMBOL_COVER_A, name::scene::main::SYMBOL_COVER_B, name::scene::main::SYMBOL_COVER_C };
	tex = cache->getTextureForKey(name::resource::SYMBOL_COVER);
	//for (int i = 0; i < 3; i++) {
	std::for_each(coverNames.begin(), coverNames.end(), [this, &cnt, tex](const std::string& key){
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(key);
		sp->setAnchorPoint(Vec2::ZERO);
		sp->setPosition(30 + 230 * cnt, 312);
		sp->setOpacity(0);
		addChild(sp);
		cnt++;
	});
	cache->removeTexture(tex);
}

void MainScene::createMessageBoard(const Size& visibleSize)
{
	// portraitモードの黒板
	auto cache = Director::getInstance()->getTextureCache();
	auto tex = cache->getTextureForKey(name::resource::CHALK_BOARD_LANDSCAPE);
	auto portraitBoard = Sprite::createWithTexture(tex);
	portraitBoard->setName(name::scene::main::portrait::CHALK_BOARD);
	portraitBoard->setAnchorPoint(Vec2::ZERO);
	portraitBoard->setPosition(18, 640);
	addChild(portraitBoard);
	cache->removeTexture(tex);

	// お知らせラベル
	/*tex = cache->getTextureForKey(Resource::NOTICE_LABEL + ".png");
	auto notice = Sprite::createWithTexture(tex);
	notice->setPosition(Vec2(boardSize.width / 2, boardSize.height));
	portraitBoard->addChild(notice);
	cache->removeTexture(tex);*/

	// あなたのポイント
	tex = cache->getTextureForKey(name::resource::TODAY_POINT);
	auto yourpoint = Sprite::createWithTexture(tex);
	yourpoint->setName(name::scene::main::portrait::TODAY_POINT);
	yourpoint->setAnchorPoint(Vec2::ZERO);
	yourpoint->setPosition(50, 265);
	portraitBoard->addChild(yourpoint);
	// landscapeモードで使うのでここで削除しない
	//cache->removeTexture(tex);

	std::vector<std::string> points = { name::resource::POINT_00, name::resource::POINT_01, name::resource::POINT_02, name::resource::POINT_03,
		name::resource::POINT_04, name::resource::POINT_05, name::resource::POINT_06, name::resource::POINT_07, name::resource::POINT_08,
		name::resource::POINT_09, name::resource::POINT_10 };
	struct ResMap {
		std::string name;
		std::string png;
		ResMap(std::string v1, std::string v2){
			name = v1;
			png = v2;
		}
	};
	std::vector<ResMap> ptMap = {
		ResMap(name::scene::main::portrait::POINT_00, name::resource::POINT_00),
		ResMap(name::scene::main::portrait::POINT_01, name::resource::POINT_01),
		ResMap(name::scene::main::portrait::POINT_02, name::resource::POINT_02),
		ResMap(name::scene::main::portrait::POINT_03, name::resource::POINT_03),
		ResMap(name::scene::main::portrait::POINT_04, name::resource::POINT_04),
		ResMap(name::scene::main::portrait::POINT_05, name::resource::POINT_05),
		ResMap(name::scene::main::portrait::POINT_06, name::resource::POINT_06),
		ResMap(name::scene::main::portrait::POINT_07, name::resource::POINT_07),
		ResMap(name::scene::main::portrait::POINT_08, name::resource::POINT_08),
		ResMap(name::scene::main::portrait::POINT_09, name::resource::POINT_09),
		ResMap(name::scene::main::portrait::POINT_10, name::resource::POINT_10)
	};
	std::for_each(ptMap.begin(), ptMap.end(), [this, cache](const ResMap& map){
		auto png = map.png;
		auto name = map.name;
		auto tex = cache->getTextureForKey(png);
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(name);
		if (name.compare(name::scene::main::portrait::POINT_00) != 0) {
			sp->setOpacity(0);
		}
		sp->setAnchorPoint(Vec2::ZERO);
		sp->setPosition(440, 265);
		getChildByName(name::scene::main::portrait::CHALK_BOARD)->addChild(sp);
		// landscapeモードで使うのでここで削除しない
		//cache->removeTexture(tex);
	});

	tex = cache->getTextureForKey(name::resource::ADVISE_MESSAGE);
	auto advise = Sprite::createWithTexture(tex);
	advise->setName(name::scene::main::portrait::ADVISE_MESSAGE);
	advise->setAnchorPoint(Vec2::ZERO);
	advise->setPosition(50, 150);
	portraitBoard->addChild(advise);
	cache->removeTexture(tex);

	tex = cache->getTextureForKey(name::resource::BYEBYE_MESSAGE);
	auto byebye = Sprite::createWithTexture(tex);
	byebye->setName(name::scene::main::portrait::BYEBYE_MESSAGE);
	byebye->setOpacity(0);
	byebye->setAnchorPoint(Vec2::ZERO);
	byebye->setPosition(50, 40);
	portraitBoard->addChild(byebye);
	cache->removeTexture(tex);

	std::vector<ResMap> cntMap = {
		ResMap(name::scene::main::portrait::LAST_COUNT_0, name::resource::LAST_COUNT_0),
		ResMap(name::scene::main::portrait::LAST_COUNT_1, name::resource::LAST_COUNT_1),
		ResMap(name::scene::main::portrait::LAST_COUNT_2, name::resource::LAST_COUNT_2),
		ResMap(name::scene::main::portrait::LAST_COUNT_3, name::resource::LAST_COUNT_3)
	};
	std::for_each(cntMap.begin(), cntMap.end(), [this, cache](const ResMap& map){
		auto png = map.png;
		auto name = map.name;
		auto tex = cache->getTextureForKey(png);
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(name);
		// 「あと３回」だけ表示しておく
		if (name.compare(name::scene::main::portrait::LAST_COUNT_3) != 0) {
			sp->setOpacity(0);
		}
		sp->setAnchorPoint(Vec2::ZERO);
		sp->setPosition(50, 90);
		getChildByName(name::scene::main::portrait::CHALK_BOARD)->addChild(sp);
		// landscapeモードで使うのでここでは削除しない
		//cache->removeTexture(tex);
	});

	// landscapeモードの黒板
	tex = cache->getTextureForKey(name::resource::CHALK_BOARD_PORTRAIT);
	auto landscapeBoard = Sprite::createWithTexture(tex);
	landscapeBoard->setName(name::scene::main::landscape::CHALK_BOARD);
	landscapeBoard->setAnchorPoint(Vec2::ZERO);
	landscapeBoard->setPosition(60, 21.65);
	landscapeBoard->setVisible(false);
	addChild(landscapeBoard);
	cache->removeTexture(tex);

	tex = cache->getTextureForKey(name::resource::TODAY_POINT);
	yourpoint = Sprite::createWithTexture(tex);
	yourpoint->setName(name::scene::main::landscape::TODAY_POINT);
	yourpoint->setAnchorPoint(Vec2::ZERO);
	yourpoint->setPosition(38, 598);
	landscapeBoard->addChild(yourpoint);
	cache->removeTexture(tex);

	std::vector<ResMap> ptMap2 = {
		ResMap(name::scene::main::landscape::POINT_00, name::resource::POINT_00),
		ResMap(name::scene::main::landscape::POINT_01, name::resource::POINT_01),
		ResMap(name::scene::main::landscape::POINT_02, name::resource::POINT_02),
		ResMap(name::scene::main::landscape::POINT_03, name::resource::POINT_03),
		ResMap(name::scene::main::landscape::POINT_04, name::resource::POINT_04),
		ResMap(name::scene::main::landscape::POINT_05, name::resource::POINT_05),
		ResMap(name::scene::main::landscape::POINT_06, name::resource::POINT_06),
		ResMap(name::scene::main::landscape::POINT_07, name::resource::POINT_07),
		ResMap(name::scene::main::landscape::POINT_08, name::resource::POINT_08),
		ResMap(name::scene::main::landscape::POINT_09, name::resource::POINT_09),
		ResMap(name::scene::main::landscape::POINT_10, name::resource::POINT_10)
	};
	std::for_each(ptMap2.begin(), ptMap2.end(), [this, cache](const ResMap& map){
		auto png = map.png;
		auto name = map.name;
		auto tex = cache->getTextureForKey(png);
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(name);
		if (name.compare(name::scene::main::landscape::POINT_00) != 0) {
			sp->setOpacity(0);
		}
		sp->setAnchorPoint(Vec2(1, 0));
		sp->setPosition(380, 528);
		getChildByName(name::scene::main::landscape::CHALK_BOARD)->addChild(sp);
		cache->removeTexture(tex);
	});

	tex = cache->getTextureForKey(name::resource::ADVISE_MESSAGE_LANDSCAPE);
	advise = Sprite::createWithTexture(tex);
	advise->setName(name::scene::main::landscape::ADVISE_MESSAGE);
	advise->setAnchorPoint(Vec2::ZERO);
	advise->setPosition(38, 332);
	landscapeBoard->addChild(advise);
	cache->removeTexture(tex);

	std::vector<ResMap> cntMap2 = {
		ResMap(name::scene::main::landscape::LAST_COUNT_0, name::resource::LAST_COUNT_0),
		ResMap(name::scene::main::landscape::LAST_COUNT_1, name::resource::LAST_COUNT_1),
		ResMap(name::scene::main::landscape::LAST_COUNT_2, name::resource::LAST_COUNT_2),
		ResMap(name::scene::main::landscape::LAST_COUNT_3, name::resource::LAST_COUNT_3)
	};
	std::for_each(cntMap2.begin(), cntMap2.end(), [this, cache](const ResMap& map){
		auto png = map.png;
		auto name = map.name;
		auto tex = cache->getTextureForKey(png);
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(name);
		// 「あと３回」だけ表示しておく
		if (name.compare(name::scene::main::landscape::LAST_COUNT_3) != 0) {
			sp->setOpacity(0);
		}
		sp->setAnchorPoint(Vec2::ZERO);
		sp->setPosition(38, 254);
		getChildByName(name::scene::main::landscape::CHALK_BOARD)->addChild(sp);
		cache->removeTexture(tex);
	});

}

void MainScene::menuSlotCallback(Ref* pSender)
{
	// 当たり、またはゲームオーバーアニメーション中ならゲームスタート・ストップしない
#if 1
	if (_gameState.check(GameState::STOPPING | GameState::OVER | GameState::ALREADY_PLAYED | GameState::ANIMATION)) return;
#else
	if (_gameState.check(GameState::STOPPING | GameState::ALREADY_PLAYED | GameState::ANIMATION)) return;
#endif

	if (_gameState.check(GameState::STOPPED)) {
		static_cast<Slot*>(getChildByName(config::slot::SLOT_A))->start();
		static_cast<Slot*>(getChildByName(config::slot::SLOT_B))->start();
		static_cast<Slot*>(getChildByName(config::slot::SLOT_C))->start();
		CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(config::audio::SPIN_EFFECT);
		auto gameValue = GameValue::getInstance();
		gameValue->setRemainingPlayCount(gameValue->getRemainingPlayCount() - 1);
		scheduleOnce(schedule_selector(MainScene::updateLastPlayCountCallback), 0);
		_gameState.set(GameState::PLAYING);
		_gameState.reset(GameState::STOPPED);
		_spinCount += 1;
	}
	else if (_gameState.check(GameState::PLAYING)) {
		// まず、スロットAを止めて、１秒経過するごとに、スロットB、スロットCが止まるようにスケジューリング
		_gameState.set(GameState::STOPPING);
		_gameState.reset(GameState::PLAYING);
		stopSlotA();
	}
}

void MainScene::stopSlotA()
{
	static_cast<Slot*>(getChildByName(config::slot::SLOT_A))->stop();
	scheduleOnce(schedule_selector(MainScene::stopSlotBScheduler), 0.8f);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(config::audio::STOP_EFFECT);
}

void MainScene::stopSlotBScheduler(float dt)
{
	std::string target;
	// スロットＡのシンボルと比較
	if (_targetSymbol.slotName.compare(config::slot::SLOT_A) == 0) {
		target = _targetSymbol.symbolName;
	}
	static_cast<Slot*>(getChildByName(config::slot::SLOT_B))->stop(target);
	scheduleOnce(schedule_selector(MainScene::stopSlotCScheduler), 0.8f);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(config::audio::STOP_EFFECT);
}

void MainScene::stopSlotCScheduler(float dt)
{
	std::string target;
	// スロットＢのシンボルと比較
	if (_targetSymbol.slotName.compare(config::slot::SLOT_B) == 0) {
		target = _targetSymbol.symbolName;
	}
	static_cast<Slot*>(getChildByName(config::slot::SLOT_C))->stop(target);
	scheduleOnce(schedule_selector(MainScene::checkIfYouGotRepdigitScheduler), 0);
	CocosDenshion::SimpleAudioEngine::getInstance()->playEffect(config::audio::STOP_EFFECT);
	_gameState.set(GameState::STOPPED);
	_gameState.reset(GameState::STOPPING);
}

void MainScene::checkIfYouGotRepdigitScheduler(float dt)
{
	const auto SYMBOL_A_NAME = static_cast<Slot*>(getChildByName(config::slot::SLOT_A))->getDisplayedSymbolName();
	const auto SYMBOL_B_NAME = static_cast<Slot*>(getChildByName(config::slot::SLOT_B))->getDisplayedSymbolName();
	const auto SYMBOL_C_NAME = static_cast<Slot*>(getChildByName(config::slot::SLOT_C))->getDisplayedSymbolName();

	//CCLOG("A: %s, B: %s, C: %s", SYMBOL_A_NAME.c_str(), SYMBOL_B_NAME.c_str(), SYMBOL_C_NAME.c_str());

	auto gameValue = GameValue::getInstance();

	// 表示されたシンボル名を比較してあたり判定
	bool repdigit = SYMBOL_A_NAME.compare(SYMBOL_B_NAME) == 0 &&
		SYMBOL_B_NAME.compare(SYMBOL_C_NAME) == 0 &&
		SYMBOL_C_NAME.compare(SYMBOL_A_NAME) == 0;

	// ぞろ目のときの処理
	if (repdigit) {
		_gameState.set(GameState::ANIMATION);
		gameValue->setCurrentScore(gameValue->getCurrentScore() + 1);
		// アニメーションをスケジューリング
		scheduleOnce(schedule_selector(MainScene::repdigitAnimationCallback), 0);
		// ログを送信
		scheduleOnce(schedule_selector(MainScene::sendLogScheduler), 0);
	}

	// プレイカウントが０になったらゲーム終了
	if (0 == gameValue->getRemainingPlayCount()) {
		_gameState.set(GameState::OVER);
		scheduleOnce(schedule_selector(MainScene::updateLastPlayCountCallback), 0);
	}
}

void MainScene::symbolCoverFadeInScheduler(float dt)
{
	if (_gameState.check(GameState::ANIMATION))
		return;

	getChildByName(name::scene::main::SYMBOL_COVER_A)->runAction(Sequence::createWithTwoActions(DelayTime::create(0.8f), FadeIn::create(0.8f)));
	getChildByName(name::scene::main::SYMBOL_COVER_B)->runAction(Sequence::createWithTwoActions(DelayTime::create(0.8f), FadeIn::create(0.8f)));
	getChildByName(name::scene::main::SYMBOL_COVER_C)->runAction(Sequence::createWithTwoActions(DelayTime::create(0.8f), FadeIn::create(0.8f)));

	unschedule(schedule_selector(MainScene::symbolCoverFadeInScheduler));
}

void MainScene::updateLastPlayCountCallback(float dt)
{
	auto landscapeBoard = getChildByName(name::scene::main::landscape::CHALK_BOARD);
	auto portraitBoard = getChildByName(name::scene::main::portrait::CHALK_BOARD);
	if (_gameState.check(GameState::ALREADY_PLAYED)) {
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_0)->runAction(FadeOut::create(0));
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_1)->runAction(FadeOut::create(0));
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_2)->runAction(FadeOut::create(0));
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_3)->runAction(Sequence::createWithTwoActions(FadeOut::create(0),
			CallFunc::create([this, portraitBoard](){
			portraitBoard->getChildByName(name::scene::main::portrait::BYEBYE_MESSAGE)->runAction(FadeIn::create(0.4f));
			getChildByName(name::scene::main::SYMBOL_COVER_A)->setOpacity(0xff);
			getChildByName(name::scene::main::SYMBOL_COVER_B)->setOpacity(0xff);
			getChildByName(name::scene::main::SYMBOL_COVER_C)->setOpacity(0xff);
		})));
	}
	else if (_gameState.check(GameState::OVER)) {
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_0)->runAction(FadeOut::create(0.4f));
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_1)->runAction(FadeOut::create(0.4f));
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_2)->runAction(FadeOut::create(0.4f));
		portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_3)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([portraitBoard](){
			portraitBoard->getChildByName(name::scene::main::portrait::BYEBYE_MESSAGE)->runAction(FadeIn::create(0.4f));
		})));
		// @TODO GameState::Animation状態なら、アニメーションの完了を待ってシンボルカバーをかぶせる
		schedule(schedule_selector(MainScene::symbolCoverFadeInScheduler), 0.2f);
	}
	else{
		switch (GameValue::getInstance()->getRemainingPlayCount()){
		case 2:
			portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_3)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([portraitBoard](){
				portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_2)->runAction(FadeIn::create(0.4f));
			})));
			landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_3)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([landscapeBoard](){
				landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_2)->runAction(FadeIn::create(0.4f));
			})));
			break;
		case 1:
			portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_3)->runAction(FadeOut::create(0.4f));
			portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_2)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([portraitBoard](){
				portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_1)->runAction(FadeIn::create(0.4f));
			})));
			landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_3)->runAction(FadeOut::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_2)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([landscapeBoard](){
				landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_1)->runAction(FadeIn::create(0.4f));
			})));
			break;
		case 0:
			portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_1)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([portraitBoard](){
				portraitBoard->getChildByName(name::scene::main::portrait::LAST_COUNT_0)->runAction(FadeIn::create(0.4f));
			})));
			landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_1)->runAction(Sequence::createWithTwoActions(FadeOut::create(0.4f), CallFunc::create([landscapeBoard](){
				landscapeBoard->getChildByName(name::scene::main::landscape::LAST_COUNT_0)->runAction(FadeIn::create(0.4f));
			})));
			break;
		}
	}
}

void MainScene::updateGameScoreCallback(float dt)
{
	std::vector<std::string> portraitScores = { name::scene::main::portrait::POINT_00, name::scene::main::portrait::POINT_01, 
		name::scene::main::portrait::POINT_02, name::scene::main::portrait::POINT_03, name::scene::main::portrait::POINT_04, 
		name::scene::main::portrait::POINT_05, name::scene::main::portrait::POINT_06, name::scene::main::portrait::POINT_07,
		name::scene::main::portrait::POINT_08, name::scene::main::portrait::POINT_09, name::scene::main::portrait::POINT_10 };
	std::vector<std::string> landscapeScores = { name::scene::main::landscape::POINT_00, name::scene::main::landscape::POINT_01,
		name::scene::main::landscape::POINT_02, name::scene::main::landscape::POINT_03, name::scene::main::landscape::POINT_04,
		name::scene::main::landscape::POINT_05, name::scene::main::landscape::POINT_06, name::scene::main::landscape::POINT_07,
		name::scene::main::landscape::POINT_08, name::scene::main::landscape::POINT_09, name::scene::main::landscape::POINT_10 };

	auto actions = Vector<FiniteTimeAction*>();
	// １．全てのスコアスプライトをフェードアウト
	actions.pushBack(CallFunc::create([this, portraitScores, landscapeScores](){
		std::for_each(portraitScores.begin(), portraitScores.end(), [this](const std::string& name){
			getChildByName(name::scene::main::portrait::CHALK_BOARD)->getChildByName(name)->runAction(FadeOut::create(0.4f));
		});
		std::for_each(landscapeScores.begin(), landscapeScores.end(), [this](const std::string& name){
			getChildByName(name::scene::main::landscape::CHALK_BOARD)->getChildByName(name)->runAction(FadeOut::create(0.4f));
		});
	}));
	// ２．スコアに応じてスコアスプライトをフェードイン
	actions.pushBack(CallFunc::create([this](){
		auto landscapeBoard = getChildByName(name::scene::main::landscape::CHALK_BOARD);
		auto portraitBoard = getChildByName(name::scene::main::portrait::CHALK_BOARD);
		switch (GameValue::getInstance()->getCurrentScore()){
		case 0:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_00)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_00)->runAction(FadeIn::create(0.4f));
			break;
		case 1:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_01)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_01)->runAction(FadeIn::create(0.4f));
			break;
		case 2:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_02)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_02)->runAction(FadeIn::create(0.4f));
			break;
		case 3:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_03)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_03)->runAction(FadeIn::create(0.4f));
			break;
		case 4:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_04)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_04)->runAction(FadeIn::create(0.4f));
			break;
		case 5:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_05)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_05)->runAction(FadeIn::create(0.4f));
			break;
		case 6:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_06)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_06)->runAction(FadeIn::create(0.4f));
			break;
		case 7:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_07)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_07)->runAction(FadeIn::create(0.4f));
			break;
		case 8:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_08)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_08)->runAction(FadeIn::create(0.4f));
			break;
		case 9:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_09)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_09)->runAction(FadeIn::create(0.4f));
			break;
		case 10:
			portraitBoard->getChildByName(name::scene::main::portrait::POINT_10)->runAction(FadeIn::create(0.4f));
			landscapeBoard->getChildByName(name::scene::main::landscape::POINT_10)->runAction(FadeIn::create(0.4f));
			break;
		}
	}));
	this->runAction(Sequence::create(actions));

}

void MainScene::repdigitAnimationCallback(float dt)
{
	const auto SYMBOL_A = static_cast<Slot*>(getChildByName(config::slot::SLOT_A))->getDisplayedSymbolNode();
	const auto SYMBOL_B = static_cast<Slot*>(getChildByName(config::slot::SLOT_B))->getDisplayedSymbolNode();
	const auto SYMBOL_C = static_cast<Slot*>(getChildByName(config::slot::SLOT_C))->getDisplayedSymbolNode();

	const auto ORG_POS = SYMBOL_A->getPosition();

	auto firstActions = Vector<FiniteTimeAction*>();
	auto secondActions = Vector<FiniteTimeAction*>();
	auto thirdActions = Vector<FiniteTimeAction*>();

	const auto ROTATION_TIME = 0.4f;

	// 3. sprite03の回転アクション
	thirdActions.pushBack(CallFunc::create([=](){
		SYMBOL_C->setAnchorPoint(Vec2(0.5, 0.5));
		Size offset = SYMBOL_C->getContentSize();
		SYMBOL_C->setPosition(offset.width / 2, ORG_POS.y + offset.height / 2);
	}));
	thirdActions.pushBack(RotateBy::create(ROTATION_TIME, 360));
	thirdActions.pushBack(CallFunc::create([=](){
		SYMBOL_C->setAnchorPoint(Vec2(0, 0));
		SYMBOL_C->setPosition(ORG_POS);
	}));
	/*thirdActions.pushBack(CallFunc::create([=](){
	SYMBOL_A->runAction(Blink::create(1.0f, 2));
	SYMBOL_B->runAction(Blink::create(1.0f, 2));
	SYMBOL_C->runAction(Blink::create(1.0f, 2));
	}));*/
	thirdActions.pushBack(CallFunc::create([=](){
		scheduleOnce(schedule_selector(MainScene::updateGameScoreCallback), 0);
	}));
	thirdActions.pushBack(DelayTime::create(1.2f));
	thirdActions.pushBack(CallFunc::create([=](){
		// アニメーションフラグを解除
		_gameState.reset(GameState::ANIMATION);
	}));

	// ２．スロットBのシンボルを回転
	secondActions.pushBack(CallFunc::create([=](){
		SYMBOL_B->setAnchorPoint(Vec2(0.5, 0.5));
		Size offset = SYMBOL_B->getContentSize();
		SYMBOL_B->setPosition(offset.width / 2, ORG_POS.y + offset.height / 2);
	}));
	secondActions.pushBack(RotateBy::create(ROTATION_TIME, 360));
	secondActions.pushBack(CallFunc::create([=](){
		SYMBOL_B->setAnchorPoint(Vec2::ZERO);
		SYMBOL_B->setPosition(ORG_POS);
		SYMBOL_C->runAction(Sequence::create(thirdActions));
	}));

	// １．スロットAのシンボルを回転
	firstActions.pushBack(CallFunc::create([=](){
		SYMBOL_A->setAnchorPoint(Vec2(0.5, 0.5));
		Size offset = SYMBOL_A->getContentSize();
		SYMBOL_A->setPosition(offset.width / 2, ORG_POS.y + offset.height / 2);
	}));
	firstActions.pushBack(RotateBy::create(ROTATION_TIME, 360));
	firstActions.pushBack(CallFunc::create([=](){
		SYMBOL_A->setAnchorPoint(Vec2::ZERO);
		SYMBOL_A->setPosition(ORG_POS);
		SYMBOL_B->runAction(Sequence::create(secondActions));
	}));
	SYMBOL_A->runAction(Sequence::create(firstActions));
}

void MainScene::sendLogScheduler(float dt)
{
	auto date = getDateTime();

	auto gameValue = GameValue::getInstance();

	if (_isPosted)
		sendLogByPutMethod(_currentLogId, gameValue->getUuid(), gameValue->getCurrentScore(), date);
	else
		sendLogByPostMethod(gameValue->getUuid(), gameValue->getCurrentScore(), date);
}

void MainScene::sendLogByPostMethod(const std::string& uuid, int point, const std::string& date, const std::string& fileName)
{
	// 終了可フラグを元に戻す
	_canExit = false;
	auto req = new network::HttpRequest();
	std::string url = config::log::API_URL;
	req->setUrl(url.c_str());
	req->setRequestType(network::HttpRequest::Type::POST);
	std::vector<std::string> header;
	header.push_back("Content-Type: application/json");
	req->setHeaders(header);
	if (fileName.empty())
		req->setResponseCallback(CC_CALLBACK_2(MainScene::onSendLogCompleteCallback, this));
	else
		req->setResponseCallback(CC_CALLBACK_2(MainScene::onSendLogAndRemoveCallback, this));

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("uuid");
	writer.String(uuid.c_str());
	writer.String("point");
	writer.Int(point);
	writer.String("date");
	writer.String(date.c_str());
	if (!fileName.empty()){
		writer.String("filename");
		writer.String(fileName.c_str());
	}
	writer.EndObject();

	std::string postData = buffer.GetString();
	req->setRequestData(postData.c_str(), postData.length());
	req->setTag("playlog");

	network::HttpClient::getInstance()->send(req);
	req->release();
}

void MainScene::sendLogByPutMethod(int id, const std::string& uuid, int point, const std::string& date, const std::string& fileName)
{
	// 終了可フラグを元に戻す
	_canExit = false;
	auto req = new network::HttpRequest();
	auto url = StringUtils::format("%s/%d", config::log::API_URL, id);
	req->setUrl(url.c_str());
	req->setRequestType(network::HttpRequest::Type::PUT);
	std::vector<std::string> header;
	header.push_back("Content-Type: application/json");
	req->setHeaders(header);
	if (fileName.empty())
		req->setResponseCallback(CC_CALLBACK_2(MainScene::onSendLogCompleteCallback, this));
	else
		req->setResponseCallback(CC_CALLBACK_2(MainScene::onSendLogAndRemoveCallback, this));

	rapidjson::StringBuffer buffer;
	rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	writer.StartObject();
	writer.String("uuid");
	writer.String(uuid.c_str());
	writer.String("point");
	writer.Int(point);
	writer.String("date");
	writer.String(date.c_str());
	if (!fileName.empty()){
		writer.String("filename");
		writer.String(fileName.c_str());
	}
	writer.EndObject();

	std::string postData = buffer.GetString();
	req->setRequestData(postData.c_str(), postData.length());
	req->setTag("playlog");

	network::HttpClient::getInstance()->send(req);
	req->release();
}

void MainScene::onSendLogCompleteCallback(network::HttpClient* sender, network::HttpResponse* response)
{
	if (response->isSucceed()){
		_isPosted = true;
		// サーバからのレスポンスを文字列化
		auto buffer = response->getResponseData();
		std::string strResponse((*buffer).begin(), (*buffer).end());
		rapidjson::Document doc;
		doc.Parse(strResponse.c_str());
		CCASSERT(doc.HasMember("log"), "reseponse data doesn't have log!");
		CCASSERT(doc["log"].HasMember("id"), "log data doesn't have id!");
		_currentLogId = doc["log"]["id"].GetInt();
		//CCLOG("get log id: %d", _currentLogId);
		if (_spinCount == 1)
			_firstPostComplete = true;
		else if (_spinCount == 2)
			_secondPostComplete = true;
		else if (_spinCount == 3)
			_thirdPostComplete = true;

		// 現在のゲームで、2回か3回目のログ送信が成功しているのに、未送信ログがある場合は削除する
		if (_secondPostComplete || _thirdPostComplete){
			if (!_temporaryUnsendLog.empty()){
				std::string dir = FileUtils::getInstance()->getWritablePath();
				std::string filepath = dir + _temporaryUnsendLog;
				FileUtils::getInstance()->removeFile(filepath);
			}
		}
	}
	else {
		CCLOG("httpclient failed");
		// @TODO flatbuffersで対応
		// playlogの送信に失敗したら、データをファイルに書きこんで保存する
		auto gameValue = GameValue::getInstance();
		ValueMap data;
		data.insert(std::pair<std::string, Value>("id", Value(_currentLogId)));
		data.insert(std::pair<std::string, Value>("uuid", Value(gameValue->getUuid())));
		data.insert(std::pair<std::string, Value>("point", Value(gameValue->getCurrentScore())));
		data.insert(std::pair<std::string, Value>("date", Value(getDateTime())));

		auto fileName = getDate() + ".playlog";
		auto filepath = FileUtils::getInstance()->getWritablePath() + fileName;
		FileUtils::getInstance()->writeToFile(data, filepath);

		_temporaryUnsendLog = fileName;
		// 同じファイル名を追加しないようにする
		auto result = std::find_if(_unsentLogNames.begin(), _unsentLogNames.end(), [fileName](const Value& v){
			auto val = v.asString();
			return val.compare(fileName) == 0;
		});
		if (result == _unsentLogNames.end()){
			_unsentLogNames.push_back(Value(fileName));
		}

		CCASSERT(FileUtils::getInstance()->isFileExist(filepath), "failed createing playlog!");
	}

#if defined(COCOS2D_DEBUG)
	auto buffer = response->getResponseData();
	std::string str((*buffer).begin(), (*buffer).end());
	CCLOG("%s", str.c_str());
#endif

	_canExit = true;
}

void MainScene::sendLogIfLogExists()
{
	auto fileUtils = FileUtils::getInstance();
	auto filepath = fileUtils->getWritablePath() + config::log::UNSENT_LOGS_FILENAME;
	if (!fileUtils->isFileExist(filepath))
		return;

	auto map = fileUtils->getValueMapFromFile(filepath);
	auto vec = map.at(config::log::UNSENT_LOGS_KEY).asValueVector();

	std::for_each(vec.begin(), vec.end(), [this](const Value& v){
		auto fileName = v.asString();
		_unsentLogNames.push_back(Value(fileName));
		// ファイルを読み込んで、Dictionaryオブジェクトを作成
		auto filepath = FileUtils::getInstance()->getWritablePath() + fileName;

		auto map = FileUtils::getInstance()->getValueMapFromFile(filepath);
		auto id = map.at("id").asInt();
		auto uuid = map.at("uuid").asString();
		auto point = map.at("point").asInt();
		auto date = map.at("date").asString();

		//CCLOG("id: %d, uuid: %s, point: %d, date: %s", id, uuid.c_str(), point, date.c_str());

		if (id)
			sendLogByPutMethod(id, uuid, point, date, fileName);
		else
			sendLogByPostMethod(uuid, point, date, fileName);
	});
}

void MainScene::onSendLogAndRemoveCallback(network::HttpClient* sender, network::HttpResponse* response)
{
	if (response->isSucceed()) {
		auto buffer = response->getResponseData();
		std::string strResponse((*buffer).begin(), (*buffer).end());
		// @TODO playlogの送信に成功したら、playlogファイルを削除する
		rapidjson::Document doc;
		doc.Parse(strResponse.c_str());
		CCASSERT(doc.HasMember("filename"), "reseponse data doesn't have the filename!");
		std::string fileName = doc["filename"].GetString();
		CCLOG("%s: %s", __FUNCTION__, fileName.c_str());

		std::string filepath = FileUtils::getInstance()->getWritablePath() + fileName;
		if (FileUtils::getInstance()->removeFile(filepath)){
			// 未送信ログファイル名を格納している配列から、削除したファイル名を検索し、配列からも削除する
			auto result = std::find_if(_unsentLogNames.begin(), _unsentLogNames.end(), [this, fileName](const Value& v){
				return (fileName.compare(v.asString()) == 0);
			});
			if (result != _unsentLogNames.end()){
				_unsentLogNames.erase(result);
			}
		}
	}
#if defined(COCOS2D_DEBUG)
	else {
		CCLOG("failed: %s", __FUNCTION__);
	}
#endif

	_canExit = true;
}
