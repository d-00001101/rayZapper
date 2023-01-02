#ifndef _rayZapperNode
#define _rayZapperNode







//
// Copyright (C) 2009 Daniel Arzabe 
// 
// File: rayZapperNode.h
//
// Dependency Graph Node: rayZapperNode
//
// Author: Maya SDK Wizard
//

#include <maya/MPxNode.h>
#include <maya/MTypeId.h> 
#include <maya/MNurbsIntersector.h>

class rayZapper : public MPxNode
{
public:
						rayZapper();
	virtual				~rayZapper(); 

	virtual MStatus		compute( const MPlug& plug, MDataBlock& data );
	virtual void      postConstructor();

	static  void*		creator();
	static  MStatus		initialize();

public:


	//			INPUT ATTRS
		
	static  MObject     aInMesh;			// The mesh object to find intersection with
	static	MObject		aInSurface;			// The surface object to find intersection with
	static  MObject		aMeshOrNurbs;		// Will determine if compute function uses the inMesh or inSurface
											// The source of the ray
		
	static  MObject		aInheritOrientation;

	static	MObject		aGetIntersection;
	static	MObject		aGetNormal;
	static	MObject		aGetUVCoordinates;
	static	MObject		aGetDistance;


	static	MObject		aMaxDistance;		

	static	MObject		aNurbsMatrix;
	static	MObject		aMeshMatrix;



	static	MObject		aOnOff;				// If off, intersection call will be skipped
	static	MObject		aInMatrix;			// The in matrix that transforms the ray vector to the mesh's local space
	static	MObject		aOffset;		
	static	MObject		aOffsetX;
	static	MObject		aOffsetY;
	static	MObject		aOffsetZ;
	static	MObject		aDirection;			// Vector indicating ray direction
	static	MObject		aDirectionX;
	static	MObject		aDirectionY;
	static	MObject		aDirectionZ;
	static  MObject		aInputTransform;	//COMPOUND ATTRIBUTE





	static	MObject		aResultCoordinateSpace;
	static	MObject		aOffsetCoordinateSpace;








	//				OUTPUT ATTRS

	static  MObject		aIntersectionPosition;		// The first intersection point (if found)
	static  MObject		aIntersectionPositionX;
	static  MObject		aIntersectionPositionY;
	static  MObject		aIntersectionPositionZ;
	static  MObject		aIntersectionNormal;
	static  MObject		aIntersectionNormalX;			// surface normal at entry point
	static  MObject		aIntersectionNormalY;
	static  MObject		aIntersectionNormalZ;
	static	MObject		aIntersectionUVCoords;
	static	MObject		aIntersectionUVCoordsU;
	static	MObject		aIntersectionUVCoordsV;
	static	MObject		aInDistance;			// The distance to entry point.
	static	MObject		aIntersection;		// was there a first intersection (entry)?


	
	static  MObject		aOutputData;	//COMPOUND ATTRIBUTE


	// The typeid is a unique 32bit indentifier that describes this node.
	// It is used to save and retrieve nodes of this type from the binary
	// file format.  If it is not unique, it will cause file IO problems.
	//
	static	MTypeId		id;


private:

	
};




#endif

