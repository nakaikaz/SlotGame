#ifndef __Game_State_H__
#define __Game_State_H__

class GameState
{
private:
	unsigned short _state;

public:
	GameState() : _state(0) {}

	/*! ゲーム準備OK */
	//static const unsigned short READY;
	/*! スロット回転中 */
	static const unsigned short PLAYING;
	/*! スロットは止まりつつある */
	static const unsigned short STOPPING;
	/*! スロット停止中 */
	static const unsigned short STOPPED;
	/*! ゲームオーバー */
	static const unsigned short OVER;
	/*! 今日はプレイ済み */
	static const unsigned short ALREADY_PLAYED;
	/*! アニメーション中 */
	static const unsigned short ANIMATION;
	/*! ゲームモード */
	static const unsigned short GAME_MODE;

	/*! フラッグをセットする */
	void set(unsigned short state, unsigned short remove = 0)
	{
		if (remove)
			reset(remove);
		_state |= state;
	}

	/*! フラグを削除する */
	void reset(unsigned short state)
	{
		_state &= ~state;
	}

	/*! フラグがたっているかチェック */
	bool check(unsigned short state) const
	{
		return (_state & state) != 0 ? true : false;
	}

	unsigned short operator()() const { return _state; }
};

#endif // __Game_State_H__