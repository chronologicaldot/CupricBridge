// (C) 2018 Nicolaus Anderson

#include <Copper.h>
#include <FileInStream.h>
#include <EngMsgToStr.h>
#include <irrlicht.h>
#include "../../src/cubridge.h"
#include "../../src/cubr_event.h"
#include "../../src/cubr_mfrunner.h"

struct ElementIDs {
enum Value {
	EXIT_BUTTON = 0,
};
};

struct Lg : public Cu::Logger {

	cubr::MultifileRunner&  mfrunner;

	Lg( cubr::MultifileRunner&  mfr )
		: mfrunner(mfr)
	{}

	virtual void print(const Cu::LogLevel::Value  logLevel, const char*  msg) {
		//std::printf("[%u, %u] %s", mfrunner.getLastLine(), mfrunner.getLastColumn(), msg);
		std::printf("\033[44m[%u, %u]\033[0m", mfrunner.getLastLine(), mfrunner.getLastColumn());
		switch(logLevel) {
		case Cu::LogLevel::info:
			std::printf(" %s\n", msg);
			break;
		case Cu::LogLevel::warning:
			std::printf("\033[34m\033[103m Warning \033[0m\033[93m %s\033[0m\n", msg);
			break;
		case Cu::LogLevel::error:
			std::printf("\33[41m ERROR \033[0m\33[91m %s\033[0m\n", msg);
			break;
		case Cu::LogLevel::debug:
			std::printf("\033[45m Debug \033[0m\033[35m %s\033[0m\n", msg);
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

		if ( logMsg.messageId == Cu::EngineMessage::CustomMessage ) {
			if ( logMsg.customCode == cubr::MultifileRunner::MessageCode::FileNonExistent ) {
				fullMsg += "File does not exist";
			}
			else if ( logMsg.customCode == cubr::MultifileRunner::MessageCode::ImportUsageInWrongContext ) {
				fullMsg += "Import used in the wrong context";
			}
			else if ( logMsg.customCode == cubr::MultifileRunner::MessageCode::RequireUsageInWrongContext ) {
				fullMsg += "Require used in the wrong context";
			}
			else if ( logMsg.customCode == cubr::MultifileRunner::MessageCode::InvalidPathArg ) {
				fullMsg += "Invalid path given";
			}
		}
		else {
			fullMsg += Cu::getStringFromEngineMessage(logMsg.messageId, errLevel);
		}

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

	Cu::ForeignFunc::Result
	exitProgram( Cu::FFIServices& ffi ) {
		if ( device )
			device->closeDevice();
		return Cu::ForeignFunc::EXIT;
	}

	Cu::ForeignFunc::Result
	print( Cu::FFIServices& ffi ) {
		util::String s;
		Cu::UInteger  a = 0;
		for (; a < ffi.getArgCount(); ++a ) {
			ffi.arg(a).writeToString(s);
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

	Cu::Engine  cuengine;

	cubr::MultifileRunner  mfrunner(cuengine);
	mfrunner.setRootDirectoryPath("./scripts");

	Lg logger(mfrunner);
	cuengine.setLogger(&logger);

	cubr::EventHandler ceh(cuengine);
	device->setEventReceiver(&ceh);

	cubr::CuBridge  cubridge = cubr::CuBridge(cuengine, device->getGUIEnvironment(), nullptr);

	AppCuInterface  aci(cuengine, device);

	Cu::EngineResult::Value  erv;
	bool  result = mfrunner.run("main.cu");

	if ( !result ) {
		switch( mfrunner.getErrorFlags() ) {
		case cubr::MultifileRunner::ERROR_FILE_NONEXISTENT:
			std::printf("\nERROR: File non-existent.\n");
			break;

		case cubr::MultifileRunner::ERROR_ENGINE_ERROR:
			std::printf("\nERROR: Engine error.\n");
			break;

		case cubr::MultifileRunner::ERROR_IMPORT_FAILED:
			std::printf("\nERROR: Import-file failed.\n");
			break;

		case cubr::MultifileRunner::ERROR_REQUIRE_FAILED:
			std::printf("\nERROR: Require-file failed.\n");
			break;

		default:
			std::printf("\nERROR: Unknown error flag.\n");
			break;
		}
		return 1;
	}

	std::printf("Now running the window.\n");

	while ( device->run() ) {
		device->getVideoDriver()->beginScene();
		device->getGUIEnvironment()->drawAll();
		device->getVideoDriver()->endScene();
	}

	return 0;
}
