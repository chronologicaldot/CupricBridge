// (C) 2018 Nicolaus Anderson

#include <Copper.h>

namespace cubr {

// Set this value according to your needs
#define CUBR_MESSAGE_CODE_START 1

struct CuBridgeMessageCode {
	enum Value {
		START = CUBR_MESSAGE_CODE_START,

		//! Info - Child GUI element not found
		GUIElementChildNotFound,

		//! C++ Error - A GUI element was empty (not set)
		GUIElementIsEmpty,

		//! Warning - GUI element factory cannot produce the requested element
		GUIElementCannotBeCreated,

		//! Warning - GUI Watcher construction did not receive the correct arguments.
		// Are the call arguments are in the wrong order?
		GUIWatcherWrongConstructionArgs,

		//! Warning - GUI element did not have a parent
		GUIElementLacksParent,

		//! A useful constant
		LAST
	};
};

} // end namespace cubr
