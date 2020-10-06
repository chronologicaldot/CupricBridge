/* (C) 2020 Nicolaus Anderson */

#include "cubr_messagecodes.h"
#include "cubr_image.h"
#include <cstdio>

namespace cubr {

Cu::ForeignFunc::Result
GetImageDimensions( Cu::FFIServices&  ffi ) {
	if ( ! ffi.demandArgType(0, Image::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}

	image_t*  img = ((Image&)ffi.arg(0)).getImage();
	irr::core::dimension2du  imgSize = irr::core::dimension2du(0,0);
	if ( img ) {
		imgSize = img->getDimension();
	}
	// Create a new Copper function with members width and height
	Cu::FunctionObject*  funcObject = new Cu::FunctionObject();
	Cu::Function*  func = REAL_NULL;
	Cu::Variable*  var;
	Cu::IntegerObject*  intObject;
	// TODO: PROBLEM: The image width is an unsigned int. What if it's value is greater than int can hold?

	if ( funcObject->getFunction(func) ) {
		// width
		var = func->getPersistentScope().addVariable("width");
		intObject = new Cu::IntegerObject(imgSize.Width);
		var->setFuncReturn(intObject, false);
		intObject->deref();

		// height
		var = func->getPersistentScope().addVariable("height");
		intObject = new Cu::IntegerObject(imgSize.Height);
		var->setFuncReturn(intObject, false);
		intObject->deref();

		ffi.setNewResult(funcObject);
	}

	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
GetImagePixel( Cu::FFIServices&  ffi ) {
	if ( ! ffi.demandArgType(0, Image::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::Numeric)
		|| ! ffi.demandArgType(2, Cu::ObjectType::Numeric)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}

	image_t*  img = ((Image&)ffi.arg(0)).getImage();
	Cu::Integer  x = ((Cu::NumericObject&)ffi.arg(1)).getIntegerValue();
	Cu::Integer  y = ((Cu::NumericObject&)ffi.arg(2)).getIntegerValue();

	irr::video::SColor  color = img->getPixel( irr::u32(x), irr::u32(y) );

	// Create a new Copper function with members red, green, blue, and alpha
	Cu::FunctionObject*  funcObject = new Cu::FunctionObject();
	Cu::Function*  func = REAL_NULL;
	Cu::Variable*  var;
	Cu::IntegerObject*  intObject;

	if ( funcObject->getFunction(func) ) {
		// red
		var = func->getPersistentScope().addVariable("red");
		intObject = new Cu::IntegerObject(color.getRed());
		var->setFuncReturn(intObject, false);
		intObject->deref();

		// green
		var = func->getPersistentScope().addVariable("green");
		intObject = new Cu::IntegerObject(color.getGreen());
		var->setFuncReturn(intObject, false);
		intObject->deref();

		// blue
		var = func->getPersistentScope().addVariable("blue");
		intObject = new Cu::IntegerObject(color.getBlue());
		var->setFuncReturn(intObject, false);
		intObject->deref();

		// alpha
		var = func->getPersistentScope().addVariable("alpha");
		intObject = new Cu::IntegerObject(color.getAlpha());
		var->setFuncReturn(intObject, false);
		intObject->deref();

		ffi.setNewResult(funcObject);
	}

	return Cu::ForeignFunc::FINISHED;
}

// Accepts an image, an x value, a y value, and a object with members red,green,blue,alpha to set a pixel color.
Cu::ForeignFunc::Result
SetImagePixel( Cu::FFIServices&  ffi ) {
	if ( ! ffi.demandArgType(0, Image::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::Numeric)
		|| ! ffi.demandArgType(2, Cu::ObjectType::Numeric)
		|| ! ffi.demandArgType(3, Cu::ObjectType::Function)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}

	image_t*  img = ((Image&)ffi.arg(0)).getImage();
	Cu::Integer  x = ((Cu::NumericObject&)ffi.arg(1)).getIntegerValue();
	Cu::Integer  y = ((Cu::NumericObject&)ffi.arg(2)).getIntegerValue();
	Cu::FunctionObject&  funcObject = (Cu::FunctionObject&)ffi.arg(3);
	Cu::Function*  func = REAL_NULL;
	Cu::Function*  member = REAL_NULL;
	Cu::Variable*  var;
	Cu::Object*  result;
	irr::video::SColor  color = irr::video::SColor(255,255,255,255);

	if ( ! funcObject.getFunction(func) ) {
		ffi.printCustomWarningCode( CuBridgeMessageCode::ImageSetPixelMissingColor );
		return Cu::ForeignFunc::NONFATAL;
	}

	func->getPersistentScope().getVariable( "red", var );
	member = var->getFunction(REAL_NULL); // Unable to report empty functions
	if ( member->constantReturn ) {
		if ( member->result.obtain( result ) ) {
			if ( result->supportsInterface( Cu::ObjectType::Numeric ) )
				color.setRed( ((Cu::NumericObject*)result)->getIntegerValue() );
		}
	}

	func->getPersistentScope().getVariable( "green", var );
	member = var->getFunction(REAL_NULL); // Unable to report empty functions
	if ( member->constantReturn ) {
		if ( member->result.obtain( result ) ) {
			if ( result->supportsInterface( Cu::ObjectType::Numeric ) )
				color.setGreen( ((Cu::NumericObject*)result)->getIntegerValue() );
		}
	}

	func->getPersistentScope().getVariable( "blue", var );
	member = var->getFunction(REAL_NULL); // Unable to report empty functions
	if ( member->constantReturn ) {
		if ( member->result.obtain( result ) ) {
			if ( result->supportsInterface( Cu::ObjectType::Numeric ) )
				color.setBlue( ((Cu::NumericObject*)result)->getIntegerValue() );
		}
	}

	func->getPersistentScope().getVariable( "alpha", var );
	member = var->getFunction(REAL_NULL); // Unable to report empty functions
	if ( member->constantReturn ) {
		if ( member->result.obtain( result ) ) {
			if ( result->supportsInterface( Cu::ObjectType::Numeric ) )
				color.setAlpha( ((Cu::NumericObject*)result)->getIntegerValue() );
		}
	}

	img->setPixel( irr::u32(x), irr::u32(y), color );

	return Cu::ForeignFunc::FINISHED;
}

} // namespace cubr
