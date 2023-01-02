#ifndef _addRayZapperOffsetAttrsCmd
#define _addRayZapperOffsetAttrsCmd
//
// Copyright (C) Daniel Arzabe
// 
// File: addRayZapperOffsetAttrsCmd.h
//
// MEL Command: addRayZapperOffsetAttrs
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxCommand.h>

class MArgList;

class addRayZapperOffsetAttrs : public MPxCommand
{

public:
				addRayZapperOffsetAttrs();
	virtual		~addRayZapperOffsetAttrs();

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
