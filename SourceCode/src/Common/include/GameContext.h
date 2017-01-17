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
| Filename   : GameContext.h                                                           
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
| Create File: 21:30:10 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:30:10 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#ifndef __GameContext_H__
#define __GameContext_H__

#include "OgreBuildSettings.h"
#include "OgreLogManager.h"
#include "OgrePlugin.h"
#include "OgreFileSystemLayer.h"
#include "OgreOverlaySystem.h"

// Static plugins declaration section
// Note that every entry in here adds an extra header / library dependency
#ifdef OGRE_STATIC_LIB
#  ifdef OGRE_BUILD_RENDERSYSTEM_GL
#    define OGRE_STATIC_GL
#  endif
#  ifdef OGRE_BUILD_RENDERSYSTEM_GLES
#    define OGRE_STATIC_GLES
#    undef INCLUDE_RTSHADER_SYSTEM
#  endif
#  ifdef OGRE_BUILD_RENDERSYSTEM_GLES2
#    undef OGRE_STATIC_GLES
#    define INCLUDE_RTSHADER_SYSTEM
#    define OGRE_STATIC_GLES2
#  endif
#  if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || OGRE_PLATFORM == OGRE_PLATFORM_WINRT
#    ifdef OGRE_BUILD_RENDERSYSTEM_D3D9
#		define OGRE_STATIC_Direct3D9
#    endif
// dx11 will only work on vista and above, so be careful about statically linking
#    ifdef OGRE_BUILD_RENDERSYSTEM_D3D11
#      define OGRE_STATIC_Direct3D11
#    endif
#  endif

#  ifdef OGRE_BUILD_PLUGIN_BSP
#  define OGRE_STATIC_BSPSceneManager
#  endif
#  ifdef OGRE_BUILD_PLUGIN_PFX
#  define OGRE_STATIC_ParticleFX
#  endif
#  ifdef OGRE_BUILD_PLUGIN_CG
#  define OGRE_STATIC_CgProgramManager
#  endif

#  ifdef OGRE_USE_PCZ
#    ifdef OGRE_BUILD_PLUGIN_PCZ
#    define OGRE_STATIC_PCZSceneManager
#    define OGRE_STATIC_OctreeZone
#    endif
#  else
#    ifdef OGRE_BUILD_PLUGIN_OCTREE
#    define OGRE_STATIC_OctreeSceneManager
#  endif
#     endif
#  include "OgreStaticPluginLoader.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include "macUtils.h"
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#   ifdef __OBJC__
#       import <UIKit/UIKit.h>
#   endif
#endif
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include <android_native_app_glue.h>
#endif

#include "Game.h"

#include "OIS.h"

namespace OgreBites
{
	/*=============================================================================
	| Base class responsible for setting up a common context for Games.
	| May be subclassed for specific Game types (not specific Games).
	| Allows one Game to run at a time, while maintaining a Game queue.
	=============================================================================*/
	class GameContext :
		public Ogre::FrameListener,
		public Ogre::WindowEventListener,
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
		public OIS::KeyListener,
		public OIS::MouseListener
#else
        public OIS::MultiTouchListener
#endif
	{
	public:

		GameContext()
		{
			mFSLayer = OGRE_NEW_T(Ogre::FileSystemLayer, Ogre::MEMCATEGORY_GENERAL)(OGRE_VERSION_NAME);
			mRoot = 0;
			mWindow = 0;
			mCurrentGame = 0;
			mOverlaySystem = 0;
			mGamePaused = false;
			mFirstRun = true;
			mLastRun = false;
			mLastGame = 0;
			mInputMgr = 0;
		}

		virtual ~GameContext() 
		{
			OGRE_DELETE_T(mFSLayer, FileSystemLayer, Ogre::MEMCATEGORY_GENERAL);
		}

		virtual Ogre::RenderWindow* getRenderWindow()
		{
			return mWindow;
		}

		virtual Game* getCurrentGame()
		{
			return mCurrentGame;
		}

		/*-----------------------------------------------------------------------------
		| Quits the current Game and starts a new one.
		-----------------------------------------------------------------------------*/
		virtual void runGame(Game* s)
		{
#if OGRE_PROFILING
            Ogre::Profiler* prof = Ogre::Profiler::getSingletonPtr();
            if (prof)
                prof->setEnabled(false);
#endif

			if (mCurrentGame)
			{
				mCurrentGame->_shutdown();    // quit current Game
				mGamePaused = false;          // don't pause the next Game
			}

			mWindow->removeAllViewports();                  // wipe viewports

			if (s)
			{
				// retrieve Game's required plugins and currently installed plugins
				Ogre::Root::PluginInstanceList ip = mRoot->getInstalledPlugins();
				Ogre::StringVector rp = s->getRequiredPlugins();

				for (Ogre::StringVector::iterator j = rp.begin(); j != rp.end(); j++)
				{
					bool found = false;
					// try to find the required plugin in the current installed plugins
					for (Ogre::Root::PluginInstanceList::iterator k = ip.begin(); k != ip.end(); k++)
					{
						if ((*k)->getName() == *j)
						{
							found = true;
							break;
						}
					}
					if (!found)  // throw an exception if a plugin is not found
					{
						Ogre::String desc = "Game requires plugin: " + *j;
						Ogre::String src = "GameContext::runGame";
						OGRE_EXCEPT(Ogre::Exception::ERR_NOT_IMPLEMENTED, desc, src);
					}
				}

				// throw an exception if Games requires the use of another renderer
				Ogre::String rrs = s->getRequiredRenderSystem();
				if (!rrs.empty() && rrs != mRoot->getRenderSystem()->getName())
				{
					Ogre::String desc = "Game only runs with renderer: " + rrs;
					Ogre::String src = "GameContext::runGame";
					OGRE_EXCEPT(Ogre::Exception::ERR_INVALID_STATE, desc, src);
				}

				// test system capabilities against Game requirements
				s->testCapabilities(mRoot->getRenderSystem()->getCapabilities());

				s->_setup(mWindow, mInputContext, mFSLayer, mOverlaySystem);   // start new Game
			}
#if OGRE_PROFILING
            if (prof)
                prof->setEnabled(true);
#endif

			mCurrentGame = s;
		}

		/*-----------------------------------------------------------------------------
		| This function initializes the render system and resources.
		-----------------------------------------------------------------------------*/
		virtual void initApp( Game* initialGame = 0 )
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            createRoot();

			if (!oneTimeConfig()) return;

            if (!mFirstRun) mRoot->setRenderSystem(mRoot->getRenderSystemByName(mNextRenderer));

            setup();

            if (!mFirstRun) recoverLastGame();
            else if (initialGame) runGame(initialGame);

            mRoot->saveConfig();

			Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();

			// Clear event times
			Ogre::Root::getSingleton().clearEventTimes();
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
			createRoot();

            setup();

            if (!mFirstRun) recoverLastGame();
            else if (initialGame) runGame(initialGame);

            //mRoot->saveConfig();

			Ogre::Root::getSingleton().getRenderSystem()->_initRenderTargets();

			// Clear event times
			Ogre::Root::getSingleton().clearEventTimes();

#else
			createRoot();
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
            mNextRenderer = mRoot->getAvailableRenderers()[0]->getName();
#else
			if (!oneTimeConfig()) return;
#endif

			// if the context was reconfigured, set requested renderer
			if (!mFirstRun) mRoot->setRenderSystem(mRoot->getRenderSystemByName(mNextRenderer));

			setup();

			// restore the last Game if there was one or, if not, start initial Game
			if (!mFirstRun) recoverLastGame();
			else if (initialGame) runGame(initialGame);
#endif
		}


		/*-----------------------------------------------------------------------------
		| This function closes down the application - saves the configuration then 
		| shutdowns.
		-----------------------------------------------------------------------------*/
		virtual void closeApp()
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
			shutdown();
#else
			mRoot->saveConfig();
			shutdown();
			if (mRoot)
			{
				OGRE_DELETE mOverlaySystem;
				OGRE_DELETE mRoot;
			}
#ifdef OGRE_STATIC_LIB
			mStaticPluginLoader.unload();
#endif
#endif
		}

		/*-----------------------------------------------------------------------------
		| This function encapsulates the entire lifetime of the context.
		-----------------------------------------------------------------------------*/
#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
		virtual void go(Game* initialGame = 0)
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || ((OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__)
            createRoot();

			if (!oneTimeConfig()) return;

            if (!mFirstRun) mRoot->setRenderSystem(mRoot->getRenderSystemByName(mNextRenderer));

            mLastRun = true;  // assume this is our last run

            setup();

            if (!mFirstRun) recoverLastGame();
            else if (initialGame) runGame(initialGame);

            mRoot->saveConfig();
#else
			while (!mLastRun)
			{
				mLastRun = true;  // assume this is our last run

				initApp(initialGame);
                loadStartUpGame();
        
                if (mRoot->getRenderSystem() != NULL)
                {
				    mRoot->startRendering();    // start the render loop
                }

				closeApp();

				mFirstRun = false;
			}
#endif
		}
#endif

        virtual void loadStartUpGame() {}
        
		virtual bool isCurrentGamePaused()
		{
			if (mCurrentGame) return mGamePaused;
			return false;
		}

		virtual void pauseCurrentGame()
		{
			if (mCurrentGame && !mGamePaused)
			{
				mGamePaused = true;
				mCurrentGame->paused();
			}
		}

		virtual void unpauseCurrentGame()
		{
			if (mCurrentGame && mGamePaused)
			{
				mGamePaused = false;
				mCurrentGame->unpaused();
			}
		}
			
		/*-----------------------------------------------------------------------------
		| Processes frame started events.
		-----------------------------------------------------------------------------*/
		virtual bool frameStarted(const Ogre::FrameEvent& evt)
		{
			captureInputDevices();      // capture input

			// manually call Game callback to ensure correct order
			return (mCurrentGame && !mGamePaused) ? mCurrentGame->frameStarted(evt) : true;
		}
			
		/*-----------------------------------------------------------------------------
		| Processes rendering queued events.
		-----------------------------------------------------------------------------*/
		virtual bool frameRenderingQueued(const Ogre::FrameEvent& evt)
		{
			// manually call Game callback to ensure correct order
			return (mCurrentGame && !mGamePaused) ? mCurrentGame->frameRenderingQueued(evt) : true;
		}
			
		/*-----------------------------------------------------------------------------
		| Processes frame ended events.
		-----------------------------------------------------------------------------*/
		virtual bool frameEnded(const Ogre::FrameEvent& evt)
		{
			// manually call Game callback to ensure correct order
			if (mCurrentGame && !mGamePaused && !mCurrentGame->frameEnded(evt)) return false;
			// quit if window was closed
			if (mWindow->isClosed()) return false;
			// go into idle mode if current Game has ended
			if (mCurrentGame && mCurrentGame->isDone()) runGame(0);

			return true;
		}

		/*-----------------------------------------------------------------------------
		| Processes window size change event. Adjusts mouse's region to match that
		| of the window. You could also override this method to prevent resizing.
		-----------------------------------------------------------------------------*/
		virtual void windowResized(Ogre::RenderWindow* rw)
		{
			// manually call Game callback to ensure correct order
			if (mCurrentGame && !mGamePaused) mCurrentGame->windowResized(rw);

#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
			if(mInputContext.mMouse)
			{
				const OIS::MouseState& ms = mInputContext.mMouse->getMouseState();
				ms.width = rw->getWidth();
				ms.height = rw->getHeight();
			}
#endif
		}

		// window event callbacks which manually call their respective Game callbacks to ensure correct order

		virtual void windowMoved(Ogre::RenderWindow* rw)
		{
			if (mCurrentGame && !mGamePaused) mCurrentGame->windowMoved(rw);
		}

		virtual bool windowClosing(Ogre::RenderWindow* rw)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->windowClosing(rw);
			return true;
		}

		virtual void windowClosed(Ogre::RenderWindow* rw)
		{
			if (mCurrentGame && !mGamePaused) mCurrentGame->windowClosed(rw);
		}

		virtual void windowFocusChange(Ogre::RenderWindow* rw)
		{
			if (mCurrentGame && !mGamePaused) mCurrentGame->windowFocusChange(rw);
		}

		// keyboard and mouse callbacks which manually call their respective Game callbacks to ensure correct order

		virtual bool keyPressed(const OIS::KeyEvent& evt)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->keyPressed(evt);
			return true;
		}

		virtual bool keyReleased(const OIS::KeyEvent& evt)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->keyReleased(evt);
			return true;
		}

#if (OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0)
    #if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        void transformInputState(OIS::MultiTouchState &state)
    #else
        void transformInputState(OIS::MouseState &state)
    #endif
        {
            int w = mWindow->getViewport(0)->getActualWidth();
            int h = mWindow->getViewport(0)->getActualHeight();
            int absX = state.X.abs;
            int absY = state.Y.abs;
            int relX = state.X.rel;
            int relY = state.Y.rel;

            switch (mWindow->getViewport(0)->getOrientationMode())
            {
            case Ogre::OR_DEGREE_0:
                break;
            case Ogre::OR_DEGREE_90:
                state.X.abs = w - absY;
                state.Y.abs = absX;
                state.X.rel = -relY;
                state.Y.rel = relX;
                break;
            case Ogre::OR_DEGREE_180:
                state.X.abs = w - absX;
                state.Y.abs = h - absY;
                state.X.rel = -relX;
                state.Y.rel = -relY;
                break;
            case Ogre::OR_DEGREE_270:
                state.X.abs = absY;
                state.Y.abs = h - absX;
                state.X.rel = relY;
                state.Y.rel = -relX;
                break;
            }
        }
#elif (OGRE_NO_VIEWPORT_ORIENTATIONMODE == 1) && (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
    // Variation based upon device orientation for use with a view controller
    void transformInputState(OIS::MultiTouchState &state)
    {
        int w = mWindow->getViewport(0)->getActualWidth();
        int h = mWindow->getViewport(0)->getActualHeight();
        int absX = state.X.abs;
        int absY = state.Y.abs;
        int relX = state.X.rel;
        int relY = state.Y.rel;

        UIInterfaceOrientation interfaceOrientation = [UIApplication sharedApplication].statusBarOrientation;
        switch (interfaceOrientation)
        {
            case UIInterfaceOrientationPortrait:
                break;
            case UIInterfaceOrientationLandscapeLeft:
                state.X.abs = w - absY;
                state.Y.abs = absX;
                state.X.rel = -relY;
                state.Y.rel = relX;
                break;
            case UIInterfaceOrientationPortraitUpsideDown:
                state.X.abs = w - absX;
                state.Y.abs = h - absY;
                state.X.rel = -relX;
                state.Y.rel = -relY;
                break;
            case UIInterfaceOrientationLandscapeRight:
                state.X.abs = absY;
                state.Y.abs = h - absX;
                state.X.rel = relY;
                state.Y.rel = -relX;
                break;
        }
    }
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchMoved(const OIS::MultiTouchEvent& evt)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->touchMoved(evt);
			return true;
		}
#else
		virtual bool mouseMoved(const OIS::MouseEvent& evt)
		{
            if (mCurrentGame && !mGamePaused) return mCurrentGame->mouseMoved(evt);
			return true;
		}
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchPressed(const OIS::MultiTouchEvent& evt)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->touchPressed(evt);
			return true;
		}
#else
		virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->mousePressed(evt, id);
			return true;
		}
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchReleased(const OIS::MultiTouchEvent& evt)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->touchReleased(evt);
			return true;
		}
#else
		virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
		{
			if (mCurrentGame && !mGamePaused) return mCurrentGame->mouseReleased(evt, id);
			return true;
		}
#endif

        bool isFirstRun() { return mFirstRun; }
        void setFirstRun(bool flag) { mFirstRun = flag; }
        bool isLastRun() { return mLastRun; }
        void setLastRun(bool flag) { mLastRun = flag; }
	protected:

        /*-----------------------------------------------------------------------------
         | Sets up the context after configuration.
         -----------------------------------------------------------------------------*/
		virtual void setup()
		{
			mWindow = createWindow();
			setupInput();
			locateResources();
			loadResources();
            
			Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
            
			// adds context as listener to process context-level (above the Game level) events
			mRoot->addFrameListener(this);
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
#endif
		}
        
		/*-----------------------------------------------------------------------------
		| Creates the OGRE root.
		-----------------------------------------------------------------------------*/
		virtual void createRoot()
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
			mRoot = Ogre::Root::getSingletonPtr();
#else
            Ogre::String pluginsPath = Ogre::StringUtil::BLANK;
#   ifndef OGRE_STATIC_LIB
            pluginsPath = mFSLayer->getConfigFilePath("plugins.cfg");
#   endif
			mRoot = OGRE_NEW Ogre::Root(pluginsPath, mFSLayer->getWritablePath("ogre.cfg"), 
				mFSLayer->getWritablePath("ogre.log"));
            
#   ifdef OGRE_STATIC_LIB
            mStaticPluginLoader.load();
#   endif
#endif
			mOverlaySystem = OGRE_NEW Ogre::OverlaySystem();
		}

		/*-----------------------------------------------------------------------------
		| Configures the startup settings for OGRE. I use the config dialog here,
		| but you can also restore from a config file. Note that this only happens
		| when you start the context, and not when you reset it.
		-----------------------------------------------------------------------------*/
		virtual bool oneTimeConfig()
		{
			return mRoot->showConfigDialog();
			// return mRoot->restoreConfig();
		}

		/*-----------------------------------------------------------------------------
		| Creates the render window to be used for this context. I use an auto-created
		| window here, but you can also create an external window if you wish.
		| Just don't forget to initialise the root.
		-----------------------------------------------------------------------------*/
		virtual Ogre::RenderWindow* createWindow()
		{
			return mRoot->initialise(true);
		}

		/*-----------------------------------------------------------------------------
		| Sets up OIS input.
		-----------------------------------------------------------------------------*/
		virtual void setupInput(bool nograb = false)
		{
#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
			OIS::ParamList pl;
			size_t winHandle = 0;
			std::ostringstream winHandleStr;

			mWindow->getCustomAttribute("WINDOW", &winHandle);
			winHandleStr << winHandle;

			pl.insert(std::make_pair("WINDOW", winHandleStr.str()));
            if (nograb)
            {
                pl.insert(std::make_pair("x11_keyboard_grab", "false"));
                pl.insert(std::make_pair("x11_mouse_grab", "false"));
                pl.insert(std::make_pair("w32_mouse", "DISCL_FOREGROUND"));
                pl.insert(std::make_pair("w32_mouse", "DISCL_NONEXCLUSIVE"));
                pl.insert(std::make_pair("w32_keyboard", "DISCL_FOREGROUND"));
                pl.insert(std::make_pair("w32_keyboard", "DISCL_NONEXCLUSIVE"));
            }

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
            // Pass the view to OIS so the contentScalingFactor can be used
			std::ostringstream viewHandleStr;
			size_t viewHandle = 0;
			mWindow->getCustomAttribute("VIEW", &viewHandle);
			viewHandleStr << viewHandle;
            pl.insert(std::make_pair("VIEW", viewHandleStr.str()));
#endif

			mInputMgr = OIS::InputManager::createInputSystem(pl);

			createInputDevices();      // create the specific input devices
#endif

			// attach input devices
			windowResized(mWindow);    // do an initial adjustment of mouse area
#if (OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS) && (OGRE_PLATFORM != OGRE_PLATFORM_ANDROID)
			if(mInputContext.mKeyboard)
				mInputContext.mKeyboard->setEventCallback(this);
			if(mInputContext.mMouse)
				mInputContext.mMouse->setEventCallback(this);
#else
			if(mInputContext.mMultiTouch)
				mInputContext.mMultiTouch->setEventCallback(this);
#endif
		}

		/*-----------------------------------------------------------------------------
		| Creates the individual input devices. I only create a keyboard and mouse
		| here because they are the most common, but you can override this method
		| for other modes and devices.
		-----------------------------------------------------------------------------*/
		virtual void createInputDevices()
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
			mInputContext.mMultiTouch = static_cast<OIS::MultiTouch*>(mInputMgr->createInputObject(OIS::OISMultiTouch, true));
			mInputContext.mAccelerometer = static_cast<OIS::JoyStick*>(mInputMgr->createInputObject(OIS::OISJoyStick, true));
#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
			// nothing to do
#elif OGRE_PLATFORM == OGRE_PLATFORM_WINRT
			// mInputMgr is NULL and input devices are already passed to us, therefore nothing to do
			assert(mInputContext.mKeyboard);
			assert(mInputContext.mMouse);
#else
			mInputContext.mKeyboard = static_cast<OIS::Keyboard*>(mInputMgr->createInputObject(OIS::OISKeyboard, true));
			mInputContext.mMouse = static_cast<OIS::Mouse*>(mInputMgr->createInputObject(OIS::OISMouse, true));
#endif
		}

		/*-----------------------------------------------------------------------------
		| Finds context-wide resource groups. I load paths from a config file here,
		| but you can choose your resource locations however you want.
		-----------------------------------------------------------------------------*/
		virtual void locateResources()
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Essential.zip", "EmbeddedZip", "Essential");
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation("Popular.zip", "EmbeddedZip", "Popular");
#else
			// load resource paths from config file
			Ogre::ConfigFile cf;
#	if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            cf.load(openAPKFile(mFSLayer->getConfigFilePath("resources.cfg")));
#	else
			cf.load(mFSLayer->getConfigFilePath("resources.cfg"));
#	endif
			Ogre::ConfigFile::SectionIterator seci = cf.getSectionIterator();
			Ogre::String sec, type, arch;

			// go through all specified resource groups
			while (seci.hasMoreElements())
			{
				sec = seci.peekNextKey();
				Ogre::ConfigFile::SettingsMultiMap* settings = seci.getNext();
				Ogre::ConfigFile::SettingsMultiMap::iterator i;

				// go through all resource paths
				for (i = settings->begin(); i != settings->end(); i++)
				{
					type = i->first;
					arch = i->second;

					#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                    // OS X does not set the working directory relative to the app,
                    // In order to make things portable on OS X we need to provide
                    // the loading with it's own bundle path location
					if (!Ogre::StringUtil::startsWith(arch, "/", false)) // only adjust relative dirs
						arch = Ogre::String(Ogre::macBundlePath() + "/" + arch);
					#endif
					Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch, type, sec);
				}
			}


        const Ogre::ResourceGroupManager::LocationList genLocs = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList("General");
        arch = genLocs.front()->archive->getName();
#	if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID
#		if OGRE_PLATFORM == OGRE_PLATFORM_APPLE
            arch = Ogre::macBundlePath() + "/Contents/Resources/Media";
#		elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            arch = Ogre::macBundlePath() + "/Media";
#		else       
            arch = Ogre::StringUtil::replaceAll(arch, "Media/../../Tests/Media", "");  
            arch = Ogre::StringUtil::replaceAll(arch, "media/../../Tests/Media", ""); 
#		endif
            type = "FileSystem";
            sec = "Popular";

            // Add locations for supported shader languages
            if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
            {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSLES", type, sec);
            }
            else if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
            {
                if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
                {
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSL150", type, sec);
                }
                else
                {
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSL", type, sec);
                }

                if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl400"))
                {
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/GLSL400", type, sec);
                }
            }
            else if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("hlsl"))
            {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/HLSL", type, sec);
            }
#		ifdef OGRE_BUILD_PLUGIN_CG
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/materials/programs/Cg", type, sec);
#		endif

#		ifdef INCLUDE_RTSHADER_SYSTEM
            if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
            {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/GLSLES", type, sec);
            }
            else if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl"))
            {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/GLSL", type, sec);
                if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
                {
                    Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/GLSL150", type, sec);
                }
            }
            else if(Ogre::GpuProgramManager::getSingleton().isSyntaxSupported("hlsl"))
            {
                Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/HLSL", type, sec);
            }
#			ifdef OGRE_BUILD_PLUGIN_CG
            Ogre::ResourceGroupManager::getSingleton().addResourceLocation(arch + "/RTShaderLib/Cg", type, sec);
#			endif
#		endif /* INCLUDE_RTSHADER_SYSTEM */
#	endif /* OGRE_PLATFORM != OGRE_PLATFORM_ANDROID */
#endif /* OGRE_PLATFORM == OGRE_PLATFORM_NACL */
		}

		/*-----------------------------------------------------------------------------
		| Loads context-wide resource groups. I chose here to simply initialise all
		| groups, but you can fully load specific ones if you wish.
		-----------------------------------------------------------------------------*/
		virtual void loadResources()
		{
			Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
		}

		/*-----------------------------------------------------------------------------
		| Reconfigures the context. Attempts to preserve the current Game state.
		-----------------------------------------------------------------------------*/
		virtual void reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options)
		{
			// save current Game state
			mLastGame = mCurrentGame;
			if (mCurrentGame) mCurrentGame->saveState(mLastGameState);

			mNextRenderer = renderer;
			Ogre::RenderSystem* rs = mRoot->getRenderSystemByName(renderer);

			// set all given render system options
			for (Ogre::NameValuePairList::iterator it = options.begin(); it != options.end(); it++)
			{
				rs->setConfigOption(it->first, it->second);
                
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                // Change the viewport orientation on the fly if requested
                if(it->first == "Orientation")
                {
                    if (it->second == "Landscape Left")
                        mWindow->getViewport(0)->setOrientationMode(Ogre::OR_LANDSCAPELEFT, true);
                    else if (it->second == "Landscape Right")
                        mWindow->getViewport(0)->setOrientationMode(Ogre::OR_LANDSCAPERIGHT, true);
                    else if (it->second == "Portrait")
                        mWindow->getViewport(0)->setOrientationMode(Ogre::OR_PORTRAIT, true);
                }
#endif
			}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            // Need to save the config on iOS to make sure that changes are kept on disk
            mRoot->saveConfig();
#endif
			mLastRun = false;             // we want to go again with the new settings
			mRoot->queueEndRendering();   // break from render loop
		}

		/*-----------------------------------------------------------------------------
		| Recovers the last Game after a reset. You can override in the case that
		| the last Game is destroyed in the process of resetting, and you have to
		| recover it through another means.
		-----------------------------------------------------------------------------*/
		virtual void recoverLastGame()
		{
			runGame(mLastGame);
			mLastGame->restoreState(mLastGameState);
			mLastGame = 0;
			mLastGameState.clear();
		}

		/*-----------------------------------------------------------------------------
		| Cleans up and shuts down the context.
		-----------------------------------------------------------------------------*/
		virtual void shutdown()
		{
			if (mCurrentGame)
			{
				mCurrentGame->_shutdown();
				mCurrentGame = 0;
			}

			// remove window event listener before shutting down OIS
			Ogre::WindowEventUtilities::removeWindowEventListener(mWindow, this);

			shutdownInput();
		}

		/*-----------------------------------------------------------------------------
		| Destroys OIS input devices and the input manager.
		-----------------------------------------------------------------------------*/
		virtual void shutdownInput()
		{
			// detach input devices
			windowResized(mWindow);    // do an initial adjustment of mouse area
			if(mInputContext.mKeyboard)
				mInputContext.mKeyboard->setEventCallback(NULL);
			if(mInputContext.mMouse)
				mInputContext.mMouse->setEventCallback(NULL);
#if OIS_WITH_MULTITOUCH
			if(mInputContext.mMultiTouch)
				mInputContext.mMultiTouch->setEventCallback(NULL);
#endif
			if(mInputContext.mAccelerometer)
                mInputContext.mAccelerometer->setEventCallback(NULL);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
			if (mInputMgr)
			{
				if(mInputContext.mKeyboard)
					mInputMgr->destroyInputObject(mInputContext.mKeyboard);
				if(mInputContext.mMouse)
					mInputMgr->destroyInputObject(mInputContext.mMouse);
#if OIS_WITH_MULTITOUCH
				if(mInputContext.mMultiTouch)
					mInputMgr->destroyInputObject(mInputContext.mMultiTouch);
#endif
				if(mInputContext.mAccelerometer)
	                mInputMgr->destroyInputObject(mInputContext.mAccelerometer);

				OIS::InputManager::destroyInputSystem(mInputMgr);
				mInputMgr = 0;
			}
#endif
		}

		/*-----------------------------------------------------------------------------
		| Captures input device states.
		-----------------------------------------------------------------------------*/
		virtual void captureInputDevices()
		{
			mInputContext.capture();
		}
        
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
        Ogre::DataStreamPtr openAPKFile(const Ogre::String& fileName)
        {
            Ogre::DataStreamPtr stream;
            AAsset* asset = AAssetManager_open(mAssetMgr, fileName.c_str(), AASSET_MODE_BUFFER);
            if(asset)
            {
                off_t length = AAsset_getLength(asset);
                void* membuf = OGRE_MALLOC(length, Ogre::MEMCATEGORY_GENERAL);
                memcpy(membuf, AAsset_getBuffer(asset), length);
                AAsset_close(asset);
                
                stream = Ogre::DataStreamPtr(new Ogre::MemoryDataStream(membuf, length, true, true));
            }
            return stream;
        }
        AAssetManager* mAssetMgr;       // Android asset manager to access files inside apk
#endif

        Ogre::FileSystemLayer* mFSLayer; // File system abstraction layer
		Ogre::Root* mRoot;              // OGRE root
		OIS::InputManager* mInputMgr;   // OIS input manager
		InputContext mInputContext;		// all OIS devices are here
		Ogre::OverlaySystem* mOverlaySystem;  // Overlay system
#ifdef OGRE_STATIC_LIB
        Ogre::StaticPluginLoader mStaticPluginLoader;
#endif
		Game* mCurrentGame;         // currently running Game
		bool mGamePaused;             // whether current Game is paused
		bool mFirstRun;                 // whether or not this is the first run
		bool mLastRun;                  // whether or not this is the final run
		Ogre::String mNextRenderer;     // name of renderer used for next run
		Game* mLastGame;            // last Game run before reconfiguration
		Ogre::NameValuePairList mLastGameState;     // state of last Game
    public:
        Ogre::RenderWindow* mWindow;    // render window
	};
}

#endif
