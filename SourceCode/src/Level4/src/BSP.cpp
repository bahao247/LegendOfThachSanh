#include "GamePlugin.h"
#include "BSP.h"

using namespace Ogre;
using namespace OgreBites;

#ifndef OGRE_STATIC_LIB

GamePlugin* sp;
Game* s;


extern "C" _OgreGameExport void dllStartPlugin()
{
	s = new Game_BSP;
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
