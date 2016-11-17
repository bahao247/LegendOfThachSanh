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
| Filename   : ParticleFX.cpp                                                           
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
| Create File: 21:32:08 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:32:08 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#include "GamePlugin.h"
#include "ParticleFX.h"

using namespace Ogre;
using namespace OgreBites;

#ifndef OGRE_STATIC_LIB

GamePlugin* sp;
Game* s;

extern "C" _OgreGameExport void dllStartPlugin()
{
	s = new Game_ParticleFX;
	sp = OGRE_NEW GamePlugin(s->getInfo()["Title"] + " Game");
	sp->addGame(s);
	Root::getSingleton().installPlugin(sp);
}

extern "C" _OgreGameExport void dllStopPlugin()
{
	Root::getSingleton().uninstallPlugin(sp); 
	OGRE_DELETE sp;
	delete s;
}
#endif
