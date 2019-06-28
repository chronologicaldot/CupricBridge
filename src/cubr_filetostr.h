// Copyright 2018 Nicolaus Anderson

#ifndef _CUBR_FILE_TO_STR_H_
#define _CUBR_FILE_TO_STR_H_

#include <Strings.h> // from Copper
#include <IReadFile.h> // from Irrlicht
#include <IFileSystem.h> // from Irrlicht

namespace cubr {

inline
util::String
fileToCuStr( irr::io::IReadFile* file ) {
	char* cs = new char[file->getSize()];
	file->read(cs, (size_t)file->getSize());
	return util::String(cs);
}

inline
util::String
fileToCuStr( irr::io::path  p, irr::io::FileSystem*  fileSystem ) {
	irr::io::IReadFile*  file = fileSystem->createAndOpenFile(p);
	if ( file ) {
		return fileToCuStr(file);
	}
	return util::String("");
}

}

#endif
