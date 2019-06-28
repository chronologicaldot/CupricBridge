// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_GUI_WATCHER_H_
#define _CUBR_GUI_WATCHER_H_

#include "cubr_defs.h"
#include "cubr_base.h"
#include "cubr_irrevent_translate.h"

namespace cubr {

class GUIWatcher : public gui_element_t, public Cu::Owner {
	Cu::Engine*  engine;
	irr::gui::EGUI_EVENT_TYPE  eventType;
	Cu::FunctionObject*  callback;

public:
/*
	NOTE TO SELF: The tricky part is figuring out how to make its bounding box.
	If it takes the entire parent area, it may prevent other elements from being clicked on.
	Ideally, it would only take on the area occupied by its child. But what if more than
	one element is made its child? Should it calculate an area between the both of them?
	Should it have only one child?
	Maybe it should steal it's child's rectangle for itself and make its child expand to its borders
	(the borders the child would have had) using the alignments set to the near corners of the
	position corners (i.e. upperLeft goes to "upperLeft", lowerRight goes to "lowerRight").
	Override addChild().
	Or I could just create a foreign function for Copper called gui_expand(elem) that causes the
	given GUI element to expand to its parent's borders.
*/
	GUIWatcher( Cu::Engine*, gui_environment_t*, gui_element_t*, irr::core::recti, irr::s32 );

	virtual bool OnEvent(const irr::SEvent&  event);

	void setCallback( irr::gui::EGUI_EVENT_TYPE, Cu::FunctionObject* );

	virtual bool owns( Cu::FunctionObject*  container ) const;

	bool run();

};

Cu::ForeignFunc::Result
setGUIWatcherCallback( Cu::FFIServices& );

class GUIWatcherObject : public cubr::GUIElement {
	GUIWatcher*  watcher;

public:
	GUIWatcherObject( Cu::Engine*, gui_environment_t*, gui_element_t* );

	virtual bool
	supportsInterface( Cu::ObjectType::Value ) const;

	void setCallback( irr::gui::EGUI_EVENT_TYPE  eventType, Cu::FunctionObject*  callback ) {
		if ( watcher ) {
			watcher->setCallback( eventType, callback );
		}
	}

	static Cu::ObjectType::Value
	getTypeAsCuType() {
		return getCubrTypeAsCuType( CubrObjectType::GUIWatcher );
	}

	static void addSetterToEngine( Cu::Engine&  engine ) {
		Cu::addForeignFuncInstance(engine, "gui_set_callback", &setGUIWatcherCallback);
	}
};

}
#endif
