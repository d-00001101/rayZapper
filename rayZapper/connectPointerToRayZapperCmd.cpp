//
// Copyright (C) Daniel Arzabe
// 
// File: connectPointerToRayZapperCmd.cpp
//
// MEL Command: connectPointerToRayZapper
//
// Author: Maya Plug-in Wizard 2.0
//

#include "connectPointerToRayZapperCmd.h"

#include <maya/MGlobal.h>
#include <maya/MString.h>
#include <maya/MCommandResult.h>

MStatus connectPointerToRayZapper::doIt( const MArgList& )
{
	MStatus stat = MS::kSuccess;


	// Typically, the doIt() method only collects the infomation required
	// to do/undo the action and then stores it in class members.  The 
	// redo method is then called to do the actuall work.  This prevents
	// code duplication.
	//
	return redoIt();
}












MStatus connectPointerToRayZapper::redoIt()
{

	MStatus stat = MS::kSuccess;

	// Since this class is derived off of MPxCommand, you can use the 
	// inherited methods to return values and set error messages
	//




	//this will capture the result of the mel proc so that it may
	//be returned after calling the command
	MCommandResult myComResult;
	//this mel proc was declared in the post constructor of velocityLocator class
	MGlobal::executeCommand(MString("da_connectPointerToRayZapper();"), myComResult, false, true);



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








MStatus connectPointerToRayZapper::undoIt()
{

	// You can also display information to the command window via MGlobal
	//
    //MGlobal::displayInfo( "addRayZapperOffsetAttrs command undone!\n" );

	return MS::kSuccess;
}








void* connectPointerToRayZapper::creator()
{
	return new connectPointerToRayZapper();
}







connectPointerToRayZapper::connectPointerToRayZapper()
{}






connectPointerToRayZapper::~connectPointerToRayZapper()
{
}




bool connectPointerToRayZapper::isUndoable() const
{
	return true;
}
