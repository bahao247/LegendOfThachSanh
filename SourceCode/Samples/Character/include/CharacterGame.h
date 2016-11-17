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
| Filename   : CharacterGame.h                                                           
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
| Create File: 21:33:30 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:33:30 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#ifndef __Character_H__
#define __Character_H__

#include "SdkGame.h"
#include "SinbadCharacterController.h"

using namespace Ogre;
using namespace OgreBites;

class _OgreGameClassExport Game_Character : public SdkGame
{
public:

	Game_Character()
	{
		mInfo["Title"] = "Level 1";
		mInfo["Description"] = "Thach Sanh Banian-tree.";
		mInfo["Thumbnail"] = "thumb_level1.png";
		mInfo["Category"] = "Easy";
		mInfo["Help"] = "Use the WASD keys to move Sinbad, and the space bar to jump. "
			"Use mouse to look around and mouse wheel to zoom. Press Q to take out or put back "
			"Sinbad's swords. With the swords equipped, you can left click to slice vertically or "
			"right click to slice horizontally. When the swords are not equipped, press E to "
			"start/stop a silly dance routine.";
	}

	bool frameRenderingQueued(const FrameEvent& evt)
	{
		// let character update animations and camera
		mChara->addTime(evt.timeSinceLastFrame);
		return SdkGame::frameRenderingQueued(evt);
	}
	
	bool keyPressed(const OIS::KeyEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectKeyDown(evt);
		return SdkGame::keyPressed(evt);
	}
	
	bool keyReleased(const OIS::KeyEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectKeyUp(evt);
		return SdkGame::keyReleased(evt);
	}

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
	bool touchPressed(const OIS::MultiTouchEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt);
		return SdkGame::touchPressed(evt);
	}

	bool touchMoved(const OIS::MultiTouchEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
		return SdkGame::touchMoved(evt);
	}
#else
	bool mouseMoved(const OIS::MouseEvent& evt)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseMove(evt);
		return SdkGame::mouseMoved(evt);
	}

	bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
	{
		// relay input events to character controller
		if (!mTrayMgr->isDialogVisible()) mChara->injectMouseDown(evt, id);
		return SdkGame::mousePressed(evt, id);
	}
#endif

protected:

	void setupContent()
	{   
		// set background and some fog
		mViewport->setBackgroundColour(ColourValue(1.0f, 1.0f, 0.8f));
		mSceneMgr->setFog(Ogre::FOG_LINEAR, ColourValue(1.0f, 1.0f, 0.8f), 0, 15, 100);

		// set shadow properties
		mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_MODULATIVE);
		mSceneMgr->setShadowColour(ColourValue(0.5, 0.5, 0.5));
		mSceneMgr->setShadowTextureSize(1024);
		mSceneMgr->setShadowTextureCount(1);

		// disable default camera control so the character can do its own
		mCameraMan->setStyle(CS_MANUAL);

		// use a small amount of ambient lighting
		mSceneMgr->setAmbientLight(ColourValue(0.3, 0.3, 0.3));

		// add a bright light above the scene
		Light* light = mSceneMgr->createLight();
		light->setType(Light::LT_POINT);
		light->setPosition(-10, 40, 20);
		light->setSpecularColour(ColourValue::White);

		// create a floor mesh resource
		MeshManager::getSingleton().createPlane("floor", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			Plane(Vector3::UNIT_Y, 0), 100, 100, 10, 10, true, 1, 10, 10, Vector3::UNIT_Z);

		// create a floor entity, give it a material, and place it at the origin
        Entity* floor = mSceneMgr->createEntity("Floor", "floor");
        floor->setMaterialName("Examples/Rockwall");
		floor->setCastShadows(false);
        mSceneMgr->getRootSceneNode()->attachObject(floor);

//		LogManager::getSingleton().logMessage("creating sinbad");
		// create our character controller
		mChara = new SinbadCharacterController(mCamera);

//		LogManager::getSingleton().logMessage("toggling stats");
		mTrayMgr->toggleAdvancedFrameStats();

//		LogManager::getSingleton().logMessage("creating panel");
		StringVector items;
		items.push_back("Help");
		ParamsPanel* help = mTrayMgr->createParamsPanel(TL_TOPLEFT, "HelpMessage", 100, items);
		help->setParamValue("Help", "H / F1");
		
//		LogManager::getSingleton().logMessage("all done");
	}

	void cleanupContent()
	{
		// clean up character controller and the floor mesh
		if (mChara)
        {
            delete mChara;
            mChara = 0;
        }
		MeshManager::getSingleton().remove("floor");
	}

	SinbadCharacterController* mChara;
};

#endif
