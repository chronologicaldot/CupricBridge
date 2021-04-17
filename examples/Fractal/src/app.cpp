// 2018, Nicolaus Anderson

#include "app.h"
#include <irrlicht.h>
#include <Copper.h>

#define APP_INCLUDE_STDPRINT
#ifdef NO_APP_INCLUDE_STDPRINT
#undef APP_INCLUDE_STDPRINT
#endif

#ifdef APP_INCLUDE_STDPRINT
#include <cstdio>
#endif

AppCuInterface::AppCuInterface( Cu::Engine&  engine, irr::IrrlichtDevice*  dev )
	: cuengine(engine)
	, device(dev)
{
	const util::String
			sn_exit("close_application"),
			sn_print("print"),
			sn_save("save_cu"),
			sn_load("load_cu");

	Cu::addForeignMethodInstance<AppCuInterface>(engine, sn_exit, this, &AppCuInterface::exitProgram);
	Cu::addForeignMethodInstance<AppCuInterface>(engine, sn_print, this, &AppCuInterface::print);
	Cu::addForeignMethodInstance<AppCuInterface>(engine, sn_save, this, &AppCuInterface::save_cu);
	Cu::addForeignMethodInstance<AppCuInterface>(engine, sn_load, this, &AppCuInterface::load_cu);
}

ForeignFunc::Result
AppCuInterface::exitProgram( Cu::FFIServices& ffi ) {
	if ( device )
		device->closeDevice();
	//return ForeignFunc::FINISHED;
	return ForeignFunc::EXIT; // Override any other activity
}

ForeignFunc::Result
AppCuInterface::print( Cu::FFIServices& ffi ) {
	util::String  s;
	Cu::UInteger  argCount = ffi.getArgCount();
	Cu::UInteger  i = 0;
	for (; i < argCount; ++i) {
		ffi.arg(i).writeToString(s);
#ifdef APP_INCLUDE_STDPRINT
		std::printf("%s", s.c_str());
#else
		device->getLogger()->log(s.c_str(), irr::ELL_INFORMATION);
#endif
	}
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
AppCuInterface::save_cu( Cu::FFIServices& ffi ) {
	std::printf("Save not yet implemented.\n");
	return ForeignFunc::FINISHED;
}

ForeignFunc::Result
AppCuInterface::load_cu(Cu::FFIServices& ffi ) {
	std::printf("Load not yet implemented.\n");
	return ForeignFunc::FINISHED;
}
