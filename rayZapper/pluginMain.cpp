//
// Copyright (C) 2009 Daniel Arzabe
// 
// File: pluginMain.cpp
//
// Author: Maya Plug-in Wizard 2.0
//

#include "rayZapperNode.h"
#include "addRayZapperOffsetAttrsCmd.h"
#include "connectPointerToRayZapperCmd.h"
#include "connectCollisionObjToRayZapperCmd.h"
#include <maya/MFnPlugin.h>










MStatus initializePlugin( MObject obj )
//
//	Description:
//		this method is called when the plug-in is loaded into Maya.  It 
//		registers all of the services that this plug-in provides with 
//		Maya.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{ 
	MStatus   status;
	MFnPlugin plugin( obj, "Daniel Arzabe", "3.0", "2011");




	status = plugin.registerNode( "rayZapper", rayZapper::id, rayZapper::creator,
								  rayZapper::initialize );
	if (!status) {
		status.perror("registerNode");
		return status;
	}




	status = plugin.registerCommand( "addOffsetAttrsToRayZapper", addRayZapperOffsetAttrs::creator );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}




	status = plugin.registerCommand( "connectPointerToRayZapper", connectPointerToRayZapper::creator );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}






	status = plugin.registerCommand( "connectCollisionObjToRayZapper", connectCollisionObjToRayZapper::creator );
	if (!status) {
		status.perror("registerCommand");
		return status;
	}









	return status;
}

MStatus uninitializePlugin( MObject obj)
//
//	Description:
//		this method is called when the plug-in is unloaded from Maya. It 
//		deregisters all of the services that it was providing.
//
//	Arguments:
//		obj - a handle to the plug-in object (use MFnPlugin to access it)
//
{
	MStatus   status;
	MFnPlugin plugin( obj );


	status = plugin.deregisterCommand( "addOffsetAttrsToRayZapper" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}


	status = plugin.deregisterCommand( "connectPointerToRayZapper" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}


	status = plugin.deregisterCommand( "connectCollisionObjToRayZapper" );
	if (!status) {
		status.perror("deregisterCommand");
		return status;
	}










	status = plugin.deregisterNode( rayZapper::id );
	if (!status) {
		status.perror("deregisterNode");
		return status;
	}


	



	return status;
}











