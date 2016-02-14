#ifndef __Config_User_H__
#define __Config_User_H__

namespace config {

	/*! UserDefaultに記録するキー名 */
	namespace user {
		const char UUID[] = "uuid";
		const char LAST_PLAYED_DATE[] = "lastPlayedDate";
		const char REMAINING_PLAY_COUNT[] = "remainingPlayCount";
		const char UNCONFIRMED_POINT[] = "unConfirmedPoint";
		const char CURRENT_SCORE[] = "currentScore";
	}
}

#endif