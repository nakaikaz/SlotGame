#ifndef __MainScene_H__
#define __MainScene_H__

#include "cocos2d.h"
#include "network\HttpClient.h"
#include "GameState.h"
#include "Config.h"

class MainScene : public cocos2d::Layer
{
private:
	/*! 端末の向き */
	DeviceOrientation _deviceOrientation;

	/*! ゲームの状態（準備・プレイ中・停止中・アニメーション中） */
	GameState _gameState;

	const static std::string _API_URL;

	/*! プレイログを送信した後、サーバーからのレスポンスを待って終了するためのフラグ */
	bool _canExit;

	/*! ログは送信済みか */
	bool _isPosted;

	/*! 未送信ログが発生したか */
	bool _firstPostComplete;
	bool _secondPostComplete;
	bool _thirdPostComplete;

	/*! 未送信ログファイル名を格納する配列 */
	cocos2d::ValueVector _unsentLogNames;

	/*! 何回目のスピンか */
	int _spinCount;

	/*! 生成されている未送信ログファイル名 */
	std::string _temporaryUnsendLog;

	/*! 送信したログのサーバー上のID。アップデート時に使用 */
	int _currentLogId;

	/*! 今日の日付を取得する */
	std::string getDate();

	/*! 現在の日付時刻を取得する */
	std::string getDateTime();

	/*! 起動時にUserDefault等をチェックして起動できるか判定する */
	bool checkUserDefault();

	/*! 次に止めるべきスロットシンボルのターゲット */
	typedef struct _target_symbol {
		std::string slotName;
		std::string symbolName;
	} TargetSymbol;
	TargetSymbol _targetSymbol;

public:
	// there's no 'id' in cpp, so we recommend returning the class instance pointer
	static cocos2d::Scene* createScene();

	// Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
	virtual bool init();

	// implement the "static create()" method manually
	CREATE_FUNC(MainScene);

	void menuExitCallback(cocos2d::Ref* pSender);
	void waitForExitSignal(float dt);

	void createSymbolFrame(const cocos2d::Size& visibleSize);
	void menuSlotCallback(cocos2d::Ref* pSender);
	void createMessageBoard(const cocos2d::Size& visibleSize);
	void createMenuButton(const cocos2d::Size& visibleSize);
	void toggleGameModeCallback(cocos2d::Ref* pSender);

	void stopSlotA();
	void stopSlotBScheduler(float dt);
	void stopSlotCScheduler(float dt);

	void checkIfYouGotRepdigitScheduler(float dt);
	void updateLastPlayCountCallback(float dt);
	void repdigitAnimationCallback(float dt);
	void updateGameScoreCallback(float dt);

	void sendLogScheduler(float dt);
	void sendLogByPostMethod(const std::string& uuid, int point, const std::string& date, const std::string& fileName = "");
	void sendLogByPutMethod(int id, const std::string& uuid, int point, const std::string& date, const std::string& fileName = "");
	void onSendLogCompleteCallback(cocos2d::network::HttpClient* sender, cocos2d::network::HttpResponse* response);
	void onSendLogAndRemoveCallback(cocos2d::network::HttpClient* sender, cocos2d::network::HttpResponse* response);
	void sendLogIfLogExists();

	void reArrangeAllChildrenByLandscape();
	void reArrangeAllChildrenByPortrait();

	void symbolCoverFadeInScheduler(float dt);
};

#endif // __MainScene_H__