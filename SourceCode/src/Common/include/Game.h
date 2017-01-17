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
| Filename   : Game.h                                                           
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
| Create File: 21:29:37 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:29:37 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#ifndef __Game_H__
#define __Game_H__

#include "Ogre.h"
#include "OgreOverlaySystem.h"
#include <iostream>

#include "InputContext.h"
#include "OgreFileSystemLayer.h"

#ifdef INCLUDE_RTSHADER_SYSTEM
#	include "OgreRTShaderSystem.h"
#endif //INCLUDE_RTSHADER_SYSTEM

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#	include "macUtils.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_NACL && !defined(INCLUDE_RTSHADER_SYSTEM)
#   define INCLUDE_RTSHADER_SYSTEM
#include "OgreShaderGenerator.h"
#endif


namespace OgreBites
{
	/*=============================================================================
	| Base class responsible for everything specific to one Game.
	| Designed to be subclassed for each Game.
	=============================================================================*/
	class Game : public Ogre::GeneralAllocatedObject
    {
    public:
		/*=============================================================================
		| Utility comparison structure for sorting Games using GameSet.
		=============================================================================*/
		struct Comparer
		{
			bool operator() (Game* a, Game* b)
			{
				Ogre::NameValuePairList::iterator aTitle = a->getInfo().find("Title");
				Ogre::NameValuePairList::iterator bTitle = b->getInfo().find("Title");
				
				if (aTitle != a->getInfo().end() && bTitle != b->getInfo().end())
					return aTitle->second.compare(bTitle->second) < 0;
				else return false;
			}
		};

#ifdef INCLUDE_RTSHADER_SYSTEM
		Game() : mShaderGenerator(0)
#else
		Game()
#endif
        {
			mRoot = Ogre::Root::getSingletonPtr();
			mWindow = 0;
			mSceneMgr = 0;
			mDone = true;
			mResourcesLoaded = false;
			mContentSetup = false;

			mFSLayer = 0;
        }

		virtual ~Game() {}

		/*-----------------------------------------------------------------------------
		| Retrieves custom Game info.
		-----------------------------------------------------------------------------*/
		Ogre::NameValuePairList& getInfo()
		{
			return mInfo;
		}

		/*-----------------------------------------------------------------------------
		| Tests to see if target machine meets any special requirements of
		| this Game. Signal a failure by throwing an exception.
		-----------------------------------------------------------------------------*/
		virtual void testCapabilities(const Ogre::RenderSystemCapabilities* caps) {}

		/*-----------------------------------------------------------------------------
		| If this Game requires a specific render system to run, this method
		| will be used to return its name.
		-----------------------------------------------------------------------------*/
		virtual Ogre::String getRequiredRenderSystem()
		{
			return "";
		}

		/*-----------------------------------------------------------------------------
		| If this Game requires specific plugins to run, this method will be
		| used to return their names.
		-----------------------------------------------------------------------------*/
		virtual Ogre::StringVector getRequiredPlugins()
		{
			return Ogre::StringVector();
		}

		Ogre::SceneManager* getSceneManager()
		{
			return mSceneMgr;
		}

		bool isDone()
		{
			return mDone;
		}

		/*-----------------------------------------------------------------------------
		| Sets up a Game. Used by the GameContext class. Do not call directly.
		-----------------------------------------------------------------------------*/
		virtual void _setup(Ogre::RenderWindow* window, InputContext inputContext, Ogre::FileSystemLayer* fsLayer, Ogre::OverlaySystem* overlaySys)
		{
			// assign mRoot here in case Root was initialised after the Game's constructor ran.
			mRoot = Ogre::Root::getSingletonPtr();
			mOverlaySystem = overlaySys;
			mWindow = window;
			mInputContext = inputContext;
			mFSLayer = fsLayer;

			locateResources();
			createSceneManager();
			setupView();

			loadResources();
			mResourcesLoaded = true;
			setupContent();
			mContentSetup = true;

			mDone = false;
		}

		/*-----------------------------------------------------------------------------
		| Shuts down a Game. Used by the GameContext class. Do not call directly.
		-----------------------------------------------------------------------------*/
		virtual void _shutdown()

		{
			if (mContentSetup)
                cleanupContent();
			if (mSceneMgr)
                mSceneMgr->clearScene();
			mContentSetup = false;

			if (mResourcesLoaded)
                unloadResources();
			mResourcesLoaded = false;
			if (mSceneMgr) 
			{
#ifdef INCLUDE_RTSHADER_SYSTEM
				mShaderGenerator->removeSceneManager(mSceneMgr);
#endif
				mSceneMgr->removeRenderQueueListener(mOverlaySystem);
				mRoot->destroySceneManager(mSceneMgr);				
			}
			mSceneMgr = 0;

			mDone = true;
		}

		/*-----------------------------------------------------------------------------
		| Actions to perform when the context stops sending frame listener events
		| and input device events to this Game.
		-----------------------------------------------------------------------------*/
		virtual void paused() {}

		/*-----------------------------------------------------------------------------
		| Actions to perform when the context continues sending frame listener
		| events and input device events to this Game.
		-----------------------------------------------------------------------------*/
		virtual void unpaused() {}

		/*-----------------------------------------------------------------------------
		| Saves the Game state. Optional. Used during reconfiguration.
		-----------------------------------------------------------------------------*/
		virtual void saveState(Ogre::NameValuePairList& state) {}

		/*-----------------------------------------------------------------------------
		| Restores the Game state. Optional. Used during reconfiguration.
		-----------------------------------------------------------------------------*/
		virtual void restoreState(Ogre::NameValuePairList& state) {}

		// callback interface copied from various listeners to be used by GameContext

		virtual bool frameStarted(const Ogre::FrameEvent& evt) { return true; }
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt) { return true; }
		virtual bool frameEnded(const Ogre::FrameEvent& evt) { return true; }
		virtual void windowMoved(Ogre::RenderWindow* rw) {}
		virtual void windowResized(Ogre::RenderWindow* rw) {}
		virtual bool windowClosing(Ogre::RenderWindow* rw) { return true; }
		virtual void windowClosed(Ogre::RenderWindow* rw) {}
		virtual void windowFocusChange(Ogre::RenderWindow* rw) {}
		virtual bool keyPressed(const OIS::KeyEvent& evt) { return true; }
		virtual bool keyReleased(const OIS::KeyEvent& evt) { return true; }
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchMoved(const OIS::MultiTouchEvent& evt) { return true; }
		virtual bool touchPressed(const OIS::MultiTouchEvent& evt) { return true; }
		virtual bool touchReleased(const OIS::MultiTouchEvent& evt) { return true; }
#else
		virtual bool mouseMoved(const OIS::MouseEvent& evt) { return true; }
		virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id) { return true; }
		virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id) { return true; }
#endif

    protected:

		/*-----------------------------------------------------------------------------
		| Finds Game-specific resources. No such effort is made for most Games,
		| but this is useful for special Games with large, exclusive resources.
		-----------------------------------------------------------------------------*/
		virtual void locateResources() {}

		/*-----------------------------------------------------------------------------
		| Loads Game-specific resources. No such effort is made for most Games,
		| but this is useful for special Games with large, exclusive resources.
		-----------------------------------------------------------------------------*/
		virtual void loadResources() {}

		/*-----------------------------------------------------------------------------
		| Creates a scene manager for the Game. A generic one is the default,
		| but many Games require a special kind of scene manager.
		-----------------------------------------------------------------------------*/
		virtual void createSceneManager()
		{
			mSceneMgr = Ogre::Root::getSingleton().createSceneManager(Ogre::ST_GENERIC);
#ifdef INCLUDE_RTSHADER_SYSTEM
			mShaderGenerator->addSceneManager(mSceneMgr);
#endif
            if(mOverlaySystem)
                mSceneMgr->addRenderQueueListener(mOverlaySystem);
		}

		/*-----------------------------------------------------------------------------
		| Sets up viewport layout and camera.
		-----------------------------------------------------------------------------*/
		virtual void setupView() {}

		/*-----------------------------------------------------------------------------
		| Sets up the scene (and anything else you want for the Game).
		-----------------------------------------------------------------------------*/
		virtual void setupContent() {}

		/*-----------------------------------------------------------------------------
		| Cleans up the scene (and anything else you used).
		-----------------------------------------------------------------------------*/
		virtual void cleanupContent() {}

		/*-----------------------------------------------------------------------------
		| Unloads Game-specific resources. My method here is simple and good
		| enough for most small Games, but your needs may vary.
		-----------------------------------------------------------------------------*/
		virtual void unloadResources()
		{
			Ogre::ResourceGroupManager::ResourceManagerIterator resMgrs =
			Ogre::ResourceGroupManager::getSingleton().getResourceManagerIterator();

			while (resMgrs.hasMoreElements())
			{
				resMgrs.getNext()->unloadUnreferencedResources();
			}
		}	

		Ogre::Root* mRoot;                // OGRE root object
		Ogre::OverlaySystem* mOverlaySystem; // OverlaySystem
		Ogre::RenderWindow* mWindow;      // context render window
		InputContext mInputContext;
		Ogre::FileSystemLayer* mFSLayer; 		  // file system abstraction layer
		Ogre::SceneManager* mSceneMgr;    // scene manager for this Game
		Ogre::NameValuePairList mInfo;    // custom Game info
		bool mDone;                       // flag to mark the end of the Game
		bool mResourcesLoaded;    // whether or not resources have been loaded
		bool mContentSetup;       // whether or not scene was created
#ifdef INCLUDE_RTSHADER_SYSTEM
		Ogre::RTShader::ShaderGenerator*			mShaderGenerator;			// The Shader generator instance.
    public:
		void setShaderGenerator(Ogre::RTShader::ShaderGenerator* shaderGenerator) 
		{ 
			mShaderGenerator = shaderGenerator;
		};
#endif
    };

	typedef std::set<Game*, Game::Comparer> GameSet;
}

#endif
