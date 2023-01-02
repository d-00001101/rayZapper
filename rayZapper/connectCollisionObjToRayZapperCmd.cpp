//
// Copyright (C) Daniel Arzabe
// 
// File: connectCollisionObjToRayZapperCmd.cpp
//
// MEL Command: connectCollisionObjToRayZapper
//
// Author: Maya Plug-in Wizard 2.0
//

#include "connectCollisionObjToRayZapperCmd.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MCommandResult.h>

MStatus connectCollisionObjToRayZapper::doIt( const MArgList& )
{
	MStatus stat = MS::kSuccess;


	// Typically, the doIt() method only collects the infomation required
	// to do/undo the action and then stores it in class members.  The 
	// redo method is then called to do the actuall work.  This prevents
	// code duplication.
	//
	return redoIt();
}












MStatus connectCollisionObjToRayZapper::redoIt()
{

	MStatus stat = MS::kSuccess;

	//this will capture the result of the mel proc so that it may
	//be returned after calling the command
	MCommandResult myComResult;
	//this mel proc was declared in the post constructor of velocityLocator class
	MGlobal::executeCommand(MString("da_connectCollisionObjToRayZapper();"), myComResult, false, true);



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








MStatus connectCollisionObjToRayZapper::undoIt()
{

	// You can also display information to the command window via MGlobal
	//
    //MGlobal::displayInfo( "addRayZapperOffsetAttrs command undone!\n" );

	return MS::kSuccess;
}








void* connectCollisionObjToRayZapper::creator()
{
	return new connectCollisionObjToRayZapper();
}







connectCollisionObjToRayZapper::connectCollisionObjToRayZapper()
{}






connectCollisionObjToRayZapper::~connectCollisionObjToRayZapper()
{
}




bool connectCollisionObjToRayZapper::isUndoable() const
{
	return true;
}
