#ifndef _connectPointerToRayZapperCmd
#define _connectPointerToRayZapperCmd
//
// Copyright (C) Daniel Arzabe
// 
// File: connectPointerToRayZapperCmd.h
//
// MEL Command: connectPointerToRayZapper
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxCommand.h>

class MArgList;

class connectPointerToRayZapper : public MPxCommand
{

public:
				connectPointerToRayZapper();
	virtual		~connectPointerToRayZapper();

	MStatus		doIt( const MArgList& );
	MStatus		redoIt();
	MStatus		undoIt();
	bool		isUndoable() const;

	static		void* creator();

private:
	// Store the data you will need to undo the command here
	//
};

#endif
