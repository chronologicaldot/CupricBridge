// (C) 2018 Nicolaus Anderson

#include "cubr_event.h"
#include <IGUIElement.h>

namespace cubr {

using namespace irr::gui;

EventHandler::EventHandler( Cu::Engine& e )
	: engine(e)
{
	guiEventCallbacks[ EGET_ELEMENT_FOCUS_LOST		].registerAs(engine, "gui_on_focus_lost");
	guiEventCallbacks[ EGET_ELEMENT_FOCUSED			].registerAs(engine, "gui_on_focused");
	guiEventCallbacks[ EGET_ELEMENT_HOVERED			].registerAs(engine, "gui_on_hovered");
	guiEventCallbacks[ EGET_ELEMENT_LEFT			].registerAs(engine, "gui_on_left");
	guiEventCallbacks[ EGET_ELEMENT_CLOSED			].registerAs(engine, "gui_on_closed");
	guiEventCallbacks[ EGET_BUTTON_CLICKED			].registerAs(engine, "gui_on_button_clicked");
	guiEventCallbacks[ EGET_SCROLL_BAR_CHANGED		].registerAs(engine, "gui_on_scrollbar_changed");
	guiEventCallbacks[ EGET_CHECKBOX_CHANGED		].registerAs(engine, "gui_on_checkbox_changed");
	guiEventCallbacks[ EGET_LISTBOX_CHANGED			].registerAs(engine, "gui_on_listbox_changed");
	guiEventCallbacks[ EGET_LISTBOX_SELECTED_AGAIN	].registerAs(engine, "gui_on_listbox_selected_again");
	guiEventCallbacks[ EGET_FILE_SELECTED			].registerAs(engine, "gui_on_file_selected");
	guiEventCallbacks[ EGET_DIRECTORY_SELECTED		].registerAs(engine, "gui_on_directory_selected");
	guiEventCallbacks[ EGET_FILE_CHOOSE_DIALOG_CANCELLED].registerAs(engine, "gui_on_file_chooser_cancelled");
	guiEventCallbacks[ EGET_MESSAGEBOX_YES			].registerAs(engine, "gui_on_messagebox_yes");
	guiEventCallbacks[ EGET_MESSAGEBOX_NO			].registerAs(engine, "gui_on_messagebox_no");
	guiEventCallbacks[ EGET_MESSAGEBOX_OK			].registerAs(engine, "gui_on_messagebox_ok");
	guiEventCallbacks[ EGET_MESSAGEBOX_CANCEL		].registerAs(engine, "gui_on_messagebox_cancel");
	guiEventCallbacks[ EGET_EDITBOX_ENTER			].registerAs(engine, "gui_on_editbox_enter");
	guiEventCallbacks[ EGET_EDITBOX_CHANGED			].registerAs(engine, "gui_on_editbox_changed");
	guiEventCallbacks[ EGET_EDITBOX_MARKING_CHANGED	].registerAs(engine, "gui_on_editbox_marking_changed");
	guiEventCallbacks[ EGET_TAB_CHANGED				].registerAs(engine, "gui_on_tab_changed");
	guiEventCallbacks[ EGET_MENU_ITEM_SELECTED		].registerAs(engine, "gui_on_menu_item_selected");
	guiEventCallbacks[ EGET_COMBO_BOX_CHANGED		].registerAs(engine, "gui_on_combobox_changed");
	guiEventCallbacks[ EGET_SPINBOX_CHANGED			].registerAs(engine, "gui_on_spinbox_changed");
	guiEventCallbacks[ EGET_TABLE_CHANGED			].registerAs(engine, "gui_on_table_changed");
	guiEventCallbacks[ EGET_TABLE_HEADER_CHANGED	].registerAs(engine, "gui_on_table_header_changed");
	guiEventCallbacks[ EGET_TABLE_SELECTED_AGAIN	].registerAs(engine, "gui_on_table_selected_again");
	guiEventCallbacks[ EGET_TREEVIEW_NODE_DESELECT	].registerAs(engine, "gui_on_treeview_node_deselect");
	guiEventCallbacks[ EGET_TREEVIEW_NODE_SELECT	].registerAs(engine, "gui_on_treeview_node_select");
	guiEventCallbacks[ EGET_TREEVIEW_NODE_EXPAND	].registerAs(engine, "gui_on_treeview_node_expand");
	guiEventCallbacks[ EGET_TREEVIEW_NODE_COLLAPSE	].registerAs(engine, "gui_on_treeview_node_collapse");
}

bool
EventHandler::OnEvent(const irr::SEvent& event) {
	switch( event.EventType )
	{
	case irr::EET_GUI_EVENT:
		return OnGUIEvent(event.GUIEvent);

	default: break;
	}
	return false;
}

bool
EventHandler::OnGUIEvent(const irr::SEvent::SGUIEvent& event) {
	if ( !event.Caller )
		return false;

	Cu::Integer  callerId = event.Caller->getID();
	Cu::Integer  elemId = (event.Element? event.Element->getID() : -1);

	Cu::IntegerObject  callerIDObject(callerId);
	Cu::IntegerObject  elemIDObject(elemId);

	EventCallback&  cb = guiEventCallbacks[event.EventType];
	util::List<Cu::Object*>  args;
	args.push_back( &callerIDObject );
	args.push_back( &elemIDObject );
	Cu::Object*  returnObject;

	if ( cb.run( engine, &args ) ) {
		returnObject = engine.getLastObject();
		if ( Cu::isBoolObject(*returnObject) ) {
			return ((Cu::BoolObject*)returnObject)->getValue();
		}
	}
	return false;
}


}
