#ifndef __Config_Slot_H__
#define __Config_Slot_H__

namespace config {

	namespace slot {
		/*! スロット識別子 */
		const char SLOT_A[] = "SLOT_A";
		const char SLOT_B[] = "SLOT_B";
		const char SLOT_C[] = "SLOT_C";
		/*! スロットアニメーション速さ */
		const float SLOT_INTERVAL = 0.1f;
		/*! スロットあたりの確率 */
		const float SLOT_B_HIT_PROBABILITY = 0.35f;
		const float SLOT_C_HIT_PROBABILITY = 0.25f;
	}
}

#endif