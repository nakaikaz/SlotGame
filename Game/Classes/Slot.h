#ifndef __Slot_H__
#define __Slot_H__

#include "cocos2d.h"

class Slot : public cocos2d::Layer
{
private:
	/*! このスロットの識別子 */
	std::string _name;
	/*! シンボル名の配列 */
	//std::vector<std::string> _symbolNames;
	/*! スロットアニメーションの状態 */
	bool _playing;
	/*! スロットフレーム切り替えの速さ */
	float _interval;
	/*! 現在表示中のシンボルのタグ */
	int _displayedTag;
	/*! 初期化処理 */
	bool init();
	/*! スロットシンボルを恣意的に止める */
	void stopTarget(const std::string& target);

	void startAnimation(float dt);

	Slot(const std::string& name, float interval);

public:
	//CREATE_FUNC(Slot);
	static Slot* create(const std::string& name, float interval);

	void start();
	void stop();
	void stop(const std::string& target);
	bool isPlaying() const { return _playing; }

	std::string getDisplayedSymbolName() const;
	cocos2d::Node* getDisplayedSymbolNode() const;
};

#define AdjacentSymbolNameNotification "AdjacentSymbolNameNotification"

#endif // __Slot_H__