#ifndef _connectCollisionObjToRayZapperCmd
#define _connectCollisionObjToRayZapperCmd
//
// Copyright (C) Daniel Arzabe
// 
// File: connectCollisionObjToRayZapperCmd.h
//
// MEL Command: connectCollisionObjToRayZapper
//
// Author: Maya Plug-in Wizard 2.0
//

#include <maya/MPxCommand.h>

class MArgList;

class connectCollisionObjToRayZapper : public MPxCommand
{

public:
				connectCollisionObjToRayZapper();
	virtual		~connectCollisionObjToRayZapper();

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
