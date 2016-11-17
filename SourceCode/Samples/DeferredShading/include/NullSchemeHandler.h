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
| Filename   : NullSchemeHandler.h                                                           
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
| Create File: 21:34:19 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:34:19 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef _NULLSCHEMEHANDLER_H
#define _NULLSCHEMEHANDLER_H

#include <OgreMaterialManager.h>

/** Class for skipping materials which do not have the scheme defined
 */
class NullSchemeHandler : public Ogre::MaterialManager::Listener
{
public:
	/** @copydoc MaterialManager::Listener::handleSchemeNotFound */
	virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, 
		const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, 
		const Ogre::Renderable* rend)
	{
		//Creating a technique so the handler only gets called once per material
		Ogre::Technique* emptyTech = originalMaterial->createTechnique();
		emptyTech->removeAllPasses();
		emptyTech->setSchemeName(schemeName);
		return emptyTech;
	}
};

#endif
