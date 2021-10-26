// (C) 2021 Nicolaus Anderson
#include "cubr_json.h"
#include "../cubr_attr.h"
#include <CuAccessHelper.h>
#include <irrList.h>
#include <IWriteFile.h>

/*
FIXME: Quotation marks in JSON files
Currently, the JSON file loader interprets all attribute values as strings. This is the convenient thing to do, but it would be nice to modify the JSON loader to optionally include the quotation marks in the string so we can use them here to differentiate between true strings that need quotes and strings of numbers.
NOTE: All JSON is strings, even the number representations, so Copper code will need to do the conversion. Copper math functions int() and dcml() will work.
*/

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
	Cu::addForeignFuncInstance(engine, "json_pretty_print", &EnablePrettyPrint);
	Cu::addForeignFuncInstance(engine, "json_init_root", &InitRootNode);
	Cu::addForeignFuncInstance(engine, "json_root", &CreateAccessor);
	Cu::addForeignFuncInstance(engine, "json_is_valid_access", &CheckAccessorValidity);
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
	// TODO: Write the node's attribute value contents (and list of members) into the string.
	// Should I do the entire tree? I'm not sure. You could get the full tree from
	// Storage::writeToString().
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
		childNode = & (node->addNode(new irrJSONElement(), -1));
		// Um... I forgot to create an element
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
			// ^ FIXME: See note at the top about quotation marks in JSON values
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
	for (; attrIdx < attributeSource.getAttributeCount(); ++attrIdx) {
		element->addAttribute(
			attributeSource.getAttributeName(attrIdx),
			attributeSource.getAttributeAsString(attrIdx)
			// ^ FIXME: See note at the top about quotation marks in JSON values
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
	, PrettyPrint(false)
{}

Storage::~Storage() {
	irr::u32 i = 0;
	for (; i < accessors.size(); i += 1)
	{
		accessors[i]->invalidate();
	}
	if ( rootNode ) {
		delete rootNode;
		rootNode = 0;
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
	//out = "{CuBridge JSON Storage}";
	// Write the entire JSON tree to a string. Useful for exporting.
	// Tabs and newlines are enabled/disabled with a Storage-internal flag, PrettyPrint.
	CharList charlist;
	writeNodeToString(rootNode, charlist, 0);
	out = charlist;
}

void
Storage::writeNodeToString(irrTreeNode* node, CharList& out, unsigned depth) const {
	if (!node) return;
	CharList tabs;
	unsigned t=0;
	if ( PrettyPrint )
		for(; t < depth; ++t) { tabs.append("\t"); }

	irrJSONElement* element = (irrJSONElement*)node->getElem();
	String colonQuote = PrettyPrint? ": \"" : ":\"";
	irr::core::list<irrJSONElement::Attribute>::Iterator attrItr = element->getAttributes().begin();
	for(; attrItr != element->getAttributes().end(); ++attrItr ) {
		if ( PrettyPrint ) out.append(tabs);
		out.append( String( (*attrItr).name.c_str() ) );
		// FIXME: See note at top about quotation marks in JSON values
		out.append(colonQuote);
		out.append( String( (*attrItr).value.c_str() ) );
		out.append( "\"," );
		if ( PrettyPrint ) out.append("\n");
	}

	irrJSONElement* childElem;
	irr::core::string<c8> colonBracket = PrettyPrint? ": {" : ":{";
	unsigned c=0;
	for(; c < node->children.size(); ++c ) {
		childElem = (irrJSONElement*)node->children[c]->getElem();
		if ( PrettyPrint ) out.append(tabs);
		out.append( (childElem->getName() + colonBracket).c_str() );
		if ( PrettyPrint ) out.append("\n");
		writeNodeToString( node->children[c], out, depth+1 );
		if ( PrettyPrint ) out.append(tabs);
		out.append("}");
		if ( c != node->children.size() - 1) out.append(",");
		if ( PrettyPrint ) out.append("\n");
	}
	//if ( PrettyPrint ) out.append("\n");
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
	if ( rootNode ) {
		delete rootNode;
		rootNode = 0;
	}
	filePath = p;
	return json.parseFile(filePath, rootNode);
}

void
Storage::setFilePath( const irr::io::path& p ) {
	filePath = p;
}

void Storage::initializeRoot() {
	if ( rootNode )
		delete rootNode;
	rootNode = new irrTreeNode(0, 0, new irrJSONElement());
}

bool
Storage::writeToFile( const irr::io::path* newFilePath ) {
	irr::io::path finalPath;
	if ( newFilePath != 0 && newFilePath->size() != 0) {
		finalPath = *newFilePath;
	}
	if ( finalPath.size() == 0 ) {
		if ( filePath.size() == 0 ) return false;
		finalPath = filePath;
	}

	irr::io::IWriteFile* outFile = fileSystem->createAndWriteFile(finalPath, false);
	if ( !outFile ) return false;

	String out;
	writeToString(out);
	outFile->write(out.c_str(), out.size());

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

/*
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
*/

//-------------------------------

Cu::ForeignFunc::Result
OpenAndParse( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| ! ffi.demandArgType(1, Cu::ObjectType::String) // TODO Should be an object wrapping irr::io::path
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	util::String filepathString = ((Cu::StringObject&)ffi.arg(1)).getString();
	bool result = ((Storage&)ffi.arg(0)).parseFile( irr::io::path(filepathString.c_str()) );
	ffi.setNewResult( new Cu::BoolObject(result) );
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
EnablePrettyPrint( Cu::FFIServices& ffi ) {
	if ( !ffi.demandArgType(0, Storage::getTypeAsCuType())
		|| !ffi.demandArgType(1, Cu::ObjectType::Bool)
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	((Storage&)ffi.arg(0)).PrettyPrint = ((Cu::BoolObject&)ffi.arg(1)).getValue();
	return Cu::ForeignFunc::FINISHED;
}

Cu::ForeignFunc::Result
InitRootNode( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	((Storage&)ffi.arg(0)).initializeRoot();
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
CheckAccessorValidity( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Accessor::getTypeAsCuType()) ) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Accessor& accessor = (Accessor&)ffi.arg(0);
	ffi.setNewResult( new Cu::BoolObject( accessor.isValid() ) );
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

/*
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
*/

Cu::ForeignFunc::Result
WriteJSON( Cu::FFIServices& ffi ) {
	if ( ! ffi.demandArgType(0, Storage::getTypeAsCuType())
	) {
		return Cu::ForeignFunc::NONFATAL;
	}
	Storage& storage = (Storage&)ffi.arg(0);
	util::String filepathString;
	irr::io::path filepath;
	if ( ffi.getArgCount() == 2 ) {
		// TODO Should be an object wrapping irr::io::path instead of String
		if ( ffi.demandArgType(1, Cu::ObjectType::String) ) {
			filepathString = ((Cu::StringObject&)ffi.arg(1)).getString();
			filepath = irr::io::path(filepathString.c_str());
			//storage.setFilePath( filepath );
		} else {
			return Cu::ForeignFunc::NONFATAL;
		}
	}
	storage.writeToFile(&filepath);

	return Cu::ForeignFunc::FINISHED;
}



} // end namespace json
} // end namespace cubr
