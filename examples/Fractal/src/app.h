// 2018, Nicolaus Anderson

#ifndef APP_H
#define APP_H

#include <Copper.h>

using Cu::ForeignFunc;

/*
namespace Cu {
	class Engine;
	class FFIServices;
	class ForeignFunc;
}
*/

namespace irr {
	class IrrlichtDevice;
}

class AppCuInterface {
	Cu::Engine&  cuengine;
	irr::IrrlichtDevice*  device;

public:

	AppCuInterface( Cu::Engine&  engine, irr::IrrlichtDevice*  dev );

	ForeignFunc::Result  exitProgram( Cu::FFIServices& ffi );
	ForeignFunc::Result  print( Cu::FFIServices& ffi );
	ForeignFunc::Result  save_cu( Cu::FFIServices& ffi );
	ForeignFunc::Result  load_cu(Cu::FFIServices& ffi );
};

#endif
