#include "GameValue.h"
#include "time.h"

static GameValue* s_gameValue = nullptr;

GameValue* GameValue::getInstance()
{
	if (!s_gameValue){
		s_gameValue = new (std::nothrow)  GameValue();
		s_gameValue->init();
	}
	return s_gameValue;
}

GameValue::GameValue()
: _currentScore(0)
, _unConfirmedScore(0)
, _remainingPlayCount(3)
{
}

void GameValue::destroyInstance()
{
	release();
}

void GameValue::init()
{
	char date[24];
	time_t timer = time(nullptr);
	struct tm* local = localtime(&timer);
	::sprintf(date, "%2d-%02d-%02d", local->tm_year + 1900, local->tm_mon + 1, local->tm_mday);
	_date = date;
}

void GameValue::setUuid(const std::string& val)
{
	_uuid = val;
}

const std::string& GameValue::getUuid() const
{
	return _uuid;
}

const std::string& GameValue::getDate() const
{
	return _date;
}

void GameValue::setCurrentScore(int val)
{
	_currentScore = val;
}

int GameValue::getCurrentScore()
{
	return _currentScore;
}

void GameValue::setUnConfirmedScore(int val)
{
	_unConfirmedScore = val;
}

int GameValue::getUnConfirmedScore()
{
	return _unConfirmedScore;
}

void GameValue::setRemainingPlayCount(int val)
{
	_remainingPlayCount = val;
}

int GameValue::getRemainingPlayCount()
{
	return _remainingPlayCount;
}

void GameValue::clearTotalScore()
{
	setCurrentScore(0);
	setUnConfirmedScore(0);
}

int GameValue::getTotalScore()
{
	return getCurrentScore() + getUnConfirmedScore();
}