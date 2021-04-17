// (C) 2021 Nicolaus Anderson
#include "cubr_json.h"
#include <CuAccessHelper.h>
#include <IWriteFile.h>

namespace cubr {

bool isJSONStorageObject( Cu::Object&  object ) {
	return object.getType() == getCubrTypeAsCuType(CubrObjectType::JSONStorage);
}

namespace json {

Hub::Hub( irr::io::IFileSystem*  fs )
	: fileSystem(fs)
{}

void
Hub::addToEngine( Cu::Engine& engine ) {
	addForeignMethodInstance<Hub>(engine, "json", this, &Hub::Create);
	addForeignFuncInstance(engine, "json_load", &OpenAndParse);
	addForeignFuncInstance(engine, "json_filepath", &SetFilePath);
	addForeignFuncInstance(engine, "json_new_accessor", &CreateAccessor);
	addForeignFuncInstance(engine, "json_accessor_count", &GetAccessorCount);
	addForeignFuncInstance(engine, "json_element_name", &GetElementName);
	addForeignFuncInstance(engine, "json_element_value", &GetElementValue);
	addForeignFuncInstance(engine, "json_accessor_to_sibling", &AccessorToSibling);
	addForeignFuncInstance(engine, "json_accessor_to_child", &AccessorToChild);
	addForeignFuncInstance(engine, "json_accessor_to_parent", &AccessorToParent);
	addForeignFuncInstance(engine, "json_to_copper", &ConvertJSONToCopper);
	addForeignFuncInstance(engine, "json_from_copper", &ConvertCopperToJSON);
	addForeignFuncInstance(engine, "json_save", &WriteJSON);
}

Cu::ForeignFunc::Result
Hub::Create( Cu::FFIServices& ffi ) {
	ffi.setNewResult( new Storage( fileSystem ) );
}


//--------------------------------------

Storage::Storage( irr::io::IFileSystem* file_system )
	: filePath()
	, fileSystem(file_system)
	, rootNode(0)
	, json(fileSystem)
	, accessors()
	, IDs(0)
{}

// ** Cu::Object virtual methods **

Cu::Object*
Storage::copy() {
	// TODO
	// Should copy the entire tree.
	// Currently treated as a monad, but that will change in the future.
	ref();
	return this;
}

void
Storage::writeToString(String& out) const {
	out = "{CuBridge JSON Storage}";
}

const char*
Storage::typeName() const {
	return JSONStorage::StaticTypeName();
}

bool
Storage::supportsInterface( Cu::ObjectType::Value ) const {
	return value == JSONStorage::getTypeAsCuType();
}

// ** JSON-related methods **

bool
Storage::parseFile( const irr::io::path& p ) {
	// All accessors must be deleted because they will be invalid
	accessors.clear();
	filePath = p;
	return json.parseFile(filePath, rootNode);
}

void
Storage::setFilePath( const irr::io::path& p ) {
	filePath = p;
}

bool
Storage::writeToFile() {
	if ( filePath.size() == 0 ) return false;
	if ( ! fileSystem->existFile(filePath) ) return false;

	irr::io::IWriteFile* outFile = fileSystem->createAndWriteFile(filePath, false);
	if ( !outFile ) return false;

	// TODO

	return true;
}

bool
Storage::createAccessor() {
	if ( rootNode ) {
		accessors.push_back(Accessor(rootNode,++IDs));
		return true;
	}
	return false;
}

bool
Storage::deleteAccessor( Cu::Integer id ) {
	Cu::UInteger i=0;
	for (; i < accessors.size(); ++i) {
		if ( accessors[i].id == id ) {
			accessors.erase(i);
			return true;
		}
	}
	return false;
}

Cu::UInteger
Storage::getLastAccessorId() {
	return IDs;
}

Cu::UInteger
Storage::getAccessorCount() {
	return accessors.size() - 1;
}

bool
Storage::getElementName( Cu::UInteger accessorId, util::String& outName ) {
	Accessor* accessor = REAL_NULL;
	irrJSONElement* element;
	if ( getAccessor(accessorId, accessor) ) {
		element = (irrJSONElement*)accessor->node->getElem();
		outName = element->getName().c_str();
		return true;
	}
	return false;
}

bool
Storage::getElementValue( Cu::UInteger accessorId, Cu::Object*& storage ) {
	Accessor* accessor = REAL_NULL;
	if ( !getAccessor(accessorId, accessor) ) {
		return false;
	}
	irrJSONElement* element = (irrJSONElement*) accessor->node->getElem();
	Cu::Function func = new Cu::Function();
	storage = new Cu::FunctionObject(func, -1);
	func->result.setWithoutRef( new Cu::StringObject( element->getValue().c_str() ) );
	func->deref();
	// Put the attributes into the Copper object as members
	Cu::AccessHelper accessHelper( (Cu::FunctionObject*)storage );
	irr::u32 a=0;
	for (; a < element->getAttributes().size(); ++a) {
		// Notice that we don't use element->getName() because that belongs to the parent.
		const irrJSONElement::Attribute& attr = element->getAttributes()[a];
		accessHelper.setMemberData(
			util::String( attr.name.c_str() ),
			new Cu::StringObject( attr.value.c_str() ),
			true
		);
	}
	return true;
}

bool
Storage::moveAccessorToSibling( Cu::Integer accessorId, bool moveDown ) {
	Accessor* accessor = REAL_NULL;
	if ( !getAccessor(accessorId, accessor) ) {
		return false;
	}
	if ( accessor->node == rootNode )
		return false;

	// TODO
	// Goto to parent, then get a list of the parent's children,
	// then find the index of this node, and pick the node after this one.
	//accessor->node->getParent()
}

bool
Storage::moveAccessorToChild( Cu::UInteger accessorId, Cu::UInteger childIndex ) {
	Accessor* accessor = REAL_NULL;
	if ( !getAccessor(accessorId, accessor) ) {
		return false;
	}
	// TODO
}

bool
Storage::moveAccessorToParent( Cu::UInteger accessorId ) {
	Accessor* accessor = REAL_NULL;
	if ( !getAccessor(accessorId, accessor) ) {
		return false;
	}
	// TODO
}

bool
Storage::convertToCopper( Cu::Function& storage ) {
	if ( !rootNode ) return false;

	// TODO
}

bool
Storage::convertFromCopper( Cu::Function& source ) {
	// All accessors must be deleted because they will be invalid
	accessors.clear();
	filePath = "#Copper#";
	// TODO
}

bool
Storage::noRootOrBadAccessorIndex( Cu::Integer index ) {
	return !rootNode || index + 1 > accessors.size();
}

bool getAccessor( Cu::UInteger id, Accessor*& accessorPtr ) {
	Cu::UInteger i=0;
	for (; i < accessors.size(); ++i) {
		if ( accessors[i].id == id ) {
			accessorPtr = & accessors[i];
			return true;
		}
	}
	return false;
}

//-------------------------------

//! Load JSON from a file
Cu::ForeignFunc::Result
OpenAndParse( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::String) // TODO Should be an object wrapping irr::io::path
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	util::String filepathString = ((Cu::String&)ffi.arg(0).getString());
	((Storage&)ffi.arg(0)).parseFile( irr::io::path(filepathString.c_str()) );

	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
SetFilePath( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::String) // TODO Should be an object wrapping irr::io::path
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	util::String filepathString = ((Cu::String&)ffi.arg(0).getString());
	((Storage&)ffi.arg(0)).setFilePath( irr::io::path(filepathString.c_str()) );

	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
CreateAccessor( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Storage& jsonStorage = (Storage&)ffi.arg(0);
	if ( jsonStorage.createAccessor() ) {
		// Return the index of the accessor
		ffi.setNewResult(jsonStorage.getLastAccessorId());
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
DeleteAccessor( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::Numeric)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Cu::Integer accessorId = ((Cu::NumericObject&)ffi.arg(1)).getIntegerValue();
	((Storage&)ffi.arg(0)).deleteAccessor(accessorId);
	return Cu::ForeignFunc::FINISHED;	
}

Cu::ForeignFunc::Result
GetAccessorCount( Cu::FFIServices& ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	ffi.setNewResult( new Cu::IntegerObject(
		((Storage&)ffi.arg(0)).getAccessorCount()
	));
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
GetElementName( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(0, Cu::ObjectType::Numeric)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Cu::Integer accessorId = ((Cu::NumericObject&)ffi.arg(1)).getIntegerValue();
	util::String name = REAL_NULL;
	if (
		((Storage&)ffi.arg(0)).getElementName(accessorId, name)
	) {
		ffi.setNewResult(name);
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
GetElementValue( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(0, Cu::ObjectType::Numeric)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Cu::Integer accessorId = ((NumericObject&)ffi.arg(1)).getIntegerValue();
	Cu::Object* value = REAL_NULL;
	if (
		((Storage&)ffi.arg(0)).getElementValue(accessorId, value)
	) {
		ffi.setNewResult(value);
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
AccessorToSibling( Cu::FFIServices& ffi ) {
	// TODO
}

Cu::ForeignFunc::Result
AccessorToChild( Cu::FFIServices& ffi ) {
	// TODO
}

Cu::ForeignFunc::Result
AccessorToParent( Cu::FFIServices& ffi ) {
	// TODO
}

Cu::ForeignFunc::Result
ConvertJSONToCopper( Cu::FFIServices& ffi ) {
	// TODO
}

Cu::ForeignFunc::Result
ConvertCopperToJSON( Cu::FFIServices& ffi ) {
	// TODO
}

Cu::ForeignFunc::Result
WriteJSON( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::String) // TODO Should be an object wrapping irr::io::path
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	util::String filepathString = ((Cu::String&)ffi.arg(0).getString());
	((Storage&)ffi.arg(0)).writeToFile( irr::io::path(filepathString.c_str()) );

	return Cu::ForeignFunc::FINISHED;
}



} // end namespace json
} // end namespace cubr
