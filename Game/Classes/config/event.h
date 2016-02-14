#ifndef __Config_Event_H__
#define __Config_Event_H__

namespace config {

	namespace event {
		/*! スロットアニメーションが止まった時に表示されているシンボル名を通知するイベント名 */
		const char DisplayedSymbolNameNotification[] = "DisplayedSymbolNameNotification";
		/*! Android端末の向きが変わったことを通知するイベント名 */
		const char OrientationChangedNotification[] = "OrientationChangedNotification";
		/*! Android端末の向きが変わったことを通知するイベント名 */
		//const char OrientationChangedNotificationForRegisterCene[] = "OrientationChangedNotificationForRegisterCene";
		/*! Android端末の向きが変わったことを通知するイベント名 */
		//const char OrientationChangedForGameScene[] = "OrientationChangedForGameScene";
		/*! １スクリーン分左へスクロールするイベント */
		const char ScrollToLeftNotification[] = "ScrollToLeftNotification";
		/*! 1スクリーン分右へスクロールするイベント */
		const char ScrollToRightNotification[] = "ScrollToRightNotification";
		/*! スクロールしてレポートシーンが完全に消えたことを通知するイベント名 */
		const char ScrollToRightCompletionNotification[] = "ScrollToRightCompletionNotification";
	}

}

#endif