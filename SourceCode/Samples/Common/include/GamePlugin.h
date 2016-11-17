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
| Filename   : GamePlugin.h                                                           
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
| Create File: 21:30:21 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:30:21 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#ifndef __GamePlugin_H__
#define __GamePlugin_H__

#include "OgrePlugin.h"
#include "Game.h"

// Export macro to export the Game's main dll functions.
#if defined( OGRE_STATIC_LIB )
#  define _OgreGameExport
#  define _OgreGameClassExport
#else
#  if (OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WINRT) && !defined(__MINGW32__)
#    define _OgreGameExport __declspec(dllexport)
#    define _OgreGameClassExport
#  elif defined ( OGRE_GCC_VISIBILITY )
#   define _OgreGameExport  __attribute__ ((visibility("default")))
#   define _OgreGameClassExport  __attribute__ ((visibility("default")))
#  else
#	define _OgreGameExport
#	define _OgreGameClassExport
#  endif
#endif


namespace OgreBites
{
	/*=============================================================================
	| Utility class used to hold a set of Games in an OGRE plugin.
	=============================================================================*/
	class _OgreGameClassExport GamePlugin : public Ogre::Plugin
    {
    public:

		GamePlugin(const Ogre::String& name)
		: mName(name)
		{
		}

		const Ogre::String& getName() const
		{
			return mName;
		}
        
		void install() {}
		void uninstall() {}
		void initialise() {}
		void shutdown() {}

		/*-----------------------------------------------------------------------------
		| Adds a Game to the queue.
		-----------------------------------------------------------------------------*/
		void addGame(Game* s)
		{
			mGames.insert(s);
		}

		/*-----------------------------------------------------------------------------
		| Retrieves the queue of Games.
		-----------------------------------------------------------------------------*/
		const GameSet& getGames()
		{
			return mGames;
		}

	protected:

		Ogre::String mName;
		GameSet mGames;
    };
}

#endif
