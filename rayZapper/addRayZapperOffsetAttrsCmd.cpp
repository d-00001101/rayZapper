//
// Copyright (C) Daniel Arzabe
// 
// File: addRayZapperOffsetAttrsCmd.cpp
//
// MEL Command: addRayZapperOffsetAttrs
//
// Author: Maya Plug-in Wizard 2.0
//

#include "addRayZapperOffsetAttrsCmd.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MCommandResult.h>

MStatus addRayZapperOffsetAttrs::doIt( const MArgList& )
{
	MStatus stat = MS::kSuccess;


	// Typically, the doIt() method only collects the infomation required
	// to do/undo the action and then stores it in class members.  The 
	// redo method is then called to do the actuall work.  This prevents
	// code duplication.
	//
	return redoIt();
}












MStatus addRayZapperOffsetAttrs::redoIt()
{

	MStatus stat = MS::kSuccess;


	//this will capture the result of the mel proc so that it may
	//be returned after calling the command
	MCommandResult myComResult;
	//this mel proc was declared in the post constructor of velocityLocator class
	MGlobal::executeCommand(MString("da_addRayZapperOffsetAttrs();"), myComResult, false, true);



	//This will hold the return data
	MStringArray resultStringArray;
	stat = myComResult.getResult(resultStringArray);
	if(!stat)
		MGlobal::displayInfo(MString("Could not get return values"));
	else
	{
		setResult( resultStringArray );
	}

	 return stat;
}








MStatus addRayZapperOffsetAttrs::undoIt()
{
	return MS::kSuccess;
}








void* addRayZapperOffsetAttrs::creator()
{
	return new addRayZapperOffsetAttrs();
}







addRayZapperOffsetAttrs::addRayZapperOffsetAttrs()
{}






addRayZapperOffsetAttrs::~addRayZapperOffsetAttrs()
{
}




bool addRayZapperOffsetAttrs::isUndoable() const
{
	return true;
}
