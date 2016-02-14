#ifndef __Config_Font_H__
#define __Config_Font_H__

namespace config {

	/** @def defaultFont
	文字を出力するさいの標準システムフォント
	*/
	namespace font {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		const char Default[] = "MS Gothic";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		const char Default[] = "DEFAULT";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		const char Default[] = "Hirakaku - W3";
#endif
	}
}

#endif