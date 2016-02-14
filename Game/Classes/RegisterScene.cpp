#include "RegisterScene.h"
#include "Name.h"
#include "GameScene.h"
#include "Config.h"

USING_NS_CC;


Scene* RegisterScene::createScene()
{
	auto scene = Scene::create();
	auto layer = RegisterScene::create();
	scene->addChild(layer);
	return scene;
}

bool RegisterScene::init()
{
	if (!Layer::init()) {
		return false;
	}

	Size visibleSize = Director::getInstance()->getVisibleSize();	

	auto cache = Director::getInstance()->getTextureCache();
	auto tex = cache->getTextureForKey(name::resource::GAME_BACKGROUND);
	auto gamePanel = Sprite::createWithTexture(tex);
	gamePanel->setName(name::scene::regist::BACKGROUND);
	gamePanel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(gamePanel);
	// ゲームシーンで使うのでキャッシュから削除しない
	//cache->removeTexture(tex);

	tex = cache->getTextureForKey(name::resource::ID_FORM_PANEL);
	auto inputIdPanel = Sprite::createWithTexture(tex);
	inputIdPanel->setName(name::scene::regist::ID_FORM_PANEL);
	inputIdPanel->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
	addChild(inputIdPanel);
	cache->removeTexture(tex);

	Size panelSize = inputIdPanel->getContentSize();

	tex = cache->getTextureForKey(name::resource::PLEASE_INPUT_ID);
	auto please = Sprite::createWithTexture(tex);
	please->setName(name::scene::regist::PLEASE_INPUT_ID);
	please->setPosition(panelSize.width / 2, panelSize.height - 50);
	please->setOpacity(0);
	inputIdPanel->addChild(please);
	cache->removeTexture(tex);

	auto editBox = ui::EditBox::create(Size(400, 70), ui::Scale9Sprite::create());
	editBox->setPosition(Vec2(panelSize.width / 2 + 40, panelSize.height / 2 + 40));
	editBox->setFont("fonts/NGO_____.ttf", 48);
	editBox->setFontColor(Color3B(0, 0, 0));
	editBox->setMaxLength(64);

/*#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
	std::string str = MultiByteToUTF8("ＩＤを入力してください");
	editBox->setPlaceHolder(str.c_str());
#else
	editBox->setPlaceHolder("ＩＤを入力してください。");
#endif*/
	editBox->setPlaceholderFont("fonts/NGO_____.ttf", 32);
	editBox->setPlaceholderFontColor(Color3B(100, 100, 100));
	editBox->setDelegate(this);

	editBox->setInputMode(ui::EditBox::InputMode::EMAIL_ADDRESS);

	editBox->setName("editBox");

	inputIdPanel->addChild(editBox);

	auto okayItem = MenuItemImage::create("", "", CC_CALLBACK_1(RegisterScene::menuOkayCallback, this));
	auto okayNormalTex = cache->getTextureForKey(name::resource::OKAY_ID_BUTTON);
	auto okaySelectedTex = cache->getTextureForKey(name::resource::OKAY_ID_BUTTON);
	okayItem->setNormalImage(Sprite::createWithTexture(okayNormalTex));
	okayItem->setSelectedImage(Sprite::createWithTexture(okaySelectedTex));
	cache->removeTexture(okayNormalTex);
	cache->removeTexture(okaySelectedTex);

	okayItem->setPosition(Vec2(panelSize.width / 2, panelSize.height / 2 - 90));

	// create menu, it's an autorelease object
	auto menu = Menu::createWithItem(okayItem);
	menu->setPosition(Vec2::ZERO);
	inputIdPanel->addChild(menu);

	// エディットボックスにフォーカスをあてる
	//editBox->touchDownAction(nullptr, ui::Widget::TouchEventType::ENDED);

	// 端末の向きの変更通知を受け取るイベント
	_customListener = Director::getInstance()->getEventDispatcher()->addCustomEventListener(config::event::OrientationChangedNotification, [this](EventCustom* evt){
		auto data = static_cast<ValueMap*>(evt->getUserData());
		auto w = data->at("w").asInt();
		auto h = data->at("h").asInt();

		auto glview = Director::getInstance()->getOpenGLView();

		// フレームサイズをセットしてから、デザイン解像度をセット
		glview->setFrameSize(w, h);
		if (w > h)
			glview->setDesignResolutionSize(config::resolution::DESIGN_RESOLUTION_LONG_SPAN, config::resolution::DESIGN_RESOLUTION_SHORT_SPAN, config::resolution::DESIGN_POLICY);
		else
			glview->setDesignResolutionSize(config::resolution::DESIGN_RESOLUTION_SHORT_SPAN, config::resolution::DESIGN_RESOLUTION_LONG_SPAN, config::resolution::DESIGN_POLICY);


		// 子要素として追加したスプライトやメニューボタンを再配置する
		reArrangeAllChildren();
	});


	return true;
}

void RegisterScene::onExit()
{
	Layer::onExit();
	// 登録シーン終了以降、OrientationChangedNotificationを受信しないようにイベントリスナをこのクラスから削除する
	if (_customListener)
		Director::getInstance()->getEventDispatcher()->removeEventListener(_customListener);
}

void RegisterScene::reArrangeAllChildren()
{
	auto visibleSize = Director::getInstance()->getVisibleSize();
	getChildByName(name::scene::regist::BACKGROUND)->setPosition(visibleSize.width / 2, visibleSize.height / 2);
	getChildByName(name::scene::regist::ID_FORM_PANEL)->setPosition(Vec2(visibleSize.width / 2, visibleSize.height / 2));
}

void RegisterScene::menuOkayCallback(Ref* pSender)
{
	if (_uuid.empty()){
		getChildByName(name::scene::regist::ID_FORM_PANEL)->getChildByName(name::scene::regist::PLEASE_INPUT_ID)->runAction(FadeIn::create(0.4f));
		return;
	}

	//@TODO UUIDを書き出し
	auto userDef = UserDefault::getInstance();
	userDef->setStringForKey(config::user::UUID, _uuid);
	userDef->flush();

	Director::getInstance()->replaceScene(TransitionFade::create(0.4f, GameScene::createScene(), Color3B::WHITE));
}

void RegisterScene::editBoxEditingDidBegin(ui::EditBox* editBox)
{
}

void RegisterScene::editBoxEditingDidEnd(ui::EditBox* editBox)
{
}

void RegisterScene::editBoxTextChanged(ui::EditBox* editBox, const std::string& text)
{
	_uuid = text;
}

void RegisterScene::editBoxReturn(ui::EditBox* editBox)
{
	CCLOG("%s", _uuid.c_str());
}
