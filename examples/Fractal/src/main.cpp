// (C) 2018 Nicolaus Anderson

#include <Copper.h>
#include <FileInStream.h>
#include <EngMsgToStr.h>
#include <cu_basicmath.h>
#include <irrlicht.h>
#include "../../../src/cubridge.h"
#include "../../../src/cubr_event.h"
#include "app.h"

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
		Cu::EngineErrorLevel::Value  errLevel;
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

int main() {

	irr::core::dimension2du  screenSize(600,700);
	irr::IrrlichtDevice*  device = irr::createDevice(irr::video::EDT_OPENGL, screenSize);

	if ( !device ) {
		return 1;
	}

	irr::gui::IGUIEnvironment*  guiEnvironment = device->getGUIEnvironment();

	irr::gui::IGUISkin* skin = guiEnvironment->getSkin();
	irr::gui::IGUIFont* font = guiEnvironment->getFont( "font/sansfont.xml" );

	if ( font )
		skin->setFont( font );

	skin->setColor(irr::gui::EGDC_BUTTON_TEXT, irr::video::SColor(255,255,255,255));
	skin->setColor(irr::gui::EGDC_GRAY_TEXT, irr::video::SColor(255,66,66,66));
	skin->setColor(irr::gui::EGDC_TOOLTIP, irr::video::SColor(255,240,240,100));
	skin->setColor(irr::gui::EGDC_TOOLTIP_BACKGROUND, irr::video::SColor(255,50,60,150));

	irr::u32 c=0;
	irr::video::SColor color;
	irr::s32 defaultColorDrop = 0;
	irr::s32 colorDrop = defaultColorDrop;
	for(; c < irr::gui::EGDF_COUNT; ++c) {
		color = skin->getColor( (irr::gui::EGUI_DEFAULT_COLOR)c );
		color.setAlpha(255);
		switch( (irr::gui::EGUI_DEFAULT_COLOR)c ) {
			case irr::gui::EGDC_3D_FACE:
				colorDrop = 50;
				break;

			//case irr::gui::EGDC_3D_SHADOW:
			case irr::gui::EGDC_3D_DARK_SHADOW:
				colorDrop = -10;
				break;

			default:
				colorDrop = defaultColorDrop;
				break;
		}
		color.setRed( (irr::u32) irr::core::clamp((irr::s32)color.getRed() - colorDrop, 0, 255) );
		color.setGreen( (irr::u32) irr::core::clamp((irr::s32)color.getGreen() - colorDrop, 0, 255) );
		color.setBlue( (irr::u32) irr::core::clamp((irr::s32)color.getBlue() - colorDrop, 0, 255) );
		skin->setColor( (irr::gui::EGUI_DEFAULT_COLOR)c, color );
	}

	Cu::FileInStream  cfistream("main.cu");
	Lg logger(cfistream);
	Cu::Engine  cuengine;
	cuengine.setLogger(&logger);

	cubr::EventHandler ceh(cuengine);
	device->setEventReceiver(&ceh);

	cubr::CuBridge::InitFlags initflags;
	initflags.enableImageModifying = true;
	cubr::CuBridge  cubridge = cubr::CuBridge(cuengine, device->getGUIEnvironment(), nullptr, initflags);
	AppCuInterface  aci(cuengine, device);
	Cu::Numeric::addFunctionsToEngine(cuengine);

	Cu::EngineResult::Value  erv;
	do {
		erv = cuengine.run(cfistream);
	} while ( erv == Cu::EngineResult::Ok );
	if ( erv == Cu::EngineResult::Error ) {
		device->closeDevice();
		device->drop();
		return 1;
	}

	while ( device->run() ) {
		device->getVideoDriver()->beginScene();
		device->getGUIEnvironment()->drawAll();
		device->getVideoDriver()->endScene();
	}

	device->drop();
	return 0;
}
