// (C) 2018 Nicolaus Anderson

#include <Copper.h>
#include <FileInStream.h>
#include <EngMsgToStr.h>
#include <irrlicht.h>
#include "../../src/cubridge.h"
#include "../../src/cubr_event.h"

struct ElementIDs {
enum Value {
	EXIT_BUTTON = 0,
};
};

struct Lg : public Cu::Logger {
	Cu::FileInStream&  stream;

	Lg( Cu::FileInStream&  fis )
		: stream(fis)
	{}

	virtual void print(const Cu::LogLevel::Value  logLevel, const char*  msg) {
		std::printf("In stream: %u, %u\n", stream.getLine(), stream.getColumn());
		switch(logLevel) {
		case Cu::LogLevel::info:
			std::printf("%s\n", msg);
			break;
		case Cu::LogLevel::warning:
			std::printf("\033[34m\033[103m Warning: %s\033[0m\n", msg);
			break;
		case Cu::LogLevel::error:
			std::printf("\33[41m ERROR\33[0m\n %s\n", msg);
			break;
		case Cu::LogLevel::debug:
			std::printf("\033[45m Debug \033[35m %s\033[0m\n", msg);
			break;
		default: break;
		}
	}

	virtual void print(const Cu::LogLevel::Value  logLevel, const Cu::EngineMessage::Value  msg) {
		Cu::EngineErrorLevel::Value errLevel;
		print(logLevel, Cu::getStringFromEngineMessage(msg, errLevel));
	}

	virtual void print(Cu::LogMessage  logMsg) {
		Cu::EngineErrorLevel::Value  errLevel;
		util::String  fullMsg = "In function ";

		fullMsg += logMsg.functionName;
		fullMsg += ": ";
		fullMsg += Cu::getStringFromEngineMessage(logMsg.messageId, errLevel);

		print(logMsg.level, fullMsg.c_str());
	}

	virtual void printTaskTrace( Cu::TaskType::Value  taskType, const util::String&  taskName, Cu::UInteger  taskNumber ) {
		std::printf("TASK TRACE: Type: %u, Name: %s, Number: %u", (unsigned)taskType, taskName.c_str(), taskNumber);
	}

	virtual void printStackTrace( const Cu::String&  frameName, Cu::UInteger  frameNumber ) {
		std::printf("STACK TRACE: Frame: %s, Number: %u", frameName.c_str(), frameNumber);
	}
};

class AppCuInterface {
	Cu::Engine&  cuengine;
	irr::IrrlichtDevice*  device;

public:

	AppCuInterface( Cu::Engine&  engine, irr::IrrlichtDevice*  dev )
		: cuengine(engine)
		, device(dev)
	{
		const util::String
				sn_exit("close_application"),
				sn_print("print");

		Cu::addForeignMethodInstance<AppCuInterface>(engine, sn_exit, this, &AppCuInterface::exitProgram);
		Cu::addForeignMethodInstance<AppCuInterface>(engine, sn_print, this, &AppCuInterface::print);
	}

	Cu::ForeignFunc::Result exitProgram( Cu::FFIServices& ffi ) {
		if ( device )
			device->closeDevice();
		return Cu::ForeignFunc::FINISHED;
	}

	Cu::ForeignFunc::Result print( Cu::FFIServices& ffi ) {
		Cu::Object* obj;
		util::String s;
		Cu::Integer i = 0;
		while ( i < ffi.getArgCount() ) {
			obj = ffi.arg(i);
			obj->writeToString(s);
			std::printf("%s", s.c_str());
		}
		return Cu::ForeignFunc::FINISHED;
	}

};

int main() {

	irr::core::dimension2du  screenSize(900,600);
	irr::IrrlichtDevice*  device = irr::createDevice(irr::video::EDT_OPENGL, screenSize);

	if ( !device ) {
		return 1;
	}

	// Another option:
	//Cu::StringInStream  sis( fileToCuStr("test.cu", device->getFileSystem()) );
	Cu::FileInStream  cfistream("test.cu");
	Lg logger(cfistream);
	Cu::Engine  cuengine;
	cuengine.setLogger(&logger);

	cubr::EventHandler ceh(cuengine);
	device->setEventReceiver(&ceh);

	std::printf("post construct cfistream, logger, engine, and event handler\n");

	cubr::CuBridge  cubridge = cubr::CuBridge(cuengine, device->getGUIEnvironment(), nullptr);

	std::printf("post construct cubridge\n");

	AppCuInterface  aci(cuengine, device);

	std::printf("post construct AppCuInterface\n");

	Cu::EngineResult::Value  erv;
	do {
		erv = cuengine.run(cfistream);
		std::printf("stream at (%u,%u)", cfistream.getLine(), cfistream.getColumn());
	} while ( erv == Cu::EngineResult::Ok );
	if ( erv == Cu::EngineResult::Error )
		return 1;

	std::printf("post call engine\n");

	//device->getGUIEnvironment()->addButton(irr::core::recti(300,300,600,600));

	while ( device->run() ) {
		device->getVideoDriver()->beginScene();
		device->getGUIEnvironment()->drawAll();
		device->getVideoDriver()->endScene();
	}

	return 0;
}
