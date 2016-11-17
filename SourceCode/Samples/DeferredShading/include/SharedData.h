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
| Filename   : SharedData.h                                                           
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
| Create File: 21:34:12 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:34:12 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef __SHAREDDATA_H
#define __SHAREDDATA_H

#include <Ogre.h>
#include "DeferredShading.h"

class SharedData : public Ogre::Singleton<SharedData> {

public:

	SharedData()
		: iRoot(0),
		  iCamera(0),
		  iWindow(0),
		  mMLAnimState(0),
		  iMainLight(0)
	{
		iActivate = false;
	}

		~SharedData() {}

		// shared data across the application
		Ogre::Real iLastFrameTime;
		Ogre::Root *iRoot;
		Ogre::Camera *iCamera;
		Ogre::RenderWindow *iWindow;

		DeferredShadingSystem *iSystem;
		bool iActivate;
		bool iGlobalActivate;

		// Animation state for light swarm
		Ogre::AnimationState* mMLAnimState;

		Ogre::Light *iMainLight;

		Ogre::vector<Ogre::Node*>::type mLightNodes;

};

#endif
