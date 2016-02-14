#ifndef __Config_Resolution_H__
#define __Config_Resolution_H__

#include "cocos2d.h"

namespace config {

	/*! デザイン解像度、デザインポリシー*/
	namespace resolution {
		const int DESIGN_RESOLUTION_LONG_SPAN = 1280;
		const int DESIGN_RESOLUTION_SHORT_SPAN = 720;
		const ResolutionPolicy DESIGN_POLICY = ResolutionPolicy::SHOW_ALL;
	}

}

#endif