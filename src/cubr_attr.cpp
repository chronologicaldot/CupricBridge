// Copyright 2018 Nicolaus Anderson

#include "cubr_attr.h"
#include "cubr_base.h"
#include "cubr_str.h"

#include <cstdio>

namespace cubr {

bool getBoolValue( Cu::Object* obj, bool defaultValue ) {
	if ( obj ) {
		if ( Cu::isBoolObject(*obj) )
			return ((Cu::BoolObject*)obj)->getValue();
	}
	return defaultValue;
}

u32 getU32Value( Cu::Object* obj, u32 defaultValue ) {
	if ( obj ) {
		if ( Cu::isNumericObject(*obj) )
			return (u32) ((Cu::NumericObject*)obj)->getIntegerValue();
	}
	return defaultValue;
}

s32 getS32Value( Cu::Object* obj, s32 defaultValue ) {
	if ( obj ) {
		if ( Cu::isNumericObject(*obj) )
			return (s32) ((Cu::NumericObject*)obj)->getIntegerValue();
	}
	return defaultValue;
}

f32 getF32Value( Cu::Object* obj, f32 defaultValue ) {
	if ( obj ) {
		if ( Cu::isNumericObject(*obj) )
			return (f32) ((Cu::NumericObject*)obj)->getDecimalValue();
	}
	return defaultValue;
}

AttributeSource::AttributeSource( video_driver_t*  vidDriver, Cu::FunctionObject& src )
	: videoDriver(vidDriver)
	, infoSource(src)
	, infoNamesList()
{
	Cu::Function*  f;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().appendNamesByInterface(this);
	}
}

Cu::FunctionObject*
AttributeSource::getMemberByName(const c8* attributeName) const {
	return getSubMemberByName(infoSource, attributeName);
}

Cu::FunctionObject*
AttributeSource::getSubMemberByName(Cu::FunctionObject& container, const c8* attributeName) const {
	Cu::Function*  f;
	const Cu::String  name(attributeName);
	Cu::Variable* v;

	if ( container.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		return v->getRawContainer();
	}
	return 0; // Irrlicht uses 0 instead of nullptr
}

Cu::Object*
AttributeSource::getMemberFunctionResult(const c8* attributeName) const {
	return getSubMemberFunctionResult(infoSource, attributeName);
}

Cu::Object*
AttributeSource::getSubMemberFunctionResult(Cu::FunctionObject& container, const c8* attributeName) const {
	Cu::FunctionObject*  member = getSubMemberByName(container, attributeName);
	Cu::Function*  func;
	Cu::Object*  object;

	if ( member ) {
		if ( member->getFunction(func) ) {
			if ( func->result.obtain(object) ) {
				return object;
			}
		}
	}
	return 0; // Irrlicht uses 0 instead of nullptr
}

u32
AttributeSource::getAttributeAsUInt(Cu::FunctionObject& source, const c8* attributeName, u32 defaultNotFound) const {
	Cu::Object*  object = getSubMemberFunctionResult(source, attributeName);
	return getU32Value(object, defaultNotFound);
}

s32
AttributeSource::getAttributeAsInt(Cu::FunctionObject& source, const c8* attributeName, s32 defaultNotFound) const {
	Cu::Object*  object = getSubMemberFunctionResult(source, attributeName);
	return getS32Value(object, defaultNotFound);
}

f32
AttributeSource::getAttributeAsFloat(Cu::FunctionObject& source, const c8* attributeName, f32 defaultNotFound) const {
	Cu::Object*  object = getSubMemberFunctionResult(source, attributeName);
	return getF32Value(object, defaultNotFound);
}

core::vector2df
AttributeSource::getAttributeAsVector2d(Cu::FunctionObject& source, const c8* attributeName, core::vector2df defaultNotFound) const {
	// Get values from members labeled "x", "y", and "z".
	Cu::FunctionObject*  wrapperMember = getSubMemberByName(source, attributeName);
	Cu::Object*  object;
	core::vector2df  out(defaultNotFound);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "x");
		out.X = getF32Value(object, defaultNotFound.X);

		object = getSubMemberFunctionResult(*wrapperMember, "y");
		out.Y = getF32Value(object, defaultNotFound.Y);
	}

	return out;
}

core::vector3df
AttributeSource::getAttributeAsVector3d(Cu::FunctionObject& source, const c8* attributeName, core::vector3df defaultNotFound) const {
	// Get values from members labeled "x", "y", and "z".
	Cu::FunctionObject*  wrapperMember = getSubMemberByName(source, attributeName);
	Cu::Object*  object;
	core::vector3df  out(0);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "x");
		out.X = getF32Value(object, defaultNotFound.X);

		object = getSubMemberFunctionResult(*wrapperMember, "y");
		out.Y = getF32Value(object, defaultNotFound.Y);

		object = getSubMemberFunctionResult(*wrapperMember, "z");
		out.Z = getF32Value(object, defaultNotFound.Z);
	}

	return out;
}

void
AttributeSource::setSubMemberFunctionResult(Cu::FunctionObject& source, const c8* attributeName, u32 value) {
	Cu::FunctionObject*  member = getSubMemberByName(source, attributeName);
	Cu::Function*  f;

	if ( member ) {
		if ( member->getFunction(f) ) {
			f->result.setWithoutRef(new Cu::IntegerObject((Cu::Integer)value));
		}
	}
}

void
AttributeSource::setSubMemberFunctionResult(Cu::FunctionObject& source, const c8* attributeName, s32 value) {
	Cu::FunctionObject*  member = getSubMemberByName(source, attributeName);
	Cu::Function*  f;

	if ( member ) {
		if ( member->getFunction(f) ) {
			f->result.setWithoutRef(new Cu::IntegerObject((Cu::Integer)value));
		}
	}
}

void
AttributeSource::setSubMemberFunctionResult(Cu::FunctionObject& source, const c8* attributeName, f32 value) {
	Cu::FunctionObject*  member = getSubMemberByName(source, attributeName);
	Cu::Function*  f;

	if ( member ) {
		if ( member->getFunction(f) ) {
			f->result.setWithoutRef(new Cu::DecimalNumObject((Cu::Decimal)value));
		}
	}
}

void
AttributeSource::setSubMemberFunctionResult(Cu::FunctionObject& source, const c8* attributeName, core::vector2df value) {
	// Break up into members labeled "x", "y", and "z".
	Cu::FunctionObject*  member = getSubMemberByName(source, attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "x", value.X);
		setSubMemberFunctionResult(*member, "y", value.Y);
	}
}

void
AttributeSource::setSubMemberFunctionResult(Cu::FunctionObject& source, const c8* attributeName, core::vector3df value) {
	// Break up into members labeled "x", "y", and "z".
	Cu::FunctionObject*  member = getSubMemberByName(source, attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "x", value.X);
		setSubMemberFunctionResult(*member, "y", value.Y);
		setSubMemberFunctionResult(*member, "z", value.Z);
	}
}

void
AttributeSource::append( Cu::Object* object ) {
	const util::String  s = ((Cu::StringObject*)object)->getString();
	infoNamesList.push_back(s);
}

u32
AttributeSource::getAttributeCount() const {
	return (u32)(infoNamesList.size());
}

const c8*
AttributeSource::getAttributeName(s32 index) const {
	return infoNamesList[index].c_str();
}

io::E_ATTRIBUTE_TYPE
AttributeSource::getAttributeType(const c8* attributeName) const {
	// Note: Some types could probably be inferred based on the member variables in
	// the variable representing the attribute, but this is unreliable.
	return io::EAT_UNKNOWN;
}

io::E_ATTRIBUTE_TYPE
AttributeSource::getAttributeType(s32 index) const {
	// Note: Some types could probably be inferred based on the member variables in
	// the variable representing the attribute, but this is unreliable.
	return io::EAT_UNKNOWN;
}

const wchar_t*
AttributeSource::getAttributeTypeString(const c8* attributeName, const wchar_t* defaultNotFound) const {
	return  defaultNotFound;
}

const wchar_t*
AttributeSource::getAttributeTypeString(s32 index, const wchar_t* defaultNotFound) const {
	return  defaultNotFound;
}

bool
AttributeSource::existsAttribute(const c8* attributeName) const {
	return findAttribute(attributeName) != -1;
}

s32 AttributeSource::findAttribute(const c8* attributeName) const {
	slist_t::ConstIter  i = infoNamesList.constStart();
	s32  idx = 0;
	
	if ( i.has() )
	do {
		if ( i.getItem().equals(attributeName) ) {
			return idx;
		}
		++idx;
	} while ( i.next() );

	return -1;
}

void AttributeSource::clear() {
	// No need for this.
	// The infoSource cannot be deleted, otherwise other things may mess up.
}

void
AttributeSource::addInt(const c8* attributeName, s32 value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, s32 value) {
	Cu::Function*  f;
	const util::String  name(attributeName);
	Cu::Variable*  v;
	Cu::IntegerObject*  r;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		r = new Cu::IntegerObject(value);
		v->setFuncReturn(r, false);
		r->deref();
	}
}

s32
AttributeSource::getAttributeAsInt(const c8* attributeName, irr::s32 defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	return getS32Value(object, defaultNotFound);
}

s32
AttributeSource::getAttributeAsInt(s32 index) const {
	return getAttributeAsInt(infoNamesList[index].c_str(), 0);
}

void
AttributeSource::setAttribute(s32 index, s32 value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addFloat(const c8* attributeName, f32 value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, f32 value) {
	Cu::Function*  f;
	const util::String  name(attributeName);
	Cu::Variable*  v;
	Cu::DecimalNumObject*  r;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		r = new Cu::DecimalNumObject(value);
		v->setFuncReturn(r, false);
		r->deref();
	}
}

f32
AttributeSource::getAttributeAsFloat(const c8* attributeName, irr::f32 defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	return getF32Value(object, defaultNotFound);
}

f32
AttributeSource::getAttributeAsFloat(s32 index) const {
	return getAttributeAsFloat(infoNamesList[index].c_str(), 0.f);
}

void
AttributeSource::setAttribute(s32 index, f32 value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addString(const c8* attributeName, const c8* value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const c8* value) {
	Cu::Function*  f;
	const util::String  name(attributeName);
	Cu::Variable*  v;
	Cu::StringObject*  r;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		r = new Cu::StringObject(value);
		v->setFuncReturn(r, false);
		r->deref();
	}
}

core::stringc
AttributeSource::getAttributeAsString(const c8* attributeName, const core::stringc& defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	util::String  cs;
	// Note that StringObject and BoolObject are the only native Copper objects that convert their true values to
	// string via writeToString. The others merely describe themselves in brackets.
	if (object)
		object->writeToString(cs);

	return core::stringc(cs.c_str());
}

void
AttributeSource::getAttributeAsString(const c8* attributeName, c8* target) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	util::String  cs;
	// Note that StringObject and BoolObject are the only native Copper objects that convert their true values to
	// string via writeToString. The others merely describe themselves in brackets.
	if (object)
		object->writeToString(cs);
	strcpy(target, cs.c_str());
}

core::stringc
AttributeSource::getAttributeAsString(s32 index) const {
	return getAttributeAsString(infoNamesList[index].c_str());
}

void
AttributeSource::setAttribute(s32 index, const c8* value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addString(const c8* attributeName, const wchar_t* value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const wchar_t* value) {
	Cu::Function*  f;
	const util::String  name(attributeName);
	Cu::Variable*  v;
	Cu::StringObject*  r;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		r = new Cu::StringObject( wcharToCuStr(value, wcstrlen(value)) );
		v->setFuncReturn(r, false);
		r->deref();
	}
}

core::stringw
AttributeSource::getAttributeAsStringW(const c8* attributeName, const core::stringw& defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	util::String  cs = "";
	// Note that StringObject and BoolObject are the only native Copper objects that convert their true values to
	// string via writeToString. The others merely describe themselves in brackets.
	if (object) {
		object->writeToString(cs);
		if ( cs.size() > 0 ) {
			// Irrlicht utf8 function PHYSFS_utf8ToUcs4 segfaults when len == 0
			return CuStrToIrrStrW(cs);
		}
	}
	return core::stringw();
}

void
AttributeSource::getAttributeAsStringW(const c8* attributeName, wchar_t* target) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	util::String  cs = "";
	// Note that StringObject and BoolObject are the only native Copper objects that convert their true values to
	// string via writeToString. The others merely describe themselves in brackets.
	if ( !object)
		return;

	object->writeToString(cs);
	if ( cs.size() == 0 ) // Irrlicht utf8 function PHYSFS_utf8ToUcs4 segfaults when len == 0
		return;

	core::stringw  w = CuStrToIrrStrW(cs);
	strcpy((c8*)target, (c8*)w.c_str()); // We cheat here.

	// It would be nice if we could pass of the pointer (rather than copy) like so:
	//target = w.c_str();
	//w.pointer() = 0;
}

core::stringw
AttributeSource::getAttributeAsStringW(s32 index) const {
	return getAttributeAsStringW(infoNamesList[index].c_str());
}

void
AttributeSource::setAttribute(s32 index, const wchar_t* value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addBinary(const c8* attributeName, void* data, s32 dataSizeInBytes) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, data, dataSizeInBytes);
}

void
AttributeSource::setAttribute(const c8* attributeName, void* data, s32 dataSizeInBytes ) {
	// Unhandled. TODO Maybe cached in a string.
}

void
AttributeSource::getAttributeAsBinaryData(const c8* attributeName, void* outData, s32 maxSizeInBytes) const {
	// Unhandled. TODO Maybe cached in a string.
}

void
AttributeSource::getAttributeAsBinaryData(s32 index, void* outData, s32 maxSizeInBytes) const {
	return getAttributeAsBinaryData(infoNamesList[index].c_str(), outData, maxSizeInBytes);
}

void
AttributeSource::setAttribute(s32 index, void* data, s32 dataSizeInBytes ) {
	setAttribute(infoNamesList[index].c_str(), data, dataSizeInBytes);
}

void
AttributeSource::addArray(const c8* attributeName, const core::array<core::stringw>& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::array<core::stringw>& value) {
	// TODO Break up into substrings of UTF-8 (It would be faster as UTF-32, but this isn't native to Copper)
}

core::array<core::stringw>
AttributeSource::getAttributeAsArray(const c8* attributeName, const core::array<core::stringw>& defaultNotFound) const {
	// TODO Store as substrings of UTF-8 (It would be faster as UTF-32, but this isn't native to Copper)
	// FIXME
	return defaultNotFound;
}

core::array<core::stringw>
AttributeSource::getAttributeAsArray(s32 index) const {
	return getAttributeAsArray(infoNamesList[index].c_str());
}

void
AttributeSource::setAttribute(s32 index, const core::array<core::stringw>& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addBool(const c8* attributeName, bool value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, bool value) {
	Cu::Function*  f;
	const util::String  name(attributeName);
	Cu::Variable*  v;
	Cu::BoolObject*  r;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		r = new Cu::BoolObject(value);
		v->setFuncReturn(r, false);
		r->deref();
	}
}

bool
AttributeSource::getAttributeAsBool(const c8* attributeName, bool defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	return getBoolValue(object, defaultNotFound);
}

bool
AttributeSource::getAttributeAsBool(s32 index) const {
	return getAttributeAsBool(infoNamesList[index].c_str());
}

void
AttributeSource::setAttribute(s32 index, bool value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addEnum(const c8* attributeName, const c8* enumValue, const c8* const* enumerationLiterals) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, enumValue, enumerationLiterals);
}

void
AttributeSource::addEnum(const c8* attributeName, s32 enumValue, const c8* const* enumerationLiterals) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, enumerationLiterals[enumValue], enumerationLiterals);
}

void
AttributeSource::setAttribute(const c8* attributeName, const c8* enumValue, const c8* const* enumerationLiterals) {
	Cu::Function*  f;
	const util::String  name(attributeName);
	Cu::Variable*  v;
	Cu::StringObject*  r;

	if ( infoSource.getFunction(f) ) {
		f->getPersistentScope().getVariable(name, v);
		r = new Cu::StringObject(enumValue);
		v->setFuncReturn(r, false);
		r->deref();
	}
}

const c8*
AttributeSource::getAttributeAsEnumeration(const c8* attributeName, const c8* defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);

	if ( object ) {
		if ( Cu::isStringObject(*object) ) {
			return ((Cu::StringObject*)object)->getString().c_str();
		}
	}
	return defaultNotFound;
}

s32
AttributeSource::getAttributeAsEnumeration(const c8* attributeName, const c8* const* enumerationLiteralsToUse, s32 defaultNotFound) const {
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	util::String  name;
	u32 i = 0;
	const c8* const*  el = enumerationLiteralsToUse;

	if ( object ) {
		if ( Cu::isStringObject(*object) ) {
			name = ((Cu::StringObject*)object)->getString();
			for (; *el != 0; ++el, ++i) {
				if ( name.equals(*el) )
					return i;
			}
		}
		return getS32Value(object, -1);
	}
	return defaultNotFound;
}

s32
AttributeSource::getAttributeAsEnumeration(s32 index, const c8* const* enumerationLiteralsToUse, s32 defaultNotFound) const {
	return getAttributeAsEnumeration(infoNamesList[index].c_str(), enumerationLiteralsToUse, defaultNotFound);
}

const c8*
AttributeSource::getAttributeAsEnumeration(s32 index) const {
	return getAttributeAsEnumeration(infoNamesList[index].c_str());
}

void
AttributeSource::getAttributeEnumerationLiteralsOfEnumeration(const c8* attributeName, core::array<core::stringc>& outLiterals) const {
	// Cannot perform. This information isn't stored in the variable yet.
}

void
AttributeSource::getAttributeEnumerationLiteralsOfEnumeration(s32 index, core::array<core::stringc>& outLiterals) const {
	return getAttributeEnumerationLiteralsOfEnumeration(infoNamesList[index].c_str(), outLiterals);
}

void
AttributeSource::setAttribute(s32 index, const c8* enumValue, const c8* const* enumerationLiterals) {
	setAttribute(infoNamesList[index].c_str(), enumValue, enumerationLiterals);
}

void
AttributeSource::addColor(const c8* attributeName, video::SColor value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void AttributeSource::setAttribute(const c8* attributeName, video::SColor value) {
	Cu::FunctionObject*  member = getMemberByName(attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "alpha", value.getAlpha());
		setSubMemberFunctionResult(*member, "red", value.getRed());
		setSubMemberFunctionResult(*member, "green", value.getGreen());
		setSubMemberFunctionResult(*member, "blue", value.getBlue());
	}
}

video::SColor
AttributeSource::getAttributeAsColor(const c8* attributeName, const video::SColor& defaultNotFound) const {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	video::SColor  out = defaultNotFound;

	if ( member ) {
		out.setAlpha( getAttributeAsUInt(*member, "alpha", defaultNotFound.getAlpha()) );
		out.setRed( getAttributeAsUInt(*member, "red", defaultNotFound.getRed()) );
		out.setGreen( getAttributeAsUInt(*member, "green", defaultNotFound.getGreen()) );
		out.setBlue( getAttributeAsUInt(*member, "blue", defaultNotFound.getBlue()) );
	}
	return out;
}

video::SColor
AttributeSource::getAttributeAsColor(s32 index) const {
	return getAttributeAsColor(infoNamesList[index].c_str(), video::SColor(0));
}

void
AttributeSource::setAttribute(s32 index, video::SColor value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addColorf(const c8* attributeName, video::SColorf value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, video::SColorf value) {
	Cu::FunctionObject*  member = getMemberByName(attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "alpha", value.getAlpha());
		setSubMemberFunctionResult(*member, "red", value.getRed());
		setSubMemberFunctionResult(*member, "green", value.getGreen());
		setSubMemberFunctionResult(*member, "blue", value.getBlue());
	}
}

video::SColorf
AttributeSource::getAttributeAsColorf(const c8* attributeName, const video::SColorf& defaultNotFound) const {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	video::SColorf  out = defaultNotFound;

	if ( member ) {
		out.a = getAttributeAsFloat(*member, "alpha", defaultNotFound.a);
		out.r = getAttributeAsFloat(*member, "red", defaultNotFound.r);
		out.g = getAttributeAsFloat(*member, "green", defaultNotFound.g);
		out.b = getAttributeAsFloat(*member, "blue", defaultNotFound.b);
	}
	return out;
}

video::SColorf
AttributeSource::getAttributeAsColorf(s32 index) const {
	return getAttributeAsColorf(infoNamesList[index].c_str(), video::SColorf(0,0,0,0));
}

void
AttributeSource::setAttribute(s32 index, video::SColorf value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addVector3d(const c8* attributeName, const core::vector3df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::vector3df& value) {
	setSubMemberFunctionResult(infoSource, attributeName, value);
}

core::vector3df
AttributeSource::getAttributeAsVector3d(const c8* attributeName, const core::vector3df& defaultNotFound) const {
	return getAttributeAsVector3d(infoSource, attributeName, defaultNotFound);
}

core::vector3df
AttributeSource::getAttributeAsVector3d(s32 index) const {
	return getAttributeAsVector3d(infoNamesList[index].c_str(), core::vector3df(0,0,0));
}

void
AttributeSource::setAttribute(s32 index, const core::vector3df& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addVector2d(const c8* attributeName, const core::vector2df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::vector2df& value) {
	// Break up into members labeled "x" and "y".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);

	if ( wrapperMember ) {
		setSubMemberFunctionResult(*wrapperMember, attributeName, value);
	}
}

core::vector2df
AttributeSource::getAttributeAsVector2d(const c8* attributeName, const core::vector2df& defaultNotFound) const {
	return getAttributeAsVector2d(infoSource, attributeName, defaultNotFound);
}

core::vector2df
AttributeSource::getAttributeAsVector2d(s32 index) const {
	return getAttributeAsVector2d(infoNamesList[index].c_str(), core::vector2df(0));
}

void
AttributeSource::setAttribute(s32 index, const core::vector2df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addPosition2d(const c8* attributeName, const core::position2di& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::position2di& value) {
	// Break up into members labeled "x" and "y".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);

	if ( wrapperMember ) {
		setSubMemberFunctionResult(*wrapperMember, "x", value.X);
		setSubMemberFunctionResult(*wrapperMember, "y", value.Y);
	}
}

core::position2di
AttributeSource::getAttributeAsPosition2d(const c8* attributeName, const core::position2di& defaultNotFound) const {
	// Get values from members labeled "x" and "y".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::Object*  object;
	core::position2di  out(defaultNotFound);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "x");
		out.X = getS32Value(object, defaultNotFound.X);

		object = getSubMemberFunctionResult(*wrapperMember, "y");
		out.Y = getS32Value(object, defaultNotFound.Y);
	}
	return out;
}

core::position2di
AttributeSource::getAttributeAsPosition2d(s32 index) const {
	return getAttributeAsPosition2d(infoNamesList[index].c_str(), core::position2di(0));
}

void
AttributeSource::setAttribute(s32 index, const core::position2di& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addRect(const c8* attributeName, const core::rect<s32>& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::rect<s32>& value) {
	// Break up into members labeled "x", "y", "x2", and "y2".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);

	if ( wrapperMember ) {
		setSubMemberFunctionResult(*wrapperMember, "x", value.UpperLeftCorner.X);
		setSubMemberFunctionResult(*wrapperMember, "y", value.UpperLeftCorner.Y);
		setSubMemberFunctionResult(*wrapperMember, "x2", value.LowerRightCorner.X);
		setSubMemberFunctionResult(*wrapperMember, "y2", value.LowerRightCorner.Y);
	}
}

core::rect<s32>
AttributeSource::getAttributeAsRect(const c8* attributeName, const core::rect<s32>& defaultNotFound) const {
	// Get values from members labeled "x", "y", "x2", and "y2".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::Object*  object;
	core::rect<s32>  out(defaultNotFound);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "x");
		out.UpperLeftCorner.X = getS32Value(object, defaultNotFound.UpperLeftCorner.X);

		object = getSubMemberFunctionResult(*wrapperMember, "y");
		out.UpperLeftCorner.Y = getS32Value(object, defaultNotFound.UpperLeftCorner.Y);

		object = getSubMemberFunctionResult(*wrapperMember, "x2");
		out.LowerRightCorner.X = getS32Value(object, defaultNotFound.LowerRightCorner.X);

		object = getSubMemberFunctionResult(*wrapperMember, "y2");
		out.LowerRightCorner.Y = getS32Value(object, defaultNotFound.LowerRightCorner.Y);
	}
	return out;
}

core::rect<s32>
AttributeSource::getAttributeAsRect(s32 index) const {
	return getAttributeAsRect(infoNamesList[index].c_str(), core::rect<s32>(0,0,0,0));
}

void
AttributeSource::setAttribute(s32 index, const core::rect<s32>& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addDimension2d(const c8* attributeName, const core::dimension2d<u32>& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::dimension2d<u32>& value) {
	// Break up into members labeled "width" and "height".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);

	if ( wrapperMember ) {
		setSubMemberFunctionResult(*wrapperMember, "width", (s32)value.Width); // Should use a UInteger version
		setSubMemberFunctionResult(*wrapperMember, "height", (s32)value.Height); // Should use a UInteger version
	}
}

core::dimension2d<u32>
AttributeSource::getAttributeAsDimension2d(const c8* attributeName, const core::dimension2d<u32>& defaultNotFound) const {
	// Get values from members labeled "width" and "height".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::Object*  object;
	core::dimension2d<u32>  out(defaultNotFound);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "width");
		out.Width = getU32Value(object, defaultNotFound.Width);

		object = getSubMemberFunctionResult(*wrapperMember, "height");
		out.Height = getU32Value(object, defaultNotFound.Height);
	}
	return out;
}

core::dimension2d<u32>
AttributeSource::getAttributeAsDimension2d(s32 index) const {
	return getAttributeAsDimension2d(infoNamesList[index].c_str(), core::dimension2d<u32>(0,0));
}

void
AttributeSource::setAttribute(s32 index, const core::dimension2d<u32>& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addMatrix(const c8* attributeName, const core::matrix4& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::matrix4& value) {
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);

	if ( wrapperMember ) {
		setSubMemberFunctionResult(*wrapperMember, "v0", value[0]);
		setSubMemberFunctionResult(*wrapperMember, "v1", value[1]);
		setSubMemberFunctionResult(*wrapperMember, "v2", value[2]);
		setSubMemberFunctionResult(*wrapperMember, "v3", value[3]);

		setSubMemberFunctionResult(*wrapperMember, "v4", value[4]);
		setSubMemberFunctionResult(*wrapperMember, "v5", value[5]);
		setSubMemberFunctionResult(*wrapperMember, "v6", value[6]);
		setSubMemberFunctionResult(*wrapperMember, "v7", value[7]);

		setSubMemberFunctionResult(*wrapperMember, "v8", value[8]);
		setSubMemberFunctionResult(*wrapperMember, "v9", value[9]);
		setSubMemberFunctionResult(*wrapperMember, "v10", value[10]);
		setSubMemberFunctionResult(*wrapperMember, "v11", value[11]);

		setSubMemberFunctionResult(*wrapperMember, "v12", value[12]);
		setSubMemberFunctionResult(*wrapperMember, "v13", value[13]);
		setSubMemberFunctionResult(*wrapperMember, "v14", value[14]);
		setSubMemberFunctionResult(*wrapperMember, "v15", value[15]);
	}
}

core::matrix4
AttributeSource::getAttributeAsMatrix(const c8* attributeName, const core::matrix4& defaultNotFound) const {
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::Object*  object;
	core::matrix4  out(defaultNotFound);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "v0");
		out[0] = getF32Value(object, defaultNotFound[0]);

		object = getSubMemberFunctionResult(*wrapperMember, "v1");
		out[1] = getF32Value(object, defaultNotFound[1]);

		object = getSubMemberFunctionResult(*wrapperMember, "v2");
		out[2] = getF32Value(object, defaultNotFound[2]);

		object = getSubMemberFunctionResult(*wrapperMember, "v3");
		out[3] = getF32Value(object, defaultNotFound[3]);

		object = getSubMemberFunctionResult(*wrapperMember, "v4");
		out[4] = getF32Value(object, defaultNotFound[4]);

		object = getSubMemberFunctionResult(*wrapperMember, "v5");
		out[5] = getF32Value(object, defaultNotFound[5]);

		object = getSubMemberFunctionResult(*wrapperMember, "v6");
		out[6] = getF32Value(object, defaultNotFound[6]);

		object = getSubMemberFunctionResult(*wrapperMember, "v7");
		out[7] = getF32Value(object, defaultNotFound[7]);

		object = getSubMemberFunctionResult(*wrapperMember, "v8");
		out[8] = getF32Value(object, defaultNotFound[8]);

		object = getSubMemberFunctionResult(*wrapperMember, "v9");
		out[9] = getF32Value(object, defaultNotFound[9]);

		object = getSubMemberFunctionResult(*wrapperMember, "v10");
		out[10] = getF32Value(object, defaultNotFound[10]);

		object = getSubMemberFunctionResult(*wrapperMember, "v11");
		out[11] = getF32Value(object, defaultNotFound[11]);

		object = getSubMemberFunctionResult(*wrapperMember, "v12");
		out[12] = getF32Value(object, defaultNotFound[12]);

		object = getSubMemberFunctionResult(*wrapperMember, "v13");
		out[13] = getF32Value(object, defaultNotFound[13]);

		object = getSubMemberFunctionResult(*wrapperMember, "v14");
		out[14] = getF32Value(object, defaultNotFound[14]);

		object = getSubMemberFunctionResult(*wrapperMember, "v15");
		out[15] = getF32Value(object, defaultNotFound[15]);
	}
	return out;
}

core::matrix4
AttributeSource::getAttributeAsMatrix(s32 index) const {
	return getAttributeAsMatrix(infoNamesList[index].c_str(), core::matrix4());
}

void
AttributeSource::setAttribute(s32 index, const core::matrix4& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addQuaternion(const c8* attributeName, const core::quaternion& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::quaternion& value) {
	// Break up into members labeled "x", "y", "z", and "w".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);

	if ( wrapperMember ) {
		setSubMemberFunctionResult(*wrapperMember, "x", value.X);
		setSubMemberFunctionResult(*wrapperMember, "y", value.Y);
		setSubMemberFunctionResult(*wrapperMember, "z", value.Z);
		setSubMemberFunctionResult(*wrapperMember, "w", value.W);
	}
}

core::quaternion
AttributeSource::getAttributeAsQuaternion(const c8* attributeName, const core::quaternion& defaultNotFound) const {
	// Get values from members labeled "x", "y", "z", and "w".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::Object*  object;
	core::quaternion  out(defaultNotFound);

	if ( wrapperMember ) {
		object = getSubMemberFunctionResult(*wrapperMember, "x");
		out.X = getF32Value(object, defaultNotFound.X);

		object = getSubMemberFunctionResult(*wrapperMember, "y");
		out.Y = getF32Value(object, defaultNotFound.Y);

		object = getSubMemberFunctionResult(*wrapperMember, "z");
		out.Z = getF32Value(object, defaultNotFound.Z);

		object = getSubMemberFunctionResult(*wrapperMember, "w");
		out.W = getF32Value(object, defaultNotFound.W);
	}
	return out;
}

core::quaternion
AttributeSource::getAttributeAsQuaternion(s32 index) const {
	return getAttributeAsQuaternion(infoNamesList[index].c_str(), core::quaternion());
}

void
AttributeSource::setAttribute(s32 index, const core::quaternion& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addBox3d(const c8* attributeName, const core::aabbox3df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::aabbox3df& value) {
	// Break up into members that are sub-members of "min" and "max.
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::FunctionObject*  member;

	if ( wrapperMember ) {
		member = getSubMemberByName(*wrapperMember, "min");
		if ( member ) {
			setSubMemberFunctionResult(*member, "x", value.MinEdge.X);
			setSubMemberFunctionResult(*member, "y", value.MinEdge.Y);
			setSubMemberFunctionResult(*member, "z", value.MinEdge.Z);
		}
		member = getSubMemberByName(*wrapperMember, "max");
		if ( member ) {
			setSubMemberFunctionResult(*member, "x", value.MaxEdge.X);
			setSubMemberFunctionResult(*member, "y", value.MaxEdge.Y);
			setSubMemberFunctionResult(*member, "z", value.MaxEdge.Z);
		}
	}
}

core::aabbox3df
AttributeSource::getAttributeAsBox3d(const c8* attributeName, const core::aabbox3df& defaultNotFound) const {
	// Get values from members that are sub-members of "min" and "max".
	Cu::FunctionObject*  wrapperMember = getMemberByName(attributeName);
	Cu::FunctionObject*  member;
	Cu::Object*  object;
	core::aabbox3df  out(defaultNotFound);

	if ( wrapperMember ) {
		member = getSubMemberByName(*wrapperMember, "min");
		if ( member ) {
			object = getSubMemberFunctionResult(*member, "x");
			out.MinEdge.X = getF32Value(object, defaultNotFound.MinEdge.X);

			object = getSubMemberFunctionResult(*member, "y");
			out.MinEdge.Y = getF32Value(object, defaultNotFound.MinEdge.Y);

			object = getSubMemberFunctionResult(*member, "z");
			out.MinEdge.Z = getF32Value(object, defaultNotFound.MinEdge.Z);
		}
		member = getSubMemberByName(*wrapperMember, "max");
		if ( member ) {
			object = getSubMemberFunctionResult(*member, "x");
			out.MaxEdge.X = getF32Value(object, defaultNotFound.MaxEdge.X);

			object = getSubMemberFunctionResult(*member, "y");
			out.MaxEdge.Y = getF32Value(object, defaultNotFound.MaxEdge.Y);

			object = getSubMemberFunctionResult(*member, "z");
			out.MaxEdge.Z = getF32Value(object, defaultNotFound.MaxEdge.Z);
		}
	}
	return out;
}

core::aabbox3df
AttributeSource::getAttributeAsBox3d(s32 index) const {
	return getAttributeAsBox3d(infoNamesList[index].c_str(), core::aabbox3df());
}

void
AttributeSource::setAttribute(s32 index, const core::aabbox3df& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addPlane3d(const c8* attributeName, const core::plane3df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::plane3df& value) {
	// Break up into members labeled "x", "y", and "z".
	Cu::FunctionObject*  member = getMemberByName(attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "normal", value.Normal);
	}
}

core::plane3df
AttributeSource::getAttributeAsPlane3d(const c8* attributeName, const core::plane3df& defaultNotFound) const {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	core::plane3df  out(defaultNotFound);

	if ( member ) {
		out.Normal = getAttributeAsVector3d(*member, "normal");
		out.recalculateD(core::vector3df(0));
	}
	return out;
}

core::plane3df
AttributeSource::getAttributeAsPlane3d(s32 index) const {
	return getAttributeAsPlane3d(infoNamesList[index].c_str(), core::plane3df());
}

void
AttributeSource::setAttribute(s32 index, const core::plane3df& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addTriangle3d(const c8* attributeName, const core::triangle3df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::triangle3df& value) {
	Cu::FunctionObject*  member = getMemberByName(attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "pointA", value.pointA);
		setSubMemberFunctionResult(*member, "pointB", value.pointB);
		setSubMemberFunctionResult(*member, "pointC", value.pointC);
	}
}

core::triangle3df
AttributeSource::getAttributeAsTriangle3d(const c8* attributeName, const core::triangle3df& defaultNotFound) const {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	core::triangle3df  out;

	if ( member ) {
		out.pointA = getAttributeAsVector3d(*member, "pointA");
		out.pointB = getAttributeAsVector3d(*member, "pointB");
		out.pointC = getAttributeAsVector3d(*member, "pointC");
	}
	return out;
}

core::triangle3df
AttributeSource::getAttributeAsTriangle3d(s32 index) const {
	return getAttributeAsTriangle3d(infoNamesList[index].c_str(), core::triangle3df());
}

void
AttributeSource::setAttribute(s32 index, const core::triangle3df& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addLine2d(const c8* attributeName, const core::line2df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::line2df& value) {
	Cu::FunctionObject*  member = getMemberByName(attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "start", value.start);
		setSubMemberFunctionResult(*member, "end", value.end);
	}
}

core::line2df
AttributeSource::getAttributeAsLine2d(const c8* attributeName, const core::line2df& defaultNotFound) const {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	core::line2df  out = defaultNotFound;

	if ( member ) {
		out.start = getAttributeAsVector2d(*member, "start");
		out.end = getAttributeAsVector2d(*member, "end");
	}
	return out;
}

core::line2df
AttributeSource::getAttributeAsLine2d(s32 index) const {
	return getAttributeAsLine2d(infoNamesList[index].c_str(), core::line2df());
}

void
AttributeSource::setAttribute(s32 index, const core::line2df& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addLine3d(const c8* attributeName, const core::line3df& value) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, value);
}

void
AttributeSource::setAttribute(const c8* attributeName, const core::line3df& value) {
	Cu::FunctionObject*  member = getMemberByName(attributeName);

	if ( member ) {
		setSubMemberFunctionResult(*member, "start", value.start);
		setSubMemberFunctionResult(*member, "end", value.end);
	}
}

core::line3df
AttributeSource::getAttributeAsLine3d(const c8* attributeName, const core::line3df& defaultNotFound) const {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	core::line3df  out = defaultNotFound;

	if ( member ) {
		out.start = getAttributeAsVector3d(*member, "start");
		out.end = getAttributeAsVector3d(*member, "end");
	}
	return out;
}

core::line3df
AttributeSource::getAttributeAsLine3d(s32 index) const {
	return getAttributeAsLine3d(infoNamesList[index].c_str(), core::line3df());
}

void
AttributeSource::setAttribute(s32 index, const core::line3df& value) {
	setAttribute(infoNamesList[index].c_str(), value);
}

void
AttributeSource::addTexture(const c8* attributeName, video::ITexture* texture, const io::path& filename) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, texture, filename);
}

void
AttributeSource::setAttribute(const c8* attributeName, video::ITexture* texture, const io::path& filename) {
	Cu::FunctionObject*  member = getMemberByName(attributeName);
	Cu::Function*  function;
	// Copper Bridge Texture
	Texture*  textureObject;

	if ( !member ) return;

	if ( ! member->getFunction(function) ) {
		return;
	}

	if ( !texture ) {
		textureObject = new Texture(videoDriver->getTexture(filename));
	} else {
		textureObject = new Texture(texture);
	}
	function->result.set(textureObject);
	textureObject->deref();
}

video::ITexture*
AttributeSource::getAttributeAsTexture(const c8* attributeName, video::ITexture* defaultNotFound) const {
	// Either the variable's function return will be a texture or a filepath (either a wchar path or a byte string as UTF-8).
	Cu::Object*  object = getMemberFunctionResult(attributeName);
	if ( object ) {
		if ( isTextureObject(*object) ) {
			return ((Texture*)object)->getTexture();
		}
	}
	return defaultNotFound;
}

video::ITexture*
AttributeSource::getAttributeAsTexture(s32 index) const {
	return getAttributeAsTexture(infoNamesList[index].c_str(), 0);
}

void
AttributeSource::setAttribute(s32 index, video::ITexture* texture, const io::path& filename) {
	setAttribute(infoNamesList[index].c_str(), texture, filename);
}

void
AttributeSource::addUserPointer(const c8* attributeName, void* userPointer) {
	// Take advantage of auto-creation of object-function members in Copper
	setAttribute(attributeName, userPointer);
}

void
AttributeSource::setAttribute(const c8* attributeName, void* userPointer) {
	// Do nothing. We don't let users set special pointers.
}

void*
AttributeSource::getAttributeAsUserPointer(const c8* attributeName, void* defaultNotFound) const {
	return defaultNotFound;
}

void*
AttributeSource::getAttributeAsUserPointer(s32 index) const {
	return getAttributeAsUserPointer(infoNamesList[index].c_str(), 0);
}

void
AttributeSource::setAttribute(s32 index, void* userPointer) {
	setAttribute(infoNamesList[index].c_str(), userPointer);
}


}
