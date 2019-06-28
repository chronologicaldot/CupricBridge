// Copyright 2018 Nicolaus Anderson

#ifndef _COPPER_BRIDGE_EVENT_H_
#define _COPPER_BRIDGE_EVENT_H_

#include <IEventReceiver.h>
#include <Copper.h>

namespace cubr {

//! Event Handler
/*
	Handles user event callbacks created in Copper.
	Normally, you would call the OnEvent of this inside that of an application-wide event handler.
*/
class EventHandler : public irr::IEventReceiver {
public:
	class EventCallback : public Cu::Owner, public Cu::ForeignFunc {
		Cu::FunctionObject*  callback;

	public:
		EventCallback()
			: callback(REAL_NULL)
		{}

		~EventCallback() {
			if ( callback ) {
				callback->disown(this);
				callback->deref();
			}
		}

		void registerAs( Cu::Engine&  engine, const util::String&  name ) {
			engine.addForeignFunction(name, this);
		}

		virtual Cu::ForeignFunc::Result
		call( Cu::FFIServices& ffi ) {
			if ( ! ffi.demandArgType(0, Cu::ObjectType::Function) )
				return Cu::ForeignFunc::NONFATAL;

			callback = &((Cu::FunctionObject&)ffi.arg(0));
			callback->ref();
			callback->changeOwnerTo(this);
			return Cu::ForeignFunc::FINISHED;
		}

		bool owns( Cu::FunctionObject*  container ) const {
			return callback && (callback == container);
		}

		bool run( Cu::Engine& engine, util::List<Cu::Object*>* args ) {
			if ( isNull(callback) )
				return false;

			return engine.runFunctionObject(callback, args) != Cu::EngineResult::Error;
		}
	};

private:

	Cu::Engine&  engine;
	EventCallback  guiEventCallbacks[irr::gui::EGET_COUNT];
	util::List<Cu::Object*>  arguments;

public:
	//! cstor
	EventHandler( Cu::Engine& );

	//! Irrlicht event handling
	virtual bool OnEvent(const irr::SEvent& event);

protected:
	//bool OnMouseInputEvent(const irr::SMouseInput& event);
	//bool OnKeyInputEvent(const irr::SKeyInput& event);
	bool OnGUIEvent(const irr::SEvent::SGUIEvent& event);
};

}

#endif
