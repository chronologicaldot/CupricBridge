// (C) 2018 Nicolaus Anderson

#include "../../src/cubridge.h"
#include <FileInStream.h>
#include <EngMsgToStr.h>
#include <irrlicht.h>

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

class EventRec : public irr::IEventReceiver {
	irr::IrrlichtDevice*  device;

public:
	EventRec( irr::IrrlichtDevice*  dev )
		: device(dev)
	{}

	virtual bool OnEvent( const irr::SEvent&  event ) {
		if ( event.EventType == irr::EET_GUI_EVENT )
			return OnGUIEvent(event.GUIEvent);

		return false;
	}

	bool OnGUIEvent( const irr::SEvent::SGUIEvent&  event ) {
		switch( event.EventType ) {
		case irr::gui::EGET_BUTTON_CLICKED:
			if ( event.Caller->getID() == ElementIDs::EXIT_BUTTON ) {
				device->closeDevice();
				return true;
			}

			break;

		default: break;
		}
		return false;
	}
};

int main() {

	irr::core::dimension2du  screenSize(900,600);
	irr::IrrlichtDevice*  device = irr::createDevice(irr::video::EDT_OPENGL, screenSize);

	if ( !device ) {
		return 1;
	}

	EventRec evrec(device);
	device->setEventReceiver(&evrec);

	// Another option:
	//Cu::StringInStream  sis( fileToCuStr("test.cu", device->getFileSystem()) );
	Cu::FileInStream  cfistream("test.cu");
	Lg logger(cfistream);
	Cu::Engine  cuengine;
	cuengine.setLogger(&logger);

	std::printf("post construct cfistream\n");

	cubr::CuBridge  cubridge = cubr::CuBridge(cuengine, device->getGUIEnvironment(), nullptr);

	std::printf("post construct cubridge\n");

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
