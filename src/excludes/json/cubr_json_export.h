// (C) 2021 Nicolaus Anderson
#ifndef _CUBR_JSON_EXPORT_H_
#define _CUBR_JSON_EXPORT_H_

#include <IFileSystem.h>
#include <Copper.h>

namespace cubr {

/*
TODO
	One problem inherited by AttributeSource is that Copper Integer and DecimalNum use writeToString()
	only for writing their type name and not their value. Number conversion was avoided for writeToString()
	but I would like numbers to be converted for the JSON exporter.

	AttributeSource can't be used because I need to access sub-members.
	However, I can use Cu::AccessHelper.

TODO:
	It might be good to merge this with cubr_json. The difference is that cubr_json handles JSON in a tree
	whereas this handles direct Copper code. The overlap is that cubr_json can convert to Copper and that
	it already has the IFileSystem.
*/

class JSONExporter {

public:
	JSONExporter( irr::io::IFileSystem* );

	void addToEngine( Cu::Engine& );

	bool exportCopper( Cu::Function&, irr::io::path );

	Cu::ForeignFunc::Result
	ExportCopper( Cu::FFIServices& );
};

} // end namespace cubr

#endif
