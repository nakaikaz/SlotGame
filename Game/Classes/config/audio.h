#ifndef __Config_Audio_H__
#define __Config_Audio_H__

#include "cocos2d.h"

namespace config {

	namespace audio {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
		const char SPIN_EFFECT[] = "spin.wav";
		const char STOP_EFFECT[] = "stop.wav";
		const char TICKET_EFFECT[] = "ticket.wav";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
		const char SPIN_EFFECT[] = "spin.ogg";
		const char STOP_EFFECT[] = "stop.ogg";
		const char TICKET_EFFECT[] = "ticket.ogg";
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
		const char SPIN_EFFECT[] = "spin.caf";
		const char STOP_EFFECT[] = "stop.caf";
		const char TICKET_EFFECT[] = "ticket.caf";
#endif
	}

}

#endif