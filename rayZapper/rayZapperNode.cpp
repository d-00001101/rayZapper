
// Copyright (C) 2011 Daniel Arzabe
// 
// File: rayZapperNode.cpp
//
// Dependency Graph Node: rayZapper
//
// Author: Maya SDK Wizard
//
// feb 17 2010 - added command: connectVelocityLocatorPointerToRayZapper
// april 2011 - added result coordinate space and offset coordinate space enum attrs making the
//				worldOrLocal coordinate space attribute obsolete (has been removed).


#include "rayZapperNode.h"
#include "closestNormalUVAndFace.h"

#include <maya/MPlug.h>
#include <maya/MVector.h>
#include <maya/MPoint.h>
#include <maya/MFloatPoint.h>
#include <maya/MFloatMatrix.h>
#include <maya/MFloatVector.h>
#include <maya/MTransformationMatrix.h>
#include <maya/MMatrix.h>
#include <maya/MDoubleArray.h>
#include <maya/MDistance.h>
#include <maya/MPointArray.h>
#include <maya/MIntArray.h>
#include <maya/MFnMesh.h>
#include <maya/MFnNurbsSurface.h>
#include <maya/MFnMeshData.h>
#include <maya/MDagPath.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnCompoundAttribute.h> 
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MArrayDataBuilder.h>
#include <maya/MGlobal.h>
#include <maya/MObject.h>
#include <maya/MObjectHandle.h>
#include <maya/MFnDagNode.h>
#include <maya/MDagPath.h>
#include <maya/MNurbsIntersector.h>
#include <maya/MMeshIntersector.h>

#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnEnumAttribute.h>

#include <iostream>

#define MG_INFO(status,message)		\
	if( MStatus::kSuccess == status ) {	\
		MGlobal::displayInfo( MString(message) );		\
	}

#define MG_ERR(status,message)		\
	if( MStatus::kFailure == status ) {	\
		MGlobal::displayError( MString(message) );		\
	}

#define McheckCode(status,messageSuccess, messageFail)		\
	if( MStatus::kSuccess == status ) {						\
		MGlobal::displayInfo( MString(messageSuccess));}					\
	else if(MStatus::kFailure == status){									\
		MGlobal::displayInfo( MString(messageFail) );						\
	}	\
	else {	\
		MGlobal::displayInfo( MString("something else happened ") );	\
	}


#define MG_VECTOR(message, vector)	\
	MGlobal::displayInfo( MString(message) + MString( "[ ") + vector.x + MString(", ") +  vector.y  + MString(", ") + vector.z + MString(" ]"));

//prints out simple data types: floats doubles ints
#define MG_VAR(message, var)	\
	MGlobal::displayInfo( MString(message) + var );

#define MG_DI(message) \
	MGlobal::displayInfo( MString(message));

#define MG_MSTRING(MStringVar) \
	MGlobal::displayInfo( MStringVar );
	



 

// You MUST change this to a unique value!!!  The id is a 32bit value used
// to identify this type of node in the binary file format.  
//

MTypeId     rayZapper::id( 0x00021 );

// attributes
// 
MObject     rayZapper::aInMesh;			// The mesh object with which to find intersection
MObject     rayZapper::aInSurface;		// The surface object with which to find intersection
MObject		rayZapper::aMeshOrNurbs;	//boolean calculates ray intersection on nurbs or poly
MObject		rayZapper::aInheritOrientation;

MObject		rayZapper::aMaxDistance;
MObject		rayZapper::aNurbsMatrix;
MObject		rayZapper::aMeshMatrix;

MObject		rayZapper::aGetIntersection;
MObject		rayZapper::aGetNormal;
MObject		rayZapper::aGetUVCoordinates;
MObject		rayZapper::aGetDistance;

MObject		rayZapper::aOnOff;
MObject		rayZapper::aInMatrix;		// The source of the ray - 
MObject		rayZapper::aOffset;		
MObject		rayZapper::aOffsetX;
MObject		rayZapper::aOffsetY;
MObject		rayZapper::aOffsetZ;
MObject		rayZapper::aDirection;		// Vector indicating ray direction
MObject		rayZapper::aDirectionX;
MObject		rayZapper::aDirectionY;
MObject		rayZapper::aDirectionZ;

MObject		rayZapper::aIntersectionPosition;			// The first intersection point (if found)
MObject		rayZapper::aIntersectionPositionX;
MObject		rayZapper::aIntersectionPositionY;
MObject		rayZapper::aIntersectionPositionZ;
MObject		rayZapper::aIntersectionNormal;
MObject		rayZapper::aIntersectionNormalX;			// surface normal at entry point
MObject		rayZapper::aIntersectionNormalY;
MObject		rayZapper::aIntersectionNormalZ;
MObject		rayZapper::aIntersectionUVCoords;
MObject		rayZapper::aIntersectionUVCoordsU;
MObject		rayZapper::aIntersectionUVCoordsV;
MObject		rayZapper::aInDistance;		
MObject		rayZapper::aIntersection;

MObject		rayZapper::aInputTransform;			//compound array attr
MObject		rayZapper::aOutputData;

MObject		rayZapper::aResultCoordinateSpace;
MObject		rayZapper::aOffsetCoordinateSpace;


rayZapper::rayZapper() {}
rayZapper::~rayZapper() {}





//An MDataBlock provides storage for the data being received by or sent by the node. It is the data 
//for the plugs and attributes of the node. The data block is only valid during the compute method 
//of the depend node. Pointers to the data block should not be retained after the compute method.
MStatus rayZapper::compute( const MPlug& plug, MDataBlock& data )
{

	// Check which output attribute we have been asked to compute.  If this 
	// node doesn't know how to compute it, we must return 
	// MS::kUnknownParameter.
	// 
	if( (plug == aOutputData) ||
		(plug == aIntersectionPosition) || 
		(plug == aIntersectionPositionX) || (plug == aIntersectionPositionY) || (plug == aIntersectionPositionZ) ||
		(plug == aIntersectionNormal) || 
		(plug == aIntersectionNormalX) || (plug == aIntersectionNormalY) || (plug == aIntersectionNormalZ) ||
		(plug == aIntersectionUVCoords )  || 
		(plug == aIntersectionUVCoordsU ) || (plug == aIntersectionUVCoordsV ) || 
		(plug == aInDistance) || (plug == aIntersection) || (plug == aResultCoordinateSpace) || (plug == aOffsetCoordinateSpace) )

	{

		MStatus stat;

		// Get bool DATA
		MDataHandle meshOrSurfaceDH = data.inputValue(aMeshOrNurbs, &stat );
			//MG_ERR(stat,"ERROR: cannot get meshOrSurface BOOL");
		
		const bool meshOrNurbs = meshOrSurfaceDH.asBool();
			//MG_VAR("meshOrNurbs: ", meshOrNurbs);


		MDataHandle getIntersectionDH = data.inputValue(aGetIntersection, &stat );
			//MG_ERR(stat,"ERROR: cannot get aGetIntersection BOOL");

		const bool getIntersection = getIntersectionDH.asBool();
			//MG_VAR("getIntersection from DH.asBool: ", getIntersection);



		MDataHandle getNormalDH = data.inputValue(aGetNormal, &stat );
			//MG_ERR(stat,"ERROR: cannot get aGetNormal BOOL");

		const bool getNormal = getNormalDH.asBool();
			//MG_VAR("getNormal from  DH.asBool: ", getNormal);


		MDataHandle getUVCoordsDH = data.inputValue(aGetUVCoordinates, &stat );
			//MG_ERR(stat,"ERROR: cannot get aGetUVCoordinates BOOL");

		const bool getUVCoords = getUVCoordsDH.asBool();
			//MG_VAR("getUVCoords from DH.asBool: ", getUVCoords);



		MDataHandle getDistanceDH = data.inputValue(aGetDistance, &stat );
			//MG_ERR(stat,"ERROR: cannot get aGetUVCoordinates BOOL");

		const bool getDistance = getDistanceDH.asBool();
			//MG_VAR("getDistance from DH.asBool: ", getDistance);



		// Get ".maxDistance" DATA:
		MDataHandle maxDistanceDH = data.inputValue(aMaxDistance, &stat );
			//MG_ERR(stat,"ERROR: cannot get maxDistance"); 

		//aMaxDistance to zap ray
		float maxParam = maxDistanceDH.asFloat();
			//MG_VAR("the maxParam from DH.asFloat = ", maxParam)












		//////////     get Coordinate Space Values    /////////////
		//get Result Coordinate Space
		MDataHandle resultCoordDH = data.inputValue(aResultCoordinateSpace, &stat);
			//MG_ERR(stat,"ERROR: cannot get aResultCoordinateSpace");

		int resultCoordSpace;
		resultCoordSpace = resultCoordDH.asShort();
			//MG_VAR("Result coord space = ", resultCoordSpace)


		//get Result Coordinate Space
		MDataHandle offsetCoordDH = data.inputValue(aOffsetCoordinateSpace, &stat);
			//MG_ERR(stat,"ERROR: cannot get aOffsetCoordinateSpace");

		int offsetCoordSpace;
		offsetCoordSpace = offsetCoordDH.asShort();
			//MG_VAR("Offset coord space = ", offsetCoordSpace)










		// Get "aInheritOrientation" bool DATA
		MDataHandle inheritOrientDH = data.inputValue(aInheritOrientation, &stat );
			//MG_ERR(stat,"ERROR: cannot get aInheritOrientation BOOL");

		bool inheritOrient = inheritOrientDH.asBool();

		MDistance::Unit internalUnit = MDistance::internalUnit();



		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//
		//				OO    OO  OO     OO  OOOOOOO   OOOOOOO    OOOOOO 
		//	            OO    OO  OO     OO  OO    OO  OO    OO  OO    OO
		//				OOO   OO  OO     OO  OO    OO  OO    OO  OO
		//				OO O  OO  OO     OO  OOOOOOO   OOOOOOO    OOOOOO
		//				OO  O OO  OO     OO  OO    OO  OO    OO        OO
		//				OO	 OOO   OO   OO   OO    OO  OO    OO  OO    OO
		//				OO    OO     OOO     OO    OO  OOOOOOO   OOOOOO
		//
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		if(meshOrNurbs && getIntersection)
		{
			
			//MG_DI("START NURBS")


			//  Get "inSurface" DATA:
			MDataHandle inSurfaceDH = data.inputValue( aInSurface, &stat );
				//MG_ERR(stat,"ERROR: cannot get inSurface");
				//MG_DI("inSurfaceDataType" + inSurfaceDH.type());


			MObject inNurbsObj = inSurfaceDH.asNurbsSurface();	

			MObjectHandle surfaceObjDH;

			surfaceObjDH = inNurbsObj;
			bool isValidSurface = surfaceObjDH.isValid();
				//MG_VAR("isValidSurface: ", isValidSurface);




	

			




			
			MMatrix collGeoMatrix;
			MNurbsIntersector nurbsIntersector;

			MDataHandle collGeoMatrixDH = data.inputValue( aNurbsMatrix, &stat);
				//MG_ERR(stat, "Could not get nurbs matrix data handle")

			collGeoMatrix  = collGeoMatrixDH.asMatrix();

			MTransformationMatrix collGeoTransMatrix(collGeoMatrix);	//this matrix is used to change the offset rotation coordinate space 
																		//to that of the collision geometry's


			
			stat = nurbsIntersector.create(inNurbsObj, collGeoMatrix.inverse() );	//why did I choose the inverse matrix???
																					//because if you don't the result IS inverted.
																					//this works:)








			//             INPUT HANDLE ARRAYS

			MArrayDataHandle inTransformADH = data.inputArrayValue(aInputTransform, &stat );
				//MG_ERR(stat, "Could not get InputTransform arrayDataHandle")
			unsigned int elementCount = inTransformADH.elementCount(&stat);
				//MG_ERR(stat, "could not get element count")



			


			//             OUTPUT HANDLE ARRAYS
			MArrayDataHandle outDataADH = data.outputArrayValue( aOutputData );

			MArrayDataBuilder outDataBuilder(aOutputData, elementCount, &stat);
				//MG_ERR(stat, "Could not create output MArrayDataBuilder")







			//MESH AND NURBS SURFACE INTERSECT FUNCTION VARIABLES

			//bool * wasExactHit;
			double uParam = 0.0;
			double vParam = 0.0;

			MPoint pntOfIntersection = MPoint::origin;


			//determines if intersect function was a success
			bool intersectResult = false;

			//distance from ray origin to point of intersection
			double intersectionDistance = 0.0;
			MVector difference(0.0,0.0,0.0);

			//the intersectionDistance will be fed to the out handle via the MDistance class
			MDistance intersectionDistanceLength (0.0, internalUnit);

			//the normal to the surface at the point of intersection
			MVector intersectionNormalValue = MVector::zero;//null vector

			




			//the out dataHandle for the interection position
			MDataHandle interPosDH;
			MDataHandle interNormDH;
			MDataHandle interUVsDH;
			MDataHandle distanceDH;
			MDataHandle intersectionDH;

			MFnNurbsSurface surfaceFn(inNurbsObj);









			//////////////////////////////////////////////////////////////////////

								
								//LOOPING


			//////////////////////////////////////////////////////////////////////
			for(unsigned int i = 0; i < elementCount; i++)
			{

				//get output data handles
				MDataHandle currentParentOutDH = outDataBuilder.addElement(i, &stat);
					//MG_ERR(stat, "Can not get handle to dataBuilder.addElement")
					//MG_INFO(stat, "Got Handle to dataBuilder.addElement")
				


				interPosDH = currentParentOutDH.child(aIntersectionPosition);
				interNormDH = currentParentOutDH.child(aIntersectionNormal);
				interUVsDH = currentParentOutDH.child(aIntersectionUVCoords);
				distanceDH = currentParentOutDH.child(aInDistance);
				intersectionDH = currentParentOutDH.child(aIntersection);




				//get input data handles
				MDataHandle currentInTransDH = inTransformADH.inputValue(&stat);
					//MG_ERR(stat, "Could not get InputTransform dataHandle")

				MDataHandle currentOnOffDH = currentInTransDH.child(aOnOff);
				bool onOff = currentOnOffDH.asBool();

				

				MDataHandle inMatrixDH = currentInTransDH.child(aInMatrix);

				//This transformation matrix will transform the position vector
				MTransformationMatrix inTransMatrix = inMatrixDH.asMatrix();
				MMatrix inMatrix = inTransMatrix.asMatrix();
					


				//this is the world space position of the pointer
				MVector wsPointerPosition = inTransMatrix.getTranslation(MSpace::kTransform, &stat);
					//MG_VECTOR("inMatrix Transform = ", wsPointerPosition) 


				//This matrix will rotate the direction vector
				MMatrix directionMatrix = inTransMatrix.asRotateMatrix();


				//get the children of current compound attr
				MDataHandle inOffsetDH = currentInTransDH.child(aOffset);

				MPoint inOffsetValue (inOffsetDH.asVector());
					

				if(offsetCoordSpace == 1)//collision object space
				{
					//transform the raw world space offset value to the collision object's local ROTATION space
					//the rotate matrix will ignore the positional offset of the collision geometry
					inOffsetValue *= collGeoTransMatrix.asRotateMatrix();
						//MG_VECTOR("offset in collision obj space = ", inOffsetValue)
				}
				else if(offsetCoordSpace == 2)//pointer object space
				{
					//transform the raw world space offset value to the pointer object's local ROTATION space
					//the rotate matrix will ignore the positional offset of the pointer
					inOffsetValue *= directionMatrix;
						//MG_VECTOR("offset in pointer obj space = ", inOffsetValue)
				}


				//if the inMatrix (pointer) is not the identity matrix, multiply by the point
				if(inMatrix != MMatrix::identity)
				{
					inOffsetValue += wsPointerPosition;
				}









				MDataHandle inDirDH = currentInTransDH.child(aDirection);
				MVector directionValue = inDirDH.asVector();
					

				if(inheritOrient)
				{
					directionValue = directionValue.transformAsNormal(directionMatrix);
						
				}

				

				//custom status set to failure.  If onOff condition is false,
				//intersectStat will deny the rest of intersection calls on the 
				//nurbs surface
				MStatus intersectStat = MS::kFailure;
				if(onOff)
				{
					//the intersection will be produced in the collision object's local space
					//intersectStat will determine if the rest of the NURBS intersection data gathering calls are made
					intersectStat = nurbsIntersector.getIntersect(inOffsetValue, directionValue, uParam, vParam, pntOfIntersection, kMFnNurbsEpsilon);
				}






				//    SET OUPUT DATA:

				//set the output if intersected
				if(intersectStat == MS::kSuccess)
				{
					
						//MG_VECTOR("intersection point (RAW) = ", pntOfIntersection);

					//set the output UV coords
					interUVsDH.set( uParam, vParam );


					//set the output normal
					if(getNormal)
					{
						intersectionNormalValue = surfaceFn.normal(uParam, vParam, MSpace::kObject, &stat);
							//MG_VECTOR("Normal in kObjectSpace = ", intersectionNormalValue)
					}
					



					///////////////////////////////////////////////////////////////////////////////////
					////////////   Set Output IntersectionPoint and Normal     ////////////////////////
					///////////////////////////////////////////////////////////////////////////////////

					if(resultCoordSpace == 0)//world space object space
					{
						//multiplying by collision geometry's matrix will convert return values to collision obj's space
						pntOfIntersection *= collGeoMatrix;
							//MG_VECTOR("intersection in ws = ", pntOfIntersection)

						intersectionNormalValue *= collGeoTransMatrix.asRotateMatrix();
							//MG_VECTOR("Normal in ws = ", intersectionNormalValue)
					}
					else if(resultCoordSpace == 2)//pointer object space.  If neither conditions are met 
												  //then output variables will retain their world space values
					{
						//first we transform the intersection point from collision obect space to worl space
						pntOfIntersection *= collGeoMatrix;
						//then we transform the intersection point from world to pointer's coord space by multiplying by its matrix
						pntOfIntersection *= inMatrix.inverse();
							//MG_VECTOR("intersection in pointer space = ", pntOfIntersection)

						intersectionNormalValue *= collGeoTransMatrix.asRotateMatrix();
						intersectionNormalValue *= directionMatrix;
							//MG_VECTOR("Normal in pointer space = ", intersectionNormalValue)
					}




					
					interPosDH.setMVector(  MVector(pntOfIntersection.x, pntOfIntersection.y, pntOfIntersection.z)  );
					interNormDH.setMVector( intersectionNormalValue );















					//set the output distance
					
					difference = pntOfIntersection - inOffsetValue;
					intersectionDistance = difference.length();

					stat = intersectionDistanceLength.setValue(intersectionDistance);
						//McheckCode(stat, "Converted double to MDistance", "Failed to convert double to MDistance")

					distanceDH.set(intersectionDistanceLength);

					//if distance to intersection exceeds maxDistance set by user then
					//set intersection to false
					if(intersectionDistance > maxParam)
					{
						intersectResult = false;
					}
					else
					{
						intersectResult = true;
					}

					intersectionDH.set(intersectResult);


				}
				else
				{
					intersectionDH.set(intersectResult);//false
					interPosDH.setMVector(MVector(0.0,0.0,0.0) );
					interUVsDH.set( uParam, vParam );
					interNormDH.setMVector( intersectionNormalValue );
					distanceDH.set(intersectionDistanceLength);
				}





				//jump to the next element in the input connections array
				stat = inTransformADH.next();
				

			}



			stat = outDataADH.set(outDataBuilder);
				//MG_ERR(stat, "Could not set builder to ArrayDataHandle")
				//MG_INFO(stat, "Just set dataBuilder to arrayDataHandle")
			stat = outDataADH.setAllClean();
				//MG_ERR( stat, "Could not setAllClean()" )
				//MG_INFO( stat, "just setAllClean() to arrayDataHanlde")	
		}
		else if(!meshOrNurbs && getIntersection)
		{
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//
			//				OO      OO   OOOOOOOO    OOOOOO    OO    OO
			//	            OO      OO   OO         OO    OO   OO    OO
			//				OOO    OOO   OO         OO 		   OO    OO
			//				OO O  O OO   OOOOOOOO    OOOOOO    OOOOOOOO
			//				OO  OO  OO   OO               OO   OO    OO 
			//				OO  OO  OO   OO         OO    OO   OO    OO
			//				OO      OO   OOOOOOOO    OOOOOO    OO    OO
			//
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			//MG_DI("START MESH")

			// Get ".inMesh" DATA:
			MDataHandle inMeshDH = data.inputValue( aInMesh, &stat );
				//MG_ERR(stat,"ERROR: cannot get inMesh"); 

			MObject inMeshObj = inMeshDH.asMesh();
				//MG_DI(inMeshDH.asString())

			MObjectHandle meshObjDH;

			//meshObjDH = inMeshObj;
			//bool isValidMesh = meshObjDH.isValid();
				//std::cout << "Is the Mesh valid? " << isValidMesh << "\n";



			MFnMesh meshFn( inMeshObj, &stat);
				//McheckCode(stat, "created meshFn", "Could not create meshFn")










			









			
			MMatrix collGeoMatrix;

			MDataHandle collGeoMatrixDH = data.inputValue( aMeshMatrix, &stat);
				//MG_ERR (stat, "Could not get mesh matrix data handle")

			collGeoMatrix  = collGeoMatrixDH.asMatrix();
			MTransformationMatrix collGeoTransMatrix(collGeoMatrix);	//this matrix is used to change the offset rotation coordinate space 
																		//to that of the collision geometry's

			

			
			





			










			//             INPUT HANDLE ARRAYS

			MArrayDataHandle inTransformADH = data.inputArrayValue(aInputTransform, &stat );
				//MG_ERR(stat, "Could not get InputTransform arrayDataHandle")
			unsigned int elementCount = inTransformADH.elementCount(&stat);
				//MG_ERR(stat, "could not get element count")
				


			


			//             OUTPUT HANDLE ARRAYS
			MArrayDataHandle outDataADH = data.outputArrayValue( aOutputData );

			MArrayDataBuilder outDataBuilder(aOutputData, elementCount, &stat);
				//MG_ERR(stat, "Could not create output MArrayDataBuilder")






			//MESH AND NURBS SURFACE INTERSECT FUNCTION VARIABLES

			//bool * wasExactHit;
			double uParam = 0.0;
			double vParam = 0.0;

			float2 uvPoint;

			MFloatPoint pntOfIntersection_f(MFloatPoint::origin);

			//determines if intersect function was a success
			bool intersectResult = false;

			//distance from ray origin to point of intersection
			float intersectionDistance = 0.0f;


			//the intersectionDistance will be fed to the out handle via the MDistance class
			MDistance intersectionDistanceLength (0.0, internalUnit);

			//the normal to the surface at the point of intersection
			MVector intersectionNormalValue = MVector::zero;//null vector




			//the out dataHandle for the interection position
			MDataHandle interPosDH;
			MDataHandle interNormDH;
			MDataHandle interUVsDH;
			MDataHandle distanceDH;
			MDataHandle intersectionDH;


		
			MMeshIsectAccelParams meshAccel = meshFn.autoUniformGridParams();











			for(unsigned int i = 0; i < elementCount; i++)
			{

				//get output data handles
				MDataHandle currentParentOutDH = outDataBuilder.addElement(i, &stat);
					//McheckCode(stat, "Got Handle to dataBuilder.addElement", "Can not get handle to dataBuilder.addElement")
				

				


				interPosDH = currentParentOutDH.child(aIntersectionPosition);
				interNormDH = currentParentOutDH.child(aIntersectionNormal);
				interUVsDH = currentParentOutDH.child(aIntersectionUVCoords);
				distanceDH = currentParentOutDH.child(aInDistance);
				intersectionDH = currentParentOutDH.child(aIntersection);




				

				//get input data handles
				MDataHandle currentInTransDH = inTransformADH.inputValue(&stat);
					//MG_ERR(stat, "Could not get InputTransform dataHandle")

				MDataHandle currentOnOffDH = currentInTransDH.child(aOnOff);
				bool onOff = currentOnOffDH.asBool();

				if(onOff)
				{
					MDataHandle inMatrixDH = currentInTransDH.child(aInMatrix);

					//This transformation matrix will transform the position vector
					MTransformationMatrix inTransMatrix = inMatrixDH.asMatrix();
					MMatrix inMatrix = inTransMatrix.asMatrix();
						


					MVector wsPointerPosition = inTransMatrix.getTranslation(MSpace::kTransform, &stat);
						//MG_VECTOR("---Translation offset = ", wsPointerPosition)


					//This matrix will rotate the direction vector
					MMatrix directionMatrix = inTransMatrix.asRotateMatrix();


					//get the children of current compound attr
					MDataHandle inOffsetDH = currentInTransDH.child(aOffset);

					MPoint inOffsetValue (inOffsetDH.asVector());
						

					//the collision face id number
					int hitFace;



					
					//MESH	
					if(offsetCoordSpace == 1)//collision object space
					{
						//transform the raw world space offset value to the collision object's local ROTATION space
						//the rotate matrix will ignore the positional offset of the collision geometry
						inOffsetValue *= collGeoTransMatrix.asRotateMatrix();
							//MG_VECTOR("offset in collision obj space = ", inOffsetValue)
					}
					else if(offsetCoordSpace == 2)//pointer object space
					{
						//transform the raw world space offset value to the pointer object's local ROTATION space
						//the rotate matrix will ignore the positional offset of the pointer
						inOffsetValue *= directionMatrix;
							//MG_VECTOR("offset in pointer obj space = ", inOffsetValue)
					}



					//if the inMatrix is not the identity matrix, multiply by the point
					if(inMatrix != MMatrix::identity)
					{
						inOffsetValue += wsPointerPosition;
							//MG_VECTOR("---inOffsetValue + transOffset = ", inOffsetValue)
					}

					







					MDataHandle inDirDH = currentInTransDH.child(aDirection);
					MVector directionValue = inDirDH.asVector();
						//MG_VECTOR("raw directionValue = ", directionValue)

					if(inheritOrient)
					{
						directionValue = directionValue.transformAsNormal(directionMatrix);
							//MG_VECTOR("---directionValue transformed = ", directionValue)
					}








					MFloatPoint inOffsetValue_f;
					stat = inOffsetValue_f.setCast(inOffsetValue);
						//McheckCode(stat, "cast inOffsetValue to float", "unable to cast inOffsetValue to float")
						//MG_VECTOR("inOffsetValue as float = ", inOffsetValue_f)

					MFloatVector directionValue_f(directionValue);
						//MG_VECTOR("directionValue as float = ", directionValue_f)


				
					//unlike the NURBS intersect function which returns the intersection in the collision object's local space
					//the meshFn.closestIntersection method returns the intersection in world space so the code
					//will be a little different when we check for the resultCoordSpace value
					intersectResult = meshFn.closestIntersection(	inOffsetValue_f, directionValue_f, NULL, NULL, false, MSpace::kWorld, maxParam,
														false, &meshAccel, pntOfIntersection_f, &intersectionDistance, &hitFace, NULL, NULL, NULL,
														0.0001f, &stat);


					//McheckCode(stat, "Intersection MS::kSuccess", "intersection MS::kFail")





					if(intersectResult)
					{

						MPoint pntOfIntersection(pntOfIntersection_f);

						if(getNormal)
						{
							stat = meshFn.getPolygonNormal(hitFace, intersectionNormalValue, MSpace::kWorld);
								//McheckCode(stat, "Got the normal","Could not get the normal")
								
								//MG_VECTOR("normal WS = ", intersectionNormalValue)
						}
						





						//MESH
						if(resultCoordSpace == 1)//collision object space
						{
							pntOfIntersection *= collGeoMatrix.inverse(); 
								//MG_VECTOR("pointOfIntersection collision object space = " , pntOfIntersection)

							intersectionNormalValue *= collGeoMatrix.inverse();
								//MG_VECTOR("Normal in Object Space = " , intersectionNormalValue)
						}
						else if(resultCoordSpace == 2)//pointer object space
						{
							pntOfIntersection *= inMatrix.inverse(); 
								//MG_VECTOR("pointOfIntersection pointer object Space = " , pntOfIntersection)

							intersectionNormalValue *= inMatrix.inverse();
								//MG_VECTOR("Normal in Pointer Object Space = " , intersectionNormalValue)
						}



						intersectionDH.set(intersectResult);
						interPosDH.setMVector(  MVector(pntOfIntersection.x, pntOfIntersection.y, pntOfIntersection.z)  );






						stat = meshFn.getUVAtPoint( pntOfIntersection, uvPoint, MSpace::kObject, NULL, &hitFace); 
							//McheckCode(stat, "getUVAtPoint worked", "getUVAtPoint failed")

						







						//set the output UV coords
						interUVsDH.set( (double)uvPoint[0], (double)uvPoint[1] );

						interNormDH.setMVector( intersectionNormalValue );

						stat = intersectionDistanceLength.setValue((double)intersectionDistance);
							//McheckCode(stat, "Converted float to double to MDistance", "Falied to convert float to double to MDistance")

						distanceDH.set( intersectionDistanceLength );
				
					}


				}
				else
				{

					intersectionDH.set(intersectResult);
					interPosDH.setMVector(MVector(0.0,0.0,0.0) );
					interUVsDH.set( uParam, vParam );
					interNormDH.setMVector( intersectionNormalValue );
					distanceDH.set(intersectionDistanceLength);
				}

				//jump to the next element in the input connections array
				stat = inTransformADH.next();
					//MG_ERR(stat, "could not go to next array element")
					//MG_INFO(stat, "inTransformADH.next()")


			}


			stat = outDataADH.set(outDataBuilder);
				//MG_ERR(stat, "Could not set builder to ArrayDataHandle")
				//MG_INFO(stat, "Just set dataBuilder to arrayDataHandle")
			stat = outDataADH.setAllClean();
				//MG_ERR( stat, "Could not setAllClean()" )
				//MG_INFO( stat, "just MESH setAllClean() to arrayDataHanlde")

		}
	}

	return MS::kSuccess;
}


























































void* rayZapper::creator()
//
//	Description:
//		this method exists to give Maya a way to create new objects
//      of this type. 
//
//	Return Value:
//		a new object of this type
//
{
	return new rayZapper();
}






MStatus rayZapper::initialize()
//
//	Description:
//		This method is called to create and initialize all of the attributes
//      and attribute dependencies for this node type.  This is only called 
//		once when the node type is registered with Maya.
//
//	Return Values:
//		MS::kSuccess
//		MS::kFailure
//		
{
	MStatus				stat;

	
		

	// Input attributes

	MFnTypedAttribute   meshAttr;
	aInMesh = meshAttr.create( "collisionMesh", "cm", MFnData::kMesh, &stat );
	//MG_ERR( stat, "cannot create rayZapper inMesh attribute" );
		meshAttr.setStorable(true);
		meshAttr.setKeyable(false);
		meshAttr.setReadable(false);
		meshAttr.setWritable(true);
		meshAttr.setCached(false);
		meshAttr.setHidden(true);

	MFnTypedAttribute   surfaceAttr;
	aInSurface = surfaceAttr.create( "collisionSurface", "cs", MFnData::kNurbsSurface, &stat );
	//MG_ERR( stat, "cannot create rayZapper inSurface attribute" );
		surfaceAttr.setStorable(false);
		surfaceAttr.setKeyable(false);
		surfaceAttr.setReadable(false);
		surfaceAttr.setWritable(true);
		surfaceAttr.setCached(false);
		surfaceAttr.setHidden(true);

	MFnNumericAttribute		meshOrSurfaceAttr;
	aMeshOrNurbs = meshOrSurfaceAttr.create("meshOrNurbs", "mon", MFnNumericData::kBoolean, 0, &stat);
	//MG_ERR( stat, "cannot create rayZapper meshOrSurface attribute" );
		meshOrSurfaceAttr.setStorable(true);
		meshOrSurfaceAttr.setKeyable(false);
		meshOrSurfaceAttr.setReadable(true);
		meshOrSurfaceAttr.setWritable(true);
		meshOrSurfaceAttr.setCached(false);


	MFnNumericAttribute		worldOrLocalAttr;
	////////////////////aWorldLocalSpace = worldOrLocalAttr.create("worldOrLocal", "wol", MFnNumericData::kBoolean, 1, &stat);
	//////////////////////MG_ERR( stat, "cannot create rayZapper worldorLocal attribute" );
	////////////////////	worldOrLocalAttr.setStorable(true);
	////////////////////	worldOrLocalAttr.setKeyable(false);
	////////////////////	worldOrLocalAttr.setReadable(true);
	////////////////////	worldOrLocalAttr.setWritable(true);
	////////////////////	worldOrLocalAttr.setCached(false);









	MFnEnumAttribute resultCoordSpaceAttr;
	aResultCoordinateSpace = resultCoordSpaceAttr.create( "resultCoordinateSpace", "rcs", 0, &stat);
	//MG_ERR( stat, "cannot create result coord space attribute" );
                resultCoordSpaceAttr.addField("World Space", 0);
                resultCoordSpaceAttr.addField("Collision Object Space", 1);
                resultCoordSpaceAttr.addField("Pointer Object Space", 2);
				resultCoordSpaceAttr.setStorable(false);



	MFnEnumAttribute offsetCoordSpaceAttr;
	aOffsetCoordinateSpace = offsetCoordSpaceAttr.create( "offsetCoordinateSpace", "ocs", 0, &stat);
	//MG_ERR( stat, "cannot create offset coord space attribute" );
                offsetCoordSpaceAttr.addField("World Space", 0);
                offsetCoordSpaceAttr.addField("Collision Object Space", 1);
                offsetCoordSpaceAttr.addField("Pointer Object Space", 2);
				offsetCoordSpaceAttr.setStorable(false);














	MFnNumericAttribute		maintainOrientationAttr;
	aInheritOrientation = maintainOrientationAttr.create("inheritOrientation", "io", MFnNumericData::kBoolean, 1, &stat);
	//MG_ERR( stat, "cannot create rayZapper maintainOrientation attribute" );
		maintainOrientationAttr.setStorable(true);
		maintainOrientationAttr.setKeyable(false);
		maintainOrientationAttr.setReadable(true);
		maintainOrientationAttr.setWritable(true);
		maintainOrientationAttr.setCached(false);







////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	MFnNumericAttribute		getIntersectionAttr;
	aGetIntersection = getIntersectionAttr.create("getIntersection", "gin", MFnNumericData::kBoolean, 1, &stat);
	//MG_ERR( stat, "cannot create rayZapper getIntersection attribute" );
		getIntersectionAttr.setStorable(true);
		getIntersectionAttr.setKeyable(false);
		getIntersectionAttr.setReadable(true);
		getIntersectionAttr.setWritable(true);
		getIntersectionAttr.setCached(false);

	MFnNumericAttribute		getNormalAttr;
	aGetNormal = getNormalAttr.create("getNormal", "gn", MFnNumericData::kBoolean, 0, &stat);
	//MG_ERR( stat, "cannot create rayZapper getnormal attribute" );
		getNormalAttr.setStorable(true);
		getNormalAttr.setKeyable(false);
		getNormalAttr.setReadable(true);
		getNormalAttr.setWritable(true);
		getNormalAttr.setCached(false);

	MFnNumericAttribute		getUVCoordinatesAttr;
	aGetUVCoordinates = getUVCoordinatesAttr.create("getUVCoordinates", "guv", MFnNumericData::kBoolean, 0, &stat);
	//MG_ERR( stat, "cannot create rayZapper aGetUVCoordinates attribute" );
		getUVCoordinatesAttr.setStorable(true);
		getUVCoordinatesAttr.setKeyable(false);
		getUVCoordinatesAttr.setReadable(true);
		getUVCoordinatesAttr.setWritable(true);
		getUVCoordinatesAttr.setCached(false);

	MFnNumericAttribute		getDistanceAttr;
	aGetDistance = getDistanceAttr.create("getDistance", "gd", MFnNumericData::kBoolean, 0, &stat);
	//MG_ERR( stat, "cannot create rayZapper aGetDistance attribute" );
		getDistanceAttr.setStorable(true);
		getDistanceAttr.setKeyable(false);
		getDistanceAttr.setReadable(true);
		getDistanceAttr.setWritable(true);
		getDistanceAttr.setCached(false);












	
	MFnNumericAttribute maxDistanceAttr;
	aMaxDistance = maxDistanceAttr.create("maxDistance", "mxd", MFnNumericData::kFloat, 2.0, &stat);
		maxDistanceAttr.setStorable (true);
		maxDistanceAttr.setKeyable (true);
		maxDistanceAttr.setWritable (true);
		maxDistanceAttr.setMin (0.01);















	MFnMatrixAttribute		nurbsMatrixAttr;
	aNurbsMatrix = nurbsMatrixAttr.create("nurbsMatrix", "nm", MFnMatrixAttribute::kDouble, &stat);
	//MG_ERR( stat, "cannot create rayZapper nurbsMatrix attribute" );
		nurbsMatrixAttr.setKeyable(false);
		nurbsMatrixAttr.setReadable(false);
		nurbsMatrixAttr.setWritable(true);
		nurbsMatrixAttr.setHidden(true);


	MFnMatrixAttribute		meshMatrixAttr;
	aMeshMatrix = meshMatrixAttr.create("meshMatrix", "mm", MFnMatrixAttribute::kDouble, &stat);
	//MG_ERR( stat, "cannot create rayZapper meshMatrix attribute" );
		meshMatrixAttr.setKeyable(false);
		meshMatrixAttr.setReadable(false);
		meshMatrixAttr.setWritable(true);
		meshMatrixAttr.setHidden(true);


















	// CHILD ATTRIBUTES
	MFnNumericAttribute		onOffAttr;
	aOnOff = meshOrSurfaceAttr.create("onOff", "onf", MFnNumericData::kBoolean, 1, &stat);
	//MG_ERR( stat, "cannot create rayZapper meshOrSurface attribute" );
		onOffAttr.setStorable(true);
		onOffAttr.setKeyable(true);
		onOffAttr.setReadable(true);
		onOffAttr.setWritable(true);
		onOffAttr.setCached(false);


	MFnMatrixAttribute		inMatrixAttr;
	aInMatrix = inMatrixAttr.create("inMatrix", "inm", MFnMatrixAttribute::kDouble, &stat);
	//MG_ERR( stat, "cannot create rayZapper inMatrix attribute" );
		inMatrixAttr.setKeyable(false);
		inMatrixAttr.setReadable(false);
		inMatrixAttr.setWritable(true);
		inMatrixAttr.setHidden(true);


	MFnUnitAttribute inOffsetXAttr;
	aOffsetX = inOffsetXAttr.create( "offsetX", "ofx", MFnUnitAttribute::kDistance, 0.0, &stat );
	//MG_ERR( stat, "cannot create rayZapper inPositionX attribute" );
		inOffsetXAttr.setKeyable(true);
		inOffsetXAttr.setChannelBox(true);
		inOffsetXAttr.setHidden(false);
		inOffsetXAttr.setWritable( true );
	MFnUnitAttribute inOffsetYAttr;
	aOffsetY = inOffsetYAttr.create( "offsetY", "ofy", MFnUnitAttribute::kDistance, 0.0, &stat );
	//MG_ERR( stat, "cannot create rayZapper inPositionY attribute" );
		inOffsetYAttr.setKeyable(true);
		inOffsetYAttr.setChannelBox(true);
		inOffsetYAttr.setHidden(false);
		inOffsetYAttr.setWritable( true );
	MFnUnitAttribute inOffsetZAttr;
	aOffsetZ = inOffsetZAttr.create( "offsetZ", "ofz", MFnUnitAttribute::kDistance, 0.0, &stat );
	//MG_ERR( stat, "cannot create rayZapper inPositionZ attribute" );
		inOffsetZAttr.setKeyable(true);
		inOffsetZAttr.setChannelBox(true);
		inOffsetZAttr.setHidden(false);
		inOffsetZAttr.setWritable( true );
	MFnNumericAttribute inOffsetAttr;
	aOffset = inOffsetAttr.create( "offset", "of", aOffsetX, aOffsetY, aOffsetZ, &stat );
	//MG_ERR( stat, "cannot create rayZapper inPosition attribute" );
		inOffsetAttr.setDefault( 0.0, 0.0, 0.0 );
		inOffsetAttr.setKeyable(true);
		inOffsetAttr.setChannelBox(true);
		inOffsetAttr.setHidden(false);
		inOffsetAttr.setWritable( true );


	MFnNumericAttribute directionXAttr;
	aDirectionX = directionXAttr.create( "directionX", "dix", MFnNumericData::kDouble, 0.0, &stat );
	//MG_ERR( stat, "cannot create rayZapper directionX attribute" );
		directionXAttr.setKeyable(true);
		directionXAttr.setHidden(false);
		directionXAttr.setWritable( true );
	MFnNumericAttribute directionYAttr;
	aDirectionY = directionYAttr.create( "directionY", "diy", MFnNumericData::kDouble, -1.0, &stat );
	//MG_ERR( stat, "cannot create rayZapper directionY attribute" );
		directionYAttr.setKeyable(true);
		directionYAttr.setHidden(false);
		directionYAttr.setWritable( true );
	MFnNumericAttribute directionZAttr;
	aDirectionZ = directionZAttr.create( "directionZ", "diz", MFnNumericData::kDouble, 0.0, &stat );
	//MG_ERR( stat, "cannot create rayZapper directionZ attribute" );
		directionZAttr.setKeyable(true);
		directionZAttr.setHidden(false);
		directionZAttr.setWritable( true );
	MFnNumericAttribute directionAttr;
	aDirection = directionAttr.create( "direction", "di", aDirectionX, aDirectionY, aDirectionZ, &stat );
	//MG_ERR( stat, "cannot create rayZapper direction attribute" );
		directionAttr.setDefault( 0.0, -1.0, 0.0 );
		directionAttr.setKeyable( true );
		directionAttr.setHidden(false);
		directionAttr.setWritable( true );




	// INPUT compoundAttribute

	MFnCompoundAttribute inputTransCA;
	aInputTransform = inputTransCA.create("inTransform", "int", &stat);
	//MG_ERR(stat,"Could not create inputTransform")
	inputTransCA.setArray(true);
	inputTransCA.setHidden(false);

	inputTransCA.addChild(aOnOff);
	inputTransCA.addChild(aOffset);
	inputTransCA.addChild(aDirection);
	inputTransCA.addChild(aInMatrix);









	// Output attributes


	//MFnUnitAttribute intersectionPositionXAttr;
	MFnNumericAttribute intersectionPositionXAttr;
	aIntersectionPositionX = intersectionPositionXAttr.create( "intersectionPositionX", "inpx", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryPositionX attribute" );
	intersectionPositionXAttr.setKeyable( false );
	//intersectionPositionXAttr.setWritable(false);
	//intersectionPositionXAttr.setReadable( true );

	MFnNumericAttribute intersectionPositionYAttr;
	aIntersectionPositionY = intersectionPositionYAttr.create( "intersectionPositionY", "inpy", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryPositionY attribute" );
	intersectionPositionYAttr.setKeyable( false );
		//intersectionPositionYAttr.setWritable(false);
		//intersectionPositionYAttr.setReadable( true );

	MFnNumericAttribute intersectionPositionZAttr;
	aIntersectionPositionZ = intersectionPositionZAttr.create( "intersectionPositionZ", "inpz", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryPositionZ attribute" );
	intersectionPositionZAttr.setKeyable( false );
		//intersectionPositionZAttr.setWritable(false);
		//intersectionPositionZAttr.setReadable( true );

	MFnNumericAttribute intersectionPositionAttr;
	aIntersectionPosition = intersectionPositionAttr.create( "intersectionPosition", "inp", aIntersectionPositionX, aIntersectionPositionY, aIntersectionPositionZ, &stat );
		//MG_ERR( stat, "cannot create rayZapper entryPosition attribute" );
	intersectionPositionAttr.setDefault( 0.0, 0.0, 0.0 );
	intersectionPositionAttr.setKeyable( false );
		//intersectionPositionAttr.setWritable(false);
		//intersectionPositionAttr.setReadable( true );







	MFnNumericAttribute intersectionNormalXAttr;
	aIntersectionNormalX = intersectionNormalXAttr.create( "intersectionNormalX", "inx", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryNormalX attribute" );
	intersectionNormalXAttr.setKeyable(false);
		//intersectionNormalXAttr.setWritable(false);
		//intersectionNormalXAttr.setReadable( true );
	MFnNumericAttribute intersectionNormalYAttr;
	aIntersectionNormalY = intersectionNormalYAttr.create( "intersectionNormalY", "iny", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryNormalY attribute" );
	intersectionNormalYAttr.setKeyable(false);
		//intersectionNormalYAttr.setWritable(false);
		//intersectionNormalYAttr.setReadable( true );
	MFnNumericAttribute intersectionNormalZAttr;
	aIntersectionNormalZ = intersectionNormalZAttr.create( "intersectionNormalZ", "inz", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryNormalZ attribute" );
	intersectionNormalZAttr.setKeyable(false);
		//intersectionNormalZAttr.setWritable(false);
		//intersectionNormalZAttr.setReadable( true );
	MFnNumericAttribute intersectionNormalAttr;
	aIntersectionNormal = intersectionNormalAttr.create( "intersectionNormal", "inn", aIntersectionNormalX, aIntersectionNormalY, aIntersectionNormalZ, &stat );
		//MG_ERR( stat, "cannot create rayZapper entryNormal attribute" );
	intersectionNormalAttr.setDefault( 0.0, 0.0, 0.0 );
	intersectionNormalAttr.setKeyable(false);
		//intersectionNormalAttr.setWritable(false);
		//intersectionNormalAttr.setReadable( true );








	MFnNumericAttribute intersectionUVCoordsUAttr;
	aIntersectionUVCoordsU = intersectionUVCoordsUAttr.create( "intersectionUVCoordsU", "iuv", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryUVCoordsU attribute" );
	intersectionUVCoordsUAttr.setKeyable(false);
		//intersectionUVCoordsUAttr.setWritable(false);
		//intersectionUVCoordsUAttr.setReadable( true );
	MFnNumericAttribute intersectionUVCoordsVAttr;
	aIntersectionUVCoordsV = intersectionUVCoordsVAttr.create( "intersectionUVCoordsV", "inu", MFnNumericData::kDouble, 0.0, &stat  );
		//MG_ERR( stat, "cannot create rayZapper entryUVCoordsV attribute" );
	intersectionUVCoordsVAttr.setKeyable(false);
		//intersectionUVCoordsVAttr.setWritable(false);
		//intersectionUVCoordsVAttr.setReadable( true );
	MFnNumericAttribute intersectionUVCoordsAttr;
	aIntersectionUVCoords = intersectionUVCoordsAttr.create( "intersectionUVCoords", "inv", aIntersectionUVCoordsU, aIntersectionUVCoordsV);
	intersectionUVCoordsAttr.setDefault( 0.0, 0.0);
	intersectionUVCoordsAttr.setKeyable(false);
		//intersectionUVCoordsAttr.setWritable(false);
		//intersectionUVCoordsAttr.setReadable( true );



	

	MFnUnitAttribute    inDistanceAttr;
	aInDistance = inDistanceAttr.create( "intersectionDistance", "id", MFnUnitAttribute::kDistance, 0.0, &stat );
		//MG_ERR( stat, "cannot create rayZapper in distance attribute" );
	inDistanceAttr.setKeyable(false);
		//inDistanceAttr.setWritable(false);
		//inDistanceAttr.setReadable( true );








	MFnNumericAttribute entryIntersectAttr;
	aIntersection = entryIntersectAttr.create( "intersection", "itc", MFnNumericData::kBoolean, false, &stat );
		//MG_ERR( stat, "cannot create rayZapper entryIntersect attribute" );
	entryIntersectAttr.setKeyable(false);
		//entryIntersectAttr.setWritable(false);
	entryIntersectAttr.setReadable( true );








	// OUTPUT compound attribute

	MFnCompoundAttribute outputDataCA;
	aOutputData = outputDataCA.create("outputData", "od", &stat);
		//MG_ERR(stat, "could not create aOutputData attr")
	outputDataCA.setArray(true);
	outputDataCA.setReadable(true);
	outputDataCA.setKeyable(false);
	outputDataCA.setHidden(true);
	stat = outputDataCA.setDisconnectBehavior(MFnAttribute::kDelete);
	outputDataCA.setUsesArrayDataBuilder(true);
		//MG_ERR(stat, "Unable to setDisconnectBehavior.");

	outputDataCA.addChild(aIntersectionPosition);
	outputDataCA.addChild(aIntersectionNormal);
	outputDataCA.addChild(aIntersectionUVCoords);
	outputDataCA.addChild(aInDistance);
	outputDataCA.addChild(aIntersection);
























	// Add the attributes we have created to the node

	stat = addAttribute( aInMesh );
		//MG_ERR( stat, "addAttribute for inMesh failed" );

	stat = addAttribute( aInSurface );
		//MG_ERR( stat, "addAttribute for inSurface failed" );

	stat = addAttribute( aMeshOrNurbs );
		//MG_ERR( stat, "addAttribute for meshOrSurface failed" );



	stat = addAttribute( aInheritOrientation);
		//MG_ERR( stat, "addAttribute for aInheritOrientation failed" );

	stat = addAttribute( aGetIntersection);
		//MG_ERR( stat, "addAttribute for aGetIntersection failed" );

	stat = addAttribute( aGetNormal);
		//MG_ERR( stat, "addAttribute for aGetNormal failed" );

	stat = addAttribute( aGetUVCoordinates);
		//MG_ERR( stat, "addAttribute for aGetUVCoordinates failed" );

	stat = addAttribute( aGetDistance);
		//MG_ERR( stat, "addAttribute for aGetDistance failed" );

	stat = addAttribute( aMaxDistance );
		//MG_ERR( stat, "addAttribute for aMaxDistance failed" );







	stat = addAttribute( aNurbsMatrix );
		//MG_ERR( stat, "addAttribute for aNurbsMatrix failed" );
	stat = addAttribute( aMeshMatrix );
		//MG_ERR( stat, "addAttribute for aMeshMatrix failed" );








	stat = addAttribute( aInputTransform );
		//MG_ERR( stat, "addAttribute for aInputTransform failed" );


	stat = addAttribute( aResultCoordinateSpace );
		//MG_ERR( stat, "addAttribute for aResultCoordinateSpace failed" );

	stat = addAttribute( aOffsetCoordinateSpace );
		//MG_ERR( stat, "addAttribute for aOffsetCoordinateSpace  failed" );

	stat = addAttribute( aOutputData );
		//MG_ERR( stat, "addAttribute for aOutputData failed" );


	
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////      ////////      //////              //////    ///////////////////////////////////////////////////////////
//////////////////    /  //////  /    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    //  ////  //    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    ///  //  ///    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    ////    ////    //////          //////////    ///////////////////////////////////////////////////////////
//////////////////    /////  /////    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    ////////////    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    ////////////    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    ////////////    //////    ////////////////    ///////////////////////////////////////////////////////////
//////////////////    ////////////    //////              //////              /////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	MString cmdString;

	 const char* connectPointerToRayZapperChar =  "proc string da_connectPointerToRayZapper(){"\
	" scriptEditorInfo -writeHistory false; "\
	" string $sel[] = `ls -sl`; "\
	" int $numSels = size($sel); "\
	" string $rayZapperNode = $sel[$numSels - 1]; "\
	" if( `nodeType $rayZapperNode` != \"rayZapper\") "\
	" { error( \"Please select transform nodes followed by one rayZapperNode.\"); } "\
	" int $i = 0; "\
	" for($i; $i < ($numSels - 1); $i++) "\
	" { "\
	" string $type = `nodeType $sel[$i]`; "\
	" if($type != \"transform\") "\
	" {  "\
	" error( \"Please select any number of transform nodes followed by one rayZapperNode.\");  "\
	" } "\
	" } "\
	" int $indeces[] = `getAttr -mi ($rayZapperNode + \".inTransform\")`; "\
	" int $size = size($indeces); "\
	" int $nextAvailableIndex = $size; "\
	" if(   ($size != 0) && ($indeces[$size - 1] > $size)   ) "\
	" { "\
	" $nextAvailableIndex = $indeces[$size - 1] + 1; "\
	" } "\
	" $i = 0; "\
	" for($i; $i < ($numSels - 1); $i++) "\
	" { "\
	" connectAttr ($sel[$i] + \".worldMatrix\") ($rayZapperNode + \".inTransform[\" + $nextAvailableIndex +\"].inMatrix\"); "\
	" getAttr ($rayZapperNode + \".inTransform[\" + $nextAvailableIndex + \"].offset\"); "\
	" getAttr ($rayZapperNode + \".inTransform[\" + $nextAvailableIndex + \"].direction\"); "\
	" getAttr ($rayZapperNode + \".outputData[\" + $nextAvailableIndex + \"].intersectionPosition\");"\
	" $nextAvailableIndex++;  "\
	" } "\
	"string $newSel[] = `ls -sl`;"\
    "select -r $rayZapperNode;"\
    "addOffsetAttrsToRayZapper;"\
   	"select -cl;"\
    "select $newSel;"\
	" scriptEditorInfo -writeHistory true; "\
	" return (\"Connected pointer(s) to \" + $rayZapperNode);"
	" }";

	//convert mel script to MString
	cmdString = connectPointerToRayZapperChar;

	//execute mel commands
	MGlobal::executeCommand(cmdString, false, true);









	const char* connectCollisionObjToRayZapperCHAR = "proc string da_connectCollisionObjToRayZapper() "\
	"{ "\
	"string $sel[] = `ls -sl`; "\
	"int $numSels = size($sel); "\
	"if($numSels != 2) "\
	"{ "\
	"error(\"Please select a NURBS surface or polygon mesh followed by a rayZapper node.\"); "\
	"} "\
	"string $rayZapperNode = $sel[1]; "\
	"if( `nodeType $rayZapperNode` != \"rayZapper\") "\
	"{ error( \"Please select a NURBS surface or polygon mesh followed by a rayZapper node.\"); } "\
	"string $type = `nodeType $sel[0]`; "\
	"if( ($type != \"transform\") && ($type != \"nurbsSurface\") && ( $type != \"mesh\") ) "\
	"{  "\
	"error( \"Please select a NURBS surface or polygon mesh followed by a rayZapper node.\" );  "\
	"} "\
	"string $childShape[]; "\
	"string $parent[]; "\
	"string $childType; "\
	"string $parentType; "\
	"if($type == \"transform\") "\
	"{ "\
	"$childShape = `listRelatives -s $sel[0]`; "\
	"$childType = `objectType $childShape[0]`; "\
	"if( ($childType != \"nurbsSurface\") &&  ( $childType != \"mesh\")  )  "\
	"{  "\
	"error( \"Please select a NURBS surface or polygon mesh followed by a rayZapper node.\" );  "\
	"} "\
	"else if( $childType == \"nurbsSurface\") "\
	"{ "\
	"connectAttr -f ($childShape[0] + \".worldSpace[0]\")  ( $rayZapperNode + \".collisionSurface\"); "\
	"connectAttr -f ($sel[0] + \".worldMatrix[0]\")  ( $rayZapperNode + \".nurbsMatrix\"); "\
	"return (\"Connected \" + $sel[0] + \" to \" + $rayZapperNode); "\
	"} "\
	"else if( $childType == \"mesh\") "\
	"{ "\
	"connectAttr -f ($childShape[0] + \".worldMesh[0]\") ( $rayZapperNode + \".collisionMesh\"); "\
	"connectAttr -f ($sel[0] + \".worldMatrix[0]\")  ( $rayZapperNode + \".meshMatrix\"); "\
	"return (\"Connected \" + $sel[0] + \" to \" + $rayZapperNode); "\
	"} "\
	"} "\
	"else if($type == \"nurbsSurface\") "\
	"{ "\
	"$parent = `listRelatives -p $sel[0]`; "\
	"connectAttr -f ($sel[0] + \".worldSpace[0]\")  ( $rayZapperNode + \".collisionSurface\"); "\
	"connectAttr -f ($parent[0] + \".worldMatrix[0]\")  ( $rayZapperNode + \".nurbsMatrix\"); "\
	"return (\"Connected \" + $parent[0] + \" to \" + $rayZapperNode);   "\
	"} "\
	"else if($type == \"mesh\") "\
	"{ "\
	"$parent = `listRelatives -p $sel[0]`; "\
	"connectAttr -f ($sel[0] + \".worldMesh[0]\") ( $rayZapperNode + \".collisionMesh\"); "\
	"connectAttr -f ($parent[0] + \".worldMatrix[0]\")  ( $rayZapperNode + \".meshMatrix\"); "\
	"return (\"Connected \" + $parent[0] + \" to \" + $rayZapperNode);       "\
	"} "\
	"else "\
	"{ "\
	"error(\"Nothing connected.\"); "\
	"} "\
	"} ";



	//convert mel script to MString
	cmdString = connectCollisionObjToRayZapperCHAR;


	//execute mel commands
	MGlobal::executeCommand(cmdString, false, true);











	const char* addRayZapperOffsetAttrsCHAR = "proc string da_addRayZapperOffsetAttrs() "\
	"{ "\
	"string $nodeName;   "\
	"string $sel[] = `ls -sl`;   "\
	"int $size = size($sel);   "\
	"if($size)   "\
	"{   "\
	"    $nodeName = $sel[0];   "\
	"}   "\
	"else   "\
	"{   "\
	"    error \"please select a rayZapper node.\";    "\
	"}   "\
	"string $nodeType = `nodeType $nodeName`;   "\
	"if($nodeType != \"rayZapper\")   "\
	"{   "\
	"    error \"Node selected is not of type rayZapper\";   "\
	"}  "\
	"string $gaitKeeper[] = `listConnections -t gaitKeeper $nodeName`;"\
	"if(`size($gaitKeeper)`)"\
	"{"\
    "string $result = `confirmDialog -title \"Confirm\" -message (\"Adding the custom offset attributes will overide any connections\\nfrom \" + $gaitKeeper[0] + \" node.  Do you want to continue?\")"\
    "-button \"Yes\" -button \"No\" -defaultButton \"Yes\""\
    "-cancelButton \"No\" -dismissString \"No\"`;"\
    "if ($result == \"No\")"\
    "{"\
    "return(\"Operation terminated.\");"\
    "}"\
    "}"\
	"int $indeces[] = `getAttr -mi ($nodeName + \".inTransform\")`;    "\
	"$size = size($indeces);   "\
	"int $i;   "\
	"for($i = 0; $i < $size; $i++)   "\
	"{   "\
	"    if(!`attributeExists (\"offset\" + $indeces[$i]) $nodeName`)   "\
	"     {    "\
	"         addAttr -ln (\"offset\" + $indeces[$i])  -at double3  $nodeName;   "\
	"         addAttr -ln (\"offset\" + $indeces[$i] + \"X\")  -at double -p (\"offset\" + $indeces[$i])  $nodeName;   "\
	"         addAttr -ln (\"offset\" + $indeces[$i] + \"Y\")  -at double -p (\"offset\" + $indeces[$i])  $nodeName;   "\
	"         addAttr -ln (\"offset\" + $indeces[$i] + \"Z\")  -at double -p (\"offset\" + $indeces[$i])  $nodeName;   "\
	"         connectAttr -f ($nodeName + \".offset\" + $indeces[$i]) ($nodeName + \".inTransform[\" + $indeces[$i] + \"].offset\");   "\
	"         addAttr -ln (\"direction\" + $indeces[$i])  -at double3  $nodeName;   "\
	"         addAttr -ln (\"direction\" + $indeces[$i] + \"X\")  -at double -dv  0 -p (\"direction\" + $indeces[$i])  $nodeName;   "\
	"         addAttr -ln (\"direction\" + $indeces[$i] + \"Y\")  -at double -dv -1 -p (\"direction\" + $indeces[$i])  $nodeName;   "\
	"         addAttr -ln (\"direction\" + $indeces[$i] + \"Z\")  -at double -dv 0 -p (\"direction\" + $indeces[$i])  $nodeName;   "\
	"         connectAttr -f ($nodeName + \".direction\" + $indeces[$i]) ($nodeName + \".inTransform[\" + $indeces[$i] + \"].direction\");   "\
	"     }   "\
	"     else "\
	"     { "\
	"         if(   !`isConnected ($nodeName + \".offset\" + $indeces[$i]) ($nodeName + \".inTransform[\" + $indeces[$i] + \"].offset\")`   ) "\
	"         { "\
	"			  setAttr -l false ($nodeName + \".offset\" + $indeces[$i]);"\
	"             connectAttr -f ($nodeName + \".offset\" + $indeces[$i]) ($nodeName + \".inTransform[\" + $indeces[$i] + \"].offset\"); "\
	"         }   "\
	"         if(   !`isConnected ($nodeName + \".direction\" + $indeces[$i]) ($nodeName + \".inTransform[\" + $indeces[$i] + \"].direction\")`   ) "\
	"         { "\
	"             connectAttr -f ($nodeName + \".direction\" + $indeces[$i]) ($nodeName + \".inTransform[\" + $indeces[$i] + \"].direction\");  "\
	"         }  "\
	"     } "\
	"     }  "\
	"     return (\"Offset attributes added to \" + $nodeName); "\
	"} ";

	//convert mel script to MString
	cmdString = addRayZapperOffsetAttrsCHAR;


	//execute mel commands
	MGlobal::executeCommand(cmdString, false, true);












	




















	// Set up a dependency between the input and the output.  This will cause
	// the output to be marked dirty when the input changes.  The output will
	// then be recomputed the next time the value of the output is requested.
	//
	try {
		attributeAffects( aGetIntersection, aIntersectionPosition );
		attributeAffects( aGetIntersection, aIntersectionNormal );
		attributeAffects( aGetIntersection, aIntersectionUVCoords );
		attributeAffects( aGetIntersection, aInDistance );
		attributeAffects( aGetIntersection, aIntersection );
		attributeAffects( aGetIntersection, aOutputData );

		attributeAffects( aGetNormal, aIntersectionPosition );
		attributeAffects( aGetNormal, aIntersectionNormal );
		attributeAffects( aGetNormal, aIntersectionUVCoords );
		attributeAffects( aGetNormal, aInDistance );
		attributeAffects( aGetNormal, aIntersection );
		attributeAffects( aGetNormal, aOutputData );

		attributeAffects( aGetUVCoordinates, aIntersectionPosition );
		attributeAffects( aGetUVCoordinates, aIntersectionNormal );
		attributeAffects( aGetUVCoordinates, aIntersectionUVCoords );
		attributeAffects( aGetUVCoordinates, aInDistance );
		attributeAffects( aGetUVCoordinates, aIntersection );
		attributeAffects( aGetUVCoordinates, aOutputData );

		attributeAffects( aGetDistance, aIntersectionPosition );
		attributeAffects( aGetDistance, aIntersectionNormal );
		attributeAffects( aGetDistance, aIntersectionUVCoords );
		attributeAffects( aGetDistance, aInDistance );
		attributeAffects( aGetDistance, aIntersection );
		attributeAffects( aGetDistance, aOutputData );








		attributeAffects( aMaxDistance, aIntersectionPosition );
		attributeAffects( aMaxDistance, aIntersectionNormal );
		attributeAffects( aMaxDistance, aIntersectionUVCoords );
		attributeAffects( aMaxDistance, aInDistance );
		attributeAffects( aMaxDistance, aIntersection );
		attributeAffects( aMaxDistance, aOutputData );




		attributeAffects(aInputTransform, aIntersectionPosition);
		attributeAffects(aInputTransform, aIntersectionNormal);
		attributeAffects(aInputTransform, aIntersectionUVCoords);
		attributeAffects(aInputTransform, aInDistance);
		attributeAffects(aInputTransform, aIntersection);
		attributeAffects(aInputTransform, aOutputData);





		attributeAffects( aInMatrix, aIntersectionPosition );
		attributeAffects( aInMatrix, aIntersectionNormal );
		attributeAffects( aInMatrix, aIntersectionUVCoords );
		attributeAffects( aInMatrix, aInDistance );
		attributeAffects( aInMatrix, aIntersection );
		attributeAffects( aInMatrix, aOutputData );






		attributeAffects( aOffset, aIntersectionPosition );
		attributeAffects( aOffset, aIntersectionNormal );
		attributeAffects( aOffset, aIntersectionUVCoords );
		attributeAffects( aOffset, aInDistance );
		attributeAffects( aOffset, aIntersection );
		attributeAffects( aOffset, aOutputData );


		


		attributeAffects( aDirection, aIntersectionPosition );
		attributeAffects( aDirection, aIntersectionNormal );
		attributeAffects( aDirection, aIntersectionUVCoords );
		attributeAffects( aDirection, aInDistance );
		attributeAffects( aDirection, aIntersection );
		attributeAffects( aDirection, aOutputData );


		


		attributeAffects( aNurbsMatrix, aIntersectionPosition );
		attributeAffects( aNurbsMatrix, aIntersectionNormal );
		attributeAffects( aNurbsMatrix, aIntersectionUVCoords );
		attributeAffects( aNurbsMatrix, aInDistance );
		attributeAffects( aNurbsMatrix, aIntersection );
		attributeAffects( aNurbsMatrix, aOutputData );


		attributeAffects( aMeshMatrix, aIntersectionPosition );
		attributeAffects( aMeshMatrix, aIntersectionNormal );
		attributeAffects( aMeshMatrix, aIntersectionUVCoords );
		attributeAffects( aMeshMatrix, aInDistance );
		attributeAffects( aMeshMatrix, aIntersection );
		attributeAffects( aMeshMatrix, aOutputData );






		attributeAffects( aResultCoordinateSpace, aIntersectionPosition );
		attributeAffects( aResultCoordinateSpace, aIntersectionNormal );
		attributeAffects( aResultCoordinateSpace, aIntersectionUVCoords );
		attributeAffects( aResultCoordinateSpace, aInDistance );
		attributeAffects( aResultCoordinateSpace, aIntersection );
		attributeAffects( aResultCoordinateSpace, aOutputData );

		attributeAffects( aOffsetCoordinateSpace, aIntersectionPosition );
		attributeAffects( aOffsetCoordinateSpace, aIntersectionNormal );
		attributeAffects( aOffsetCoordinateSpace, aIntersectionUVCoords );
		attributeAffects( aOffsetCoordinateSpace, aInDistance );
		attributeAffects( aOffsetCoordinateSpace, aIntersection );
		attributeAffects( aOffsetCoordinateSpace, aOutputData );








		attributeAffects( aInMesh, aOutputData );
		attributeAffects( aInSurface, aOutputData );
		attributeAffects( aMeshOrNurbs, aOutputData );
		attributeAffects( aInheritOrientation, aOutputData );
		attributeAffects( aMaxDistance, aOutputData );

	} 
	catch ( MStatus stat ) 
	{
		//MG_ERR( stat, "attribute affects failed" );
	}
	return MS::kSuccess;

}



void rayZapper::postConstructor( )
{
	setMPSafe(false);
}