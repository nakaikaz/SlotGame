#ifndef __Name_Scene_H__
#define __Name_Scene_H__

namespace name {

	namespace scene {

		const char MAIN[] = "main_scene";
		const char REPORT[] = "report_scene";

		namespace regist {
			//const char INPUT_ID_PANEL[] = "input_id_panel";
			const char ID_FORM_PANEL[] = "id_form_panel";
			const char PLEASE_INPUT_ID[] = "please_input_id";
			const char BACKGROUND[] = "background";
		}

		namespace game {
			const char BACKGROUND_LAYER[] = "background_layer";
			const char CONTAINER_LAYER[] = "container_layer";
		}

		namespace main {
			const char EXIT_BUTTON[] = "exit_button";
			const char EXIT_BUTTON_SELECTED[] = "exit_button_selected";
			const char TOGGLE_ON[] = "toggle_on";
			const char TOGGLE_ON_SELECTED[] = "toggle_on_selected";
			const char TOGGLE_OFF[] = "toggle_off";
			const char TOGGLE_OFF_SELECTED[] = "toggle_off_selected";
			const char TICKET_SCENE_BUTTON[] = "ticket_scene_button";
			const char TICKET_SCENE_BUTTON_SELECTED[] = "ticket_scene_button_selected";
			const char SLOT_ACTION_BUTTON[] = "slot_action_button";
			const char SLOT_ACTION_BUTTON_SELECTED[] = "slot_action_button_selected";
			const char MENU[] = "main_scene_menu";

			const char SYMBOL_BACKGROUND_A[] = "symbol_background_a";
			const char SYMBOL_BACKGROUND_B[] = "symbol_background_b";
			const char SYMBOL_BACKGROUND_C[] = "symbol_background_c";

			const char SYMBOL_FRAME_A[] = "symbol_frame_a";
			const char SYMBOL_FRAME_B[] = "symbol_frame_b";
			const char SYMBOL_FRAME_C[] = "symbol_frame_c";

			const char SYMBOL_COVER_A[] = "symbol_cover_a";
			const char SYMBOL_COVER_B[] = "symbol_cover_b";
			const char SYMBOL_COVER_C[] = "symbol_cover_c";

			namespace landscape {
				const char CHALK_BOARD[] = "chalk_board landscape";
				const char TODAY_POINT[] = "today_point landscape";
				const char ADVISE_MESSAGE[] = "advise_message landscape";
				const char BYEBYE_MESSAGE[] = "byebye_message landscape";

				// カウントスプライトの名前
				const char LAST_COUNT_0[] = "last_count_0 landscape";
				const char LAST_COUNT_1[] = "last_count_1 landscape";
				const char LAST_COUNT_2[] = "last_count_2 landscape";
				const char LAST_COUNT_3[] = "last_count_3 landscape";

				// ポイントスプライト
				const char POINT_00[] = "0pt landscape";
				const char POINT_01[] = "1pt landscape";
				const char POINT_02[] = "2pt landscape";
				const char POINT_03[] = "3pt landscape";
				const char POINT_04[] = "4pt landscape";
				const char POINT_05[] = "5pt landscape";
				const char POINT_06[] = "6pt landscape";
				const char POINT_07[] = "7pt landscape";
				const char POINT_08[] = "8pt landscape";
				const char POINT_09[] = "9pt landscape";
				const char POINT_10[] = "10pt landscape";
			}

			namespace portrait {
				const char CHALK_BOARD[] = "chalk_board portrait";
				const char TODAY_POINT[] = "today_point portrait";
				const char ADVISE_MESSAGE[] = "advise_message portrait";
				const char BYEBYE_MESSAGE[] = "byebye_message portrait";

				// カウントスプライトの名前
				const char LAST_COUNT_0[] = "last_count_0 portrait";
				const char LAST_COUNT_1[] = "last_count_1 portrait";
				const char LAST_COUNT_2[] = "last_count_2 portrait";
				const char LAST_COUNT_3[] = "last_count_3 portrait";

				// ポイントスプライト
				const char POINT_00[] = "0pt portrait";
				const char POINT_01[] = "1pt portrait";
				const char POINT_02[] = "2pt portrait";
				const char POINT_03[] = "3pt portrait";
				const char POINT_04[] = "4pt portrait";
				const char POINT_05[] = "5pt portrait";
				const char POINT_06[] = "6pt portrait";
				const char POINT_07[] = "7pt portrait";
				const char POINT_08[] = "8pt portrait";
				const char POINT_09[] = "9pt portrait";
				const char POINT_10[] = "10pt portrait";
			}
		}

		namespace report {
			const char POINT_TICKET_HEAD[] = "point_ticket_head";
			const char POINT_TICKET_BODY[] = "point_ticket_body";
			const char LABEL_ID[] = "label_id";
			const char LABEL_POINT[] = "label_point";
			const char TO_MAIN_BUTTON[] = "to_main_button";
			const char REPORT_SCENE_MENU[] = "report_scene_menu";
		}

	}

}

#endif