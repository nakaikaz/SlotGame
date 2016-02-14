#ifndef __GameValue_H__
#define __GameValue_H__

#include "cocos2d.h"
#include <string>

/*! シーン間で参照可能なデータ */
class GameValue : public cocos2d::Ref
{
private:
	GameValue();
	~GameValue(){}
	// コピーコンストラクタ使用禁止
	GameValue(const GameValue& obj);
	// 代入演算子使用禁止
	const GameValue& operator= (const GameValue& obj);

	void init();

	// ID
	CC_PROPERTY_PASS_BY_REF(std::string, _uuid, Uuid);
	// 今日の日付
	CC_PROPERTY_READONLY_PASS_BY_REF(std::string, _date, Date);
	// ゲーム中スコア
	CC_PROPERTY(int, _currentScore, CurrentScore);
	// 保存データのスコア
	CC_PROPERTY(int, _unConfirmedScore, UnConfirmedScore);
	// プレイ可能回数
	CC_PROPERTY(int, _remainingPlayCount, RemainingPlayCount);

public:
	static GameValue* getInstance();
	void destroyInstance();
	int getTotalScore();
	void clearTotalScore();
};

#endif // __GameValue_H__