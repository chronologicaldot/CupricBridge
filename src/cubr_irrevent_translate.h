// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_IRR_EVENT_TRANSLATE_H_
#define _CUBR_IRR_EVENT_TRANSLATE_H_

#include <IEventReceiver.h> // Irrlicht file
#include <Strings.h> // Copper file

namespace irr {
namespace gui {

const irr::c8* const GUIEventTypeNames[] = {
	"focus lost",
	"focus",
	"hover",
	"left",
	"close",
	"button click",
	"scrollbar change",
	"checkbox change",
	"listbox change",
	"listbox select again",
	"file select",
	"directory select",
	"file dialog cancel",
	"messagebox yes",
	"messagebox no",
	"messagebox ok",
	"messagebox cancel",
	"editbox enter",
	"editbox change",
	"editbox marking change",
	"tab change",
	"menu item select",
	"combobox change",
	"spinbox change",
	"table change",
	"table header change",
	"table select again",
	"treeview node deselect",
	"treeview node select",
	"treeview node expand",
	"treeview node collapse",
	0
};

}}

namespace cubr {

irr::gui::EGUI_EVENT_TYPE
getGUIEventTypeFromString( const util::String&  eventName );

}

#endif
