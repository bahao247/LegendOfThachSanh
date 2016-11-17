/*
---------------------------------------------------------------------------------------
   ___                   _															   
  /___\__ _ _ __ ___    /_\  _ __  _ __
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \											   
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \
\___/ \__, |_|  \___| \_/ \_/ .__/| .___/
      |___/                 |_|   |_|                                 
      Ogre 1.9.x Application for VC10
      https://plus.google.com/+bahaonguyen/
---------------------------------------------------------------------------------------
| Filename   : GeomUtils.h                                                           
---------------------------------------------------------------------------------------
| Written in : C++                                                     
---------------------------------------------------------------------------------------
| Compiler   : Microsoft Visual C++ 2010 Service Pack 1.                               
---------------------------------------------------------------------------------------
| Engine     : Ogre3D; ....................................                                                                     
---------------------------------------------------------------------------------------
| Written by : Ba Hao Nguyen – Sports editors, IT.                                
---------------------------------------------------------------------------------------
| Note       : ....................................                                
---------------------------------------------------------------------------------------
| Create File: 21:34:42 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:34:42 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef _GEOMUTILS_H
#define _GEOMUTILS_H

#include "OgreString.h"
#include "OgreVertexIndexData.h"

class GeomUtils
{
public:
	// Create a sphere Mesh with a given name, radius, number of rings and number of segments
	static void createSphere(const Ogre::String& strName
		, float radius
		, int nRings, int nSegments
		, bool bNormals
		, bool bTexCoords
		);


	// Fill up a fresh copy of VertexData and IndexData with a sphere's coords given the number of rings and the number of segments
	static void createSphere(Ogre::VertexData*& vertexData, Ogre::IndexData*& indexData
		, float radius
		, int nRings, int nSegments
		, bool bNormals
		, bool bTexCoords);

	// Create a cone Mesh with a given name, radius and number of vertices in base
	// Created cone will have its head at 0,0,0, and will 'expand to' positive y
	static void createCone(const Ogre::String& strName
		, float radius
		, float height
		, int nVerticesInBase);

	// Fill up a fresh copy of VertexData and IndexData with a cone's coords given the radius and number of vertices in base
	static void createCone(Ogre::VertexData*& vertexData, Ogre::IndexData*& indexData
		, float radius
		, float height
		, int nVerticesInBase);


	// Fill up a fresh copy of VertexData with a normalized quad
	static void createQuad(Ogre::VertexData*& vertexData);


};


#endif
