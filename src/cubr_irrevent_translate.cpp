// Copyright 2018 Nicolaus Anderson

#include "cubr_irrevent_translate.h"

namespace cubr {

irr::gui::EGUI_EVENT_TYPE
getGUIEventTypeFromString( const util::String&  eventName ) {
	irr::u32  i = 0;
	const irr::c8* const*  c = irr::gui::GUIEventTypeNames;
	for (; c[i]; ++i) {
		if ( eventName.equals(c[i]) )
			return static_cast<irr::gui::EGUI_EVENT_TYPE>(i);
	}
	return irr::gui::EGET_COUNT;
}

}
