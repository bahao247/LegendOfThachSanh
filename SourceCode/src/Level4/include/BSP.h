#ifndef __BSP_H__
#define __BSP_H__
//////////////////////////////////////////////////////////////////////////
#pragma region [Include lib]
#include "SdkGame.h"
#include "OgreFileSystemLayer.h"
#include "SinbadCharacterControllerCommon2.h"
#include "SdkTableCommon.h"
#include "MyFileFactoryCommon.h"
#pragma endregion [Include lib]
//////////////////////////////////////////////////////////////////////////
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include "macUtils.h"
#endif

using namespace Ogre;
using namespace OgreBites;

class _OgreGameClassExport Game_BSP : public SdkGame
{
public:

	Game_BSP()
	{
		mInfo["Title"] = "Level 4";
		mInfo["Description"] = "Tich tich tinh tang.";
		mInfo["Thumbnail"] = "thumb_level4.png";
		mInfo["Category"] = "Easy";
		mInfo["Help"] = "Use the WASD keys to move Sinbad, and the space bar to jump. "
			"Use mouse to look around and mouse wheel to zoom. Press Q to take out or put back "
			"Sinbad's swords. With the swords equipped, you can left click to slice vertically or "
			"right click to slice horizontally. When the swords are not equipped, press E to "
			"start/stop a silly dance routine.";
	}
//////////////////////////////////////////////////////////////////////////
    void testCapabilities(const RenderSystemCapabilities* caps)
	{
        if (!caps->hasCapability(RSC_VERTEX_PROGRAM) || !caps->hasCapability(RSC_FRAGMENT_PROGRAM))
        {
			OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex or fragment shaders, "
                        "so you cannot run this Game. Sorry!", "Game_BSP::testCapabilities");
        }
	}
//////////////////////////////////////////////////////////////////////////
	StringVector getRequiredPlugins()
	{
		StringVector names;
        names.push_back("Cg Program Manager");
		names.push_back("BSP Scene Manager");
		return names;
	}

protected:
//////////////////////////////////////////////////////////////////////////
	void locateResources()
	{
		// load the Quake archive location and map name from a config file
		ConfigFile cf;
		cf.load(mFSLayer->getConfigFilePath("quakemap.cfg"));
		mArchive = cf.getSetting("Archive");
		mMap = cf.getSetting("Map");

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        // OS X does not set the working directory relative to the app,
        // In order to make things portable on OS X we need to provide
        // the loading with it's own bundle path location
        if (!Ogre::StringUtil::startsWith(mArchive, "/", false)) // only adjust relative dirs
            mArchive = Ogre::String(Ogre::macBundlePath() + "/" + mArchive);
#endif
        
		// add the Quake archive to the world resource group
		ResourceGroupManager::getSingleton().addResourceLocation(mArchive, "Zip",
			ResourceGroupManager::getSingleton().getWorldResourceGroupName(), true);
	}
//////////////////////////////////////////////////////////////////////////
	void createSceneManager()
	{
		mSceneMgr = mRoot->createSceneManager("BspSceneManager");   // the BSP scene manager is required for this Game
		// create main model
		
#ifdef INCLUDE_RTSHADER_SYSTEM
		mShaderGenerator->addSceneManager(mSceneMgr);
#endif
		if(mOverlaySystem)
			mSceneMgr->addRenderQueueListener(mOverlaySystem);
	}

//////////////////////////////////////////////////////////////////////////
	void createSound()
	{
		engine = createIrrKlangDevice();

		CMyFileFactory* factory = new CMyFileFactory();
		engine->addFileFactory(factory);
		factory->drop(); // we don't need it anymore, delete it

		// play a single sound
		engine->play3D("../../media/sound/XinChaoVietNam_Jmi.flac",
			vec3df(0,0,0), true, false, true);
	}
//////////////////////////////////////////////////////////////////////////
	void setupContent()
	{
		setupScene();
		createSound();
		createCharacter();
	}
//////////////////////////////////////////////////////////////////////////
	void setupScene()
	{
		mSceneMgr->setAmbientLight(Ogre::ColourValue(0.25, 0.25, 0.25));

		//Create Light
		Ogre::Light* pointLight = mSceneMgr->createLight("pointLight");
		pointLight->setType(Ogre::Light::LT_POINT);
		pointLight->setPosition(Ogre::Vector3(730,1007,-562));
		pointLight->setDiffuseColour(Ogre::ColourValue::White);
		pointLight->setSpecularColour(Ogre::ColourValue::White);
	}
////////////////////////////////////////////////////////////////////////// 
	//////////////////////////////////////////////////////////////////////////
	void createCharacter()
	{
		// disable default camera control so the character can do its own
		mCameraMan->setStyle(CS_MANUAL);

		mChara = new SinbadCharacterController(mCamera);

		mChara->setCharacterPos(Vector3(730,950,-562));
	}
////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////
	void loadResources()
	{
		/* NOTE: The browser initialises everything at the beginning already, so we use a 0 init proportion.
		If you're not compiling this Game for use with the browser, then leave the init proportion at 0.7. */
		mTrayMgr->showLoadingBar(1, 1, 0);

		// associate the world geometry with the world resource group, and then load the group
		ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
		rgm.linkWorldGeometryToResourceGroup(rgm.getWorldResourceGroupName(), mMap, mSceneMgr);
		rgm.initialiseResourceGroup(rgm.getWorldResourceGroupName());
		rgm.loadResourceGroup(rgm.getWorldResourceGroupName(), false);

		mTrayMgr->hideLoadingBar();
	}
//////////////////////////////////////////////////////////////////////////
	void unloadResources()
	{
		// unload the map so we don't interfere with subsequent Games
		ResourceGroupManager& rgm = ResourceGroupManager::getSingleton();
		rgm.unloadResourceGroup(rgm.getWorldResourceGroupName());
		rgm.removeResourceLocation(mArchive, ResourceGroupManager::getSingleton().getWorldResourceGroupName());
	}
	
//////////////////////////////////////////////////////////////////////////
	bool frameRenderingQueued(const FrameEvent& evt)
    {
		//PTR TuanNA [Add Character- 3/1/2017]
		mChara->addTime(evt.timeSinceLastFrame);

		return SdkGame::frameRenderingQueued(evt);  // don't forget the parent updates!
    }
//////////////////////////////////////////////////////////////////////////
	bool keyPressed (const OIS::KeyEvent &e)
	{
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectKeyDown(e);

		return SdkGame::keyPressed(e);
#endif

		return true;
	}
//////////////////////////////////////////////////////////////////////////
	bool keyReleased(const OIS::KeyEvent& e)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectKeyUp(e);
		return SdkGame::keyReleased(e);
	}
//////////////////////////////////////////////////////////////////////////
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
	bool touchPressed(const OIS::MultiTouchEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt);
		return SdkGame::touchPressed(evt);
	}
//////////////////////////////////////////////////////////////////////////
	bool touchMoved(const OIS::MultiTouchEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
		return SdkGame::touchMoved(evt);
	}
//////////////////////////////////////////////////////////////////////////
#else
	bool mouseMoved(const OIS::MouseEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
		return SdkGame::mouseMoved(evt);
	}
//////////////////////////////////////////////////////////////////////////
	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt, id);
		return SdkGame::mousePressed(evt, id);
	}
#endif
//////////////////////////////////////////////////////////////////////////
	//PTR TuanNA begin comment
	//[Clean up Grass and Ocean Surface- 11/12/2016]
	void cleanupContent()
	{
		// clean up character controller and the floor mesh
		if (mChara)
		{
			delete mChara;
			mChara = 0;
		}

		engine->drop(); // delete engine
	}
	//PTR TuanNA end comment
//////////////////////////////////////////////////////////////////////////
	String mArchive;
	String mMap;
	SinbadCharacterController* mChara;
	// start the sound engine with default parameters
	ISoundEngine* engine;
	ISound* music;
};

#endif
