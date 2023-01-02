// Copyright (C) 1997-2001 Alias|Wavefront,
// a division of Silicon Graphics Limited.
//
// The information in this file is provided for the exclusive use of the
// licensees of Alias|Wavefront.  Such users have the right to use, modify,
// and incorporate this code into other products for purposes authorized
// by the Alias|Wavefront license agreement, without fee.
//
// ALIAS|WAVEFRONT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
// INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
// EVENT SHALL ALIAS|WAVEFRONT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
// CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
// USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
// OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
// PERFORMANCE OF THIS SOFTWARE.
//
// DISCLAIMER: THIS PLUGIN IS PROVIDED AS IS.  IT IS NOT SUPPORTED BY
//            ALIAS|WAVEFRONT, SO PLEASE USE AND MODIFY AT YOUR OWN RISK.
//
// PLUGIN NAME: closestPointOnMesh v1.0
// FILE: closestNormalUVAndFace.h
// DESCRIPTION: -Utility function declaration, used by both the command and node.
//              -Please see readme.txt for full details.
// AUTHOR: QT
// REFERENCES: -This plugin's concept is based off of the "closestPointOnSurface" node.
//             -The MEL script AEclosestPointOnSurfaceTemplate.mel was referred to for
//              the AE template MEL script that accompanies the closestPointOnMesh node.
// LAST UPDATED: Oct. 13th, 2001.
// COMPILED AND TESTED ON: Maya 4.0 on Windows




#ifndef _closestNormalUVAndFace
#define _closestNormalUVAndFace


// MAYA HEADER FILES:

#include <maya/MObject.h>
#include <maya/MDagPath.h>
#include <maya/MFnMesh.h>
#include <maya/MItMeshPolygon.h>
#include <maya/MPoint.h>
#include <maya/MVector.h>




// FUNCTION DECLARATION:

void closestNormalUVAndFace(MDagPath meshDagPath, 
							MPoint inPosition, 
							MPoint &position, 
							MVector &normal, 
							double &parameterU, 
							double &parameterV, 
							int &closestFaceIndex, 
							MObject theMesh=MObject::kNullObj);



#endif

