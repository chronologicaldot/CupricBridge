// Copyright 2018 Nicolaus Anderson

#include "cubr_guiwatcher.h"

namespace cubr {

GUIWatcher::GUIWatcher(
	Cu::Engine*  aEngine,
	gui_environment_t*  aEnvironment,
	gui_element_t*  aParent,
	irr::core::recti  aPos,
	irr::s32  id
)
	: gui_element_t(irr::gui::EGUIET_ELEMENT, aEnvironment, aParent, id, aPos)
	, engine(aEngine)
	, eventType(irr::gui::EGET_ELEMENT_FOCUS_LOST)
	, callback(REAL_NULL)
{}

bool
GUIWatcher::OnEvent(const irr::SEvent&  event) {
	if ( event.EventType == irr::EET_GUI_EVENT ) {
		if ( event.GUIEvent.EventType == eventType ) {
			return run();
		}
	}
	return IGUIElement::OnEvent(event);
}

void
GUIWatcher::setCallback( irr::gui::EGUI_EVENT_TYPE  aEventType, Cu::FunctionObject*  aCallback ) {
	if ( !aCallback ) return;

	aCallback->ref();
	if ( callback ) callback->deref();
	callback = aCallback;
	callback->changeOwnerTo(this);
	eventType = aEventType;
}

bool
GUIWatcher::owns( Cu::FunctionObject*  container ) const {
	return notNull(callback) && callback == container;
}

bool
GUIWatcher::run() {
	if ( callback && engine ) {
		engine->runFunctionObject(callback);
		return true;
	}
	return false;
}

/*Cu::ForeignFunc::Result
GUIWatcher::run() {
	if ( callback ) {
		switch ( engine.runFunctionObject(callback) ) {
		case EngineResult::Done:
			return Cu::ForeignFunc::EXIT;
		case EngineResult::Error:
			return Cu::ForeignFunc::NONFATAL;
		default:
			return Cu::ForeignFunc::FINISHED;
		}
	}
	return Cu::ForeignFunc::FINISHED;
}*/

GUIWatcherObject::GUIWatcherObject(
	Cu::Engine*  aEngine,
	gui_environment_t*  aEnvironment,
	gui_element_t*  aParent
)
	: GUIElement( new GUIWatcher(aEngine, aEnvironment, aParent, irr::core::recti(0,0,0,0), 0), aEnvironment)
	, watcher( nullptr )
{
	watcher = (GUIWatcher*) this->getElement();
	watcher->drop(); // Counters "new" + data.set() in GUIElement()
}

bool
GUIWatcherObject::supportsInterface( Cu::ObjectType::Value  type ) const {
	return type == GUIElement::getTypeAsCuType() || type == GUIWatcherObject::getTypeAsCuType();
}

Cu::ForeignFunc::Result
setGUIWatcherCallback( Cu::FFIServices& ffi ) {
	if ( ffi.demandArgCount(3)
		&& ffi.demandArgType(0, GUIWatcherObject::getTypeAsCuType())
		&& ffi.demandArgType(1, Cu::ObjectType::String)
		&& ffi.demandArgType(2, Cu::ObjectType::Function) )
	{
		((GUIWatcherObject&)ffi.arg(0)).setCallback(
			getGUIEventTypeFromString( ((Cu::StringObject&)ffi.arg(1)).getString() ),
			(Cu::FunctionObject*)&(ffi.arg(2))
		);
		return Cu::ForeignFunc::FINISHED;
	}
	return Cu::ForeignFunc::NONFATAL;
}


}
