// (C) 2021 Nicolaus Anderson
#include "cubr_json.h"
#include "../cubr_attr.h"
#include <CuAccessHelper.h>
#include <irrList.h>
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
	Cu::addForeignMethodInstance<Hub>(engine, "json", this, &Hub::Create);
	Cu::addForeignFuncInstance(engine, "json_load", &OpenAndParse);
	Cu::addForeignFuncInstance(engine, "json_filepath", &SetFilePath);
	Cu::addForeignFuncInstance(engine, "json_root", &CreateAccessor);
	Cu::addForeignFuncInstance(engine, "json_child_count", &GetAccessorChildCount);
	Cu::addForeignFuncInstance(engine, "json_child", &AccessorChild);
	Cu::addForeignFuncInstance(engine, "json_add_child", &AddChild);
	Cu::addForeignFuncInstance(engine, "json_parent", &AccessorParent);
	Cu::addForeignFuncInstance(engine, "json_element_name", &GetSetElementName);
	Cu::addForeignFuncInstance(engine, "json_element_attrs", &GetSetElementAttrs);
	//Cu::addForeignFuncInstance(engine, "json_to_copper", &ConvertJSONToCopper);
	//Cu::addForeignFuncInstance(engine, "json_from_copper", &ConvertCopperToJSON);
	Cu::addForeignFuncInstance(engine, "json_save", &WriteJSON);
}

Cu::ForeignFunc::Result
Hub::Create( Cu::FFIServices& ffi ) {
	ffi.setNewResult( new Storage( fileSystem ) );
	return Cu::ForeignFunc::FINISHED;
}

//--------------------------------------

Accessor::Accessor( Storage* s, irrTreeNode* n, bool mustRegister )
	: storage(s)
	, node(n)
{
	if ( storage && mustRegister ) {
		storage->registerAccessor(this);
	}
}

Accessor::~Accessor()
{
	if ( storage ) {
		storage->deregisterAccessor(this);
	}
}

// ** Cu::Object virtual methods **

Cu::Object*
Accessor::copy() {
	return new Accessor(storage, node);
}

void
Accessor::writeToString(String& out) const {
	// TODO: Comment out
	out = "{CuBridge JSON Accessor}";
	// TODO: Write the node's value contents into the string
}

const char*
Accessor::typeName() const {
	return Accessor::StaticTypeName();
}

bool
Accessor::supportsInterface( Cu::ObjectType::Value value ) const {
	return value == Accessor::getTypeAsCuType();
}

//** Class-relationship methods **

// When the Storage class is about to destruct, this is called.
void
Accessor::invalidate() {
	storage = 0;
	node = 0;
}

// ** JSON-related methods **

Cu::UInteger
Accessor::getChildCount() {
	if ( valid() )
		return node->children.size();
	return 0;
}

Accessor*
Accessor::getChild( Cu::UInteger index ) {
	if ( valid() ) {
		if ( index < node->children.size() ) {
			return new Accessor(storage, node->children[index]);
		}
	}
	return REAL_NULL;
}

Accessor*
Accessor::addChild( const util::String& name ) {
	irrTreeNode* childNode;
	if ( valid() ) {
		childNode = & (node->addNode(-1));
		((irrJSONElement*)childNode->getElem())->getName() = name.c_str();
		return new Accessor(storage, childNode);
	}
	return REAL_NULL;
}

Accessor*
Accessor::getParent() {
	if ( valid() ) {
		return new Accessor(storage, node->parent); // Invalid for root, but this is checked with valid()
	}
	return REAL_NULL;
}

bool
Accessor::getElementName( util::String& outName ) {
	irrJSONElement* element;
	if ( valid() ) {
		element = (irrJSONElement*)node->getElem();
		outName = element->getName().c_str();
		return true;
	}
	return false;
}

bool
Accessor::setElementName( const util::String& name ) {
	irrJSONElement* element;
	if ( valid() ) {
		element = (irrJSONElement*)node->getElem();
		element->getName() = name.c_str();
		return true;
	}
	return false;
}

bool
Accessor::getElementAttributes( Cu::FunctionObject*& storage ) {
	if ( !valid() ) {
		return false;
	}
	irrJSONElement* element = (irrJSONElement*) node->getElem();
	storage = new Cu::FunctionObject();
	// Put the attributes into the Copper object as members
	Cu::AccessHelper accessHelper( storage );
	irr::core::list<irrJSONElement::Attribute>::Iterator attrItr = element->getAttributes().begin();
	for (; attrItr != element->getAttributes().end(); ++attrItr) {
		// Notice that we don't use element->getName() because that belongs to the parent.
		const irrJSONElement::Attribute& attr = *attrItr;
		accessHelper.setMemberData(
			util::String( attr.name.c_str() ),
			new Cu::StringObject( attr.value.c_str() ),
			true
		);
	}
	return true;
}

bool
Accessor::setElementAttributes( Cu::FunctionObject& source ) {
	if ( !valid() ) {
		return false;
	}
	irrJSONElement* element = (irrJSONElement*) node->getElem();
	AttributeSource attributeSource( 0, source );
	irr::u32 attrIdx = 0;
	for (; attributeSource.getAttributeCount(); ++attrIdx) {
		element->getAttributes().push_back(
			irrJSONElement::Attribute(
				attributeSource.getAttributeName(attrIdx),
				attributeSource.getAttributeAsString(attrIdx).c_str()
			)
		);
	}
	return true;
}


//--------------------------------------

Storage::Storage( irr::io::IFileSystem* file_system )
	: filePath()
	, fileSystem(file_system)
	, rootNode(0)
	, json(fileSystem)
	, accessors()
{}

Storage::~Storage() {
	irr::u32 i = 0;
	for (; i < accessors.size(); i += 1)
	{
		accessors[i]->invalidate();
	}
}

// ** Cu::Object virtual methods **

Cu::Object*
Storage::copy() {
	// TODO
	// Should copy the entire tree.
	// Currently treated as a monad, but that will change in the future.
	ref();
	return this;
}

// ** Cu::Object virtual methods **

void
Storage::writeToString(String& out) const {
	// TODO: Comment out
	out = "{CuBridge JSON Storage}";
	// TODO: Write the entire JSON tree to a string. Will be helpful for exporting.
	// Tabs and newlines could be enabled/disabled with a Storage-internal flag.
}

const char*
Storage::typeName() const {
	return Storage::StaticTypeName();
}

bool
Storage::supportsInterface( Cu::ObjectType::Value value ) const {
	return value == Storage::getTypeAsCuType();
}

// ** JSON-related methods **

bool
Storage::parseFile( const irr::io::path& p ) {
	// All accessors must be deleted because they will be invalid
	irr::u32 a = 0;
	for (; a < accessors.size(); ++a) {
		accessors[a]->invalidate();
	}
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

Accessor*
Storage::createAccessor() {
	Accessor* a = new Accessor(this, rootNode, false);
	accessors.push_back(a);
	return a;
}

void
Storage::registerAccessor( Accessor* acc ) {
	accessors.push_back(acc);
}

void
Storage::deregisterAccessor( Accessor* acc ) {
	irr::u32 a = 0;
	for (; a < accessors.size(); ++a)
	{
		if ( accessors[a] == *acc ) {
			accessors.erase(a);
			break;
		}
	}
}

bool
Storage::convertToCopper( Cu::Function& storage ) {
	if ( !rootNode ) return false;

	// TODO
	return false;
}

bool
Storage::convertFromCopper( Cu::Function& source ) {
	// All accessors must be invalidated
	irr::u32 a = 0;
	for (; a < accessors.size(); ++a) {
		accessors[a]->invalidate();
	}
	accessors.clear();
	filePath = "#Copper#";
	// TODO

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
	util::String filepathString = ((Cu::StringObject&)ffi.arg(1)).getString();
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
	util::String filepathString = ((Cu::StringObject&)ffi.arg(1)).getString();
	((Storage&)ffi.arg(0)).setFilePath( irr::io::path(filepathString.c_str()) );

	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
CreateAccessor( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Storage& jsonStorage = (Storage&)ffi.arg(0);
	ffi.setNewResult(jsonStorage.createAccessor());
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
GetAccessorChildCount( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Accessor::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Accessor& accessor = (Accessor&)ffi.arg(0);
	ffi.setNewResult( new Cu::IntegerObject( accessor.getChildCount() ) );
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
AccessorChild( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Accessor::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::Integer)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Accessor& accessor = (Accessor&)ffi.arg(0);
	Cu::Integer index = ((Cu::IntegerObject&)ffi.arg(1)).getIntegerValue();
	if ( index < 0 )
		index = 0;
	Accessor* child = accessor.getChild( (Cu::UInteger)index );
	if ( child ) {
		ffi.setNewResult( child );
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
AddChild( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Accessor::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::String)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Accessor& accessor = (Accessor&)ffi.arg(0);
	const util::String name = ((Cu::StringObject&)ffi.arg(1)).getString();
	Accessor* child = accessor.addChild( name );
	if ( child ) {
		ffi.setNewResult( child );
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
AccessorParent( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Accessor::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Accessor& accessor = (Accessor&)ffi.arg(0);
	Accessor* parent = accessor.getParent();
	if ( parent ) {
		ffi.setNewResult( parent );
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
GetSetElementName( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgCountRange(1,2)
		|| ! ffi.demandArgType(0, Accessor::getTypeAsCuType())
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	util::String name;
	if ( ffi.getArgCount() == 2 ) {
		if ( ! ffi.demandArgType(1, Cu::ObjectType::String) ) {
			return Cu::ForeignFunc::NONFATAL;
		}
		name = ((Cu::StringObject&)ffi.arg(1)).getString();
		((Accessor&)ffi.arg(0)).setElementName(name);
	}
	else if (
		((Accessor&)ffi.arg(0)).getElementName(name)
	) {
		ffi.setNewResult(new Cu::StringObject(name));
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
GetSetElementAttrs( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgCountRange(1,2)
		|| ! ffi.demandArgType(0, Accessor::getTypeAsCuType())
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Cu::FunctionObject* value = REAL_NULL;
	if ( ffi.getArgCount() == 2 ) {
		if ( ! ffi.demandArgType(1, Cu::ObjectType::Function) ) {
			return Cu::ForeignFunc::NONFATAL;
		}
		((Accessor&)ffi.arg(0)).setElementAttributes(
			(Cu::FunctionObject&)ffi.arg(1)
		);
	}
	else if (
		((Accessor&)ffi.arg(0)).getElementAttributes(value)
	) {
		ffi.setNewResult(value);
	}
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
ConvertJSONToCopper( Cu::FFIServices& ffi ) {
	// TODO? Can be implemented in Copper.
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
ConvertCopperToJSON( Cu::FFIServices& ffi ) {
	// TODO? Can be implemented in Copper.
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
WriteJSON( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Storage& storage = (Storage&)ffi.arg(0);
	util::String filepathString;
	if ( ffi.getArgCount() == 2 ) {
		// TODO Should be an object wrapping irr::io::path instead of String
		if ( ffi.demandArgType(1, Cu::ObjectType::String) ) {
			filepathString = ((Cu::StringObject&)ffi.arg(1)).getString();
			storage.setFilePath( irr::io::path(filepathString.c_str()) );
		} else {
			return Cu::ForeignFunc::NONFATAL;
		}
	}
	storage.writeToFile();

	return Cu::ForeignFunc::FINISHED;
}



} // end namespace json
} // end namespace cubr
