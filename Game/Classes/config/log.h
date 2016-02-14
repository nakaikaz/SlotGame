#ifndef __Config_Log_H__
#define __Config_Log_H__

namespace config {

	namespace log {
		/*! プレイログを送信するWEB API URL */
		const char API_URL[] = "http://pan-club.jp/api/playlogs";
		/*! 未送信ログファイルを配列として保持するファイル名 */
		const char UNSENT_LOGS_FILENAME[] = "unsent.xml";
		/*! 未送信ログファイルの配列を値としてもつコンテナのキー名 */
		const char UNSENT_LOGS_KEY[] = "unsentList";
	}
}

#endif