#include "Slot.h"
#include "Name.h"
#include "name/resource.h"
#include "Config.h"
//#include <algorithm>

USING_NS_CC;

Slot::Slot(const std::string& name, float interval)
	: _name(name)
	, _interval(interval)
	, _playing(false)
{
}

Slot* Slot::create(const std::string& name, float interval)
{
	auto ret = new Slot(name, interval);

	if (ret && ret->init())
	{
		ret->autorelease();
		return ret;
	}
	CC_SAFE_DELETE(ret);
	return nullptr;
}

bool Slot::init()
{
	if (!Layer::init()){
		return false;
	}

	//_interval = 0.2f;
	_displayedTag = 0;

	setContentSize(Size(200, 200));

	std::vector<std::string> symbols = {
		name::resource::BREAD01,
		name::resource::BREAD02,
		name::resource::BREAD03,
		name::resource::BREAD04,
		name::resource::BREAD05,
		name::resource::BREAD06,
		name::resource::BREAD07,
		name::resource::BREAD08,
		name::resource::BREAD09,
		name::resource::BREAD10
	};

	// シンボル名の配列をシャッフル
	//std::random_shuffle(symbols.begin(), symbols.end());
	//std::shuffle(symbols.begin(), symbols.end(), std::default_random_engine());
	std::shuffle(symbols.begin(), symbols.end(), std::mt19937(std::random_device()()));

	auto cnt = 0;
	std::for_each(symbols.begin(), symbols.end(), [this, &cnt](const std::string& key) {
		auto tex = Director::getInstance()->getTextureCache()->getTextureForKey(key);
		auto sp = Sprite::createWithTexture(tex);
		sp->setName(key);
		sp->setTag(cnt);
		sp->setAnchorPoint(Vec2::ZERO);
		sp->setPosition(Vec2::ZERO);
		sp->setVisible(false);
		addChild(sp);
		cnt++;
	});

	getChildByTag(_displayedTag)->setVisible(true);

	// カスタムイベント登録
	// 隣のスロットシンボル名を教えてもらう
	/*Director::getInstance()->getEventDispatcher()->addCustomEventListener(AdjacentSymbolNameNotification, [this](EventCustom* evt){
		auto data = static_cast<ValueMap*>(evt->getUserData());
		CCLOG("%s: %s", data->at("where").asString().c_str(), data->at("symbol").asString().c_str());
	});*/

	schedule(schedule_selector(Slot::startAnimation), _interval);;

	return true;
}

void Slot::start()
{
	//schedule(schedule_selector(Slot::startAnimation), _interval);
	_playing = true;
}

void Slot::stop()
{
	stop("");
}

void Slot::stop(const std::string& target)
{
	//unschedule(schedule_selector(Slot::startAnimation));
	// ターゲットが指定されていれば、それを表示してスロットを止める
	if (!target.empty()){
		// 定義済みの確率に当たれば、ターゲットシンボルで止める
		auto hit = false;
		auto result = RandomHelper::random_real<float>(0, 1.0f);
		if (_name.compare(config::slot::SLOT_B) == 0) {
			if (result <= config::slot::SLOT_B_HIT_PROBABILITY){
				hit = true;
			}
		}
		else if (_name.compare(config::slot::SLOT_C) == 0) {
			if (result <= config::slot::SLOT_C_HIT_PROBABILITY) {
				hit = true;
			}
		}
		if (hit) {
			stopTarget(target);
		}
	}
	_playing = false;
	// 現在表示中のスロットシンボル名をカスタムイベントで通知
	auto data = ValueMap();
	data["where"] = _name;
	data["symbol"] = Value(getDisplayedSymbolName());
	getEventDispatcher()->dispatchCustomEvent(config::event::DisplayedSymbolNameNotification, &data);
}

void Slot::stopTarget(const std::string& target)
{
	std::vector<int> tags = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::for_each(tags.begin(), tags.end(), [this](const int& tag){
		auto node = static_cast<Sprite*>(getChildByTag(tag));
		//node->setLocalZOrder(1);
		node->setVisible(false);
	});
	auto targetNode = getChildByName(target);
	targetNode->setVisible(true);
	_displayedTag = targetNode->getTag();
}

void Slot::startAnimation(float dt)
{
	if (!_playing)
		return;

	std::vector<int> tags = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	std::for_each(tags.begin(), tags.end(), [this](const int& tag){
		auto node = static_cast<Sprite*>(getChildByTag(tag));
		//node->setLocalZOrder(1);
		node->setVisible(false);
	});
	_displayedTag++;
	if (_displayedTag > 9)
		_displayedTag = 0;
	//CCLOG("tag: %d", _displayedTag);
	getChildByTag(_displayedTag)->setVisible(true);
	//symbol->setLocalZOrder(0xff);
}

std::string Slot::getDisplayedSymbolName() const
{
	auto symbol = getChildByTag(_displayedTag);
	auto name = symbol->getName();
	return name;
}

Node* Slot::getDisplayedSymbolNode() const
{
	return getChildByTag(_displayedTag);
}