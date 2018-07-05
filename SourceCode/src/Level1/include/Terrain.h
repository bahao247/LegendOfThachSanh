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
| Filename   : Terrain.h                                                           
---------------------------------------------------------------------------------------
| Written in : C++                                                     
---------------------------------------------------------------------------------------
| Compiler   : Microsoft Visual C++ 2010 Service Pack 1.                               
---------------------------------------------------------------------------------------
| Engine     : Ogre3D; ....................................                                                                     
---------------------------------------------------------------------------------------
| Written by : Ba Hao Nguyen ÅESports editors, IT.                                
---------------------------------------------------------------------------------------
| Note       : ....................................                                
---------------------------------------------------------------------------------------
| Create File: 21:31:47 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:31:47 07/01/2017 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#ifndef __Terrain_H__
#define __Terrain_H__
//////////////////////////////////////////////////////////////////////////
#pragma region [Include lib]
#include "GamePlugin.h"
#include "MaterialControlsCommon.h"
#include "SdkGame.h"
#include "OgreTerrain.h"
#include "OgreTerrainGroup.h"
#include "OgreTerrainQuadTreeNode.h"
#include "OgreTerrainMaterialGeneratorA.h"
#include "OgreTerrainPaging.h"
#include "SinbadCharacterControllerCommon.h"
#include "MyFileFactoryCommon.h"
#pragma endregion [Include lib]
//////////////////////////////////////////////////////////////////////////
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS || OGRE_PLATFORM == OGRE_PLATFORM_APPLE
#include "macUtils.h"
#endif
//////////////////////////////////////////////////////////////////////////
using namespace Ogre;
using namespace OgreBites;

class _OgreGameClassExport Game_Terrain : public SdkGame
{
public:

	Game_Terrain()
		: mTerrainGroup(0)
		, mTerrainPaging(0)
		, mPageManager(0)
		, mFly(false)
		, mFallVelocity(0)
		, mMode(MODE_NORMAL)
		, mLayerEdit(1)
		, mBrushSizeTerrainSpace(0.02)
		, mHeightUpdateCountDown(0)
		, mTerrainPos(1000,0,5000)
		, mTerrainsImported(false)
		,GRASS_WIDTH(40) 
		,GRASS_HEIGHT(40)
		,mHPSlider(0)
		,mHPBotSlider(0)
		,mMute(true)
		,mWeapon(false)
		,mActiveChara(true)
		,mWinChara(false)
		,mFinal(false)
		,mHPBot(100)
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

		// Update terrain at max 20fps
		mHeightUpdateRate = 1.0 / 20.0;
		for (int i = 0; i < 3; i++)
		{
			mRun[i] = false;
		}		
	}
//////////////////////////////////////////////////////////////////////////
    void testCapabilities(const RenderSystemCapabilities* caps)
	{
        if (!caps->hasCapability(RSC_VERTEX_PROGRAM) || !caps->hasCapability(RSC_FRAGMENT_PROGRAM))
        {
			OGRE_EXCEPT(Exception::ERR_NOT_IMPLEMENTED, "Your graphics card does not support vertex or fragment shaders, "
                        "so you cannot run this Game. Sorry!", "Game_Terrain::testCapabilities");
        }
	}
 //////////////////////////////////////////////////////////////////////////
	StringVector getRequiredPlugins()
	{
		StringVector names;
        if (!GpuProgramManager::getSingleton().isSyntaxSupported("glsles") && !GpuProgramManager::getSingleton().isSyntaxSupported("glsl150"))
            names.push_back("Cg Program Manager");
		return names;
	}
//////////////////////////////////////////////////////////////////////////
	void doTerrainModify(Terrain* terrain, const Vector3& centrepos, Real timeElapsed)
	{
		Vector3 tsPos;
		terrain->getTerrainPosition(centrepos, &tsPos);
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		if (mInputContext.isKeyDown(OIS::KC_EQUALS) || mInputContext.isKeyDown(OIS::KC_ADD) ||
				mInputContext.isKeyDown(OIS::KC_MINUS) || mInputContext.isKeyDown(OIS::KC_SUBTRACT))
		{
			switch(mMode)
			{
			case MODE_EDIT_HEIGHT:
				{
					// we need point coords
					Real terrainSize = (terrain->getSize() - 1);
					long startx = (tsPos.x - mBrushSizeTerrainSpace) * terrainSize;
					long starty = (tsPos.y - mBrushSizeTerrainSpace) * terrainSize;
					long endx = (tsPos.x + mBrushSizeTerrainSpace) * terrainSize;
					long endy= (tsPos.y + mBrushSizeTerrainSpace) * terrainSize;
					startx = std::max(startx, 0L);
					starty = std::max(starty, 0L);
					endx = std::min(endx, (long)terrainSize);
					endy = std::min(endy, (long)terrainSize);
					for (long y = starty; y <= endy; ++y)
					{
						for (long x = startx; x <= endx; ++x)
						{
							Real tsXdist = (x / terrainSize) - tsPos.x;
							Real tsYdist = (y / terrainSize)  - tsPos.y;

							Real weight = std::min((Real)1.0, 
								Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5 * mBrushSizeTerrainSpace));
							weight = 1.0 - (weight * weight);

							float addedHeight = weight * 250.0 * timeElapsed;
							float newheight;
							if (mInputContext.isKeyDown(OIS::KC_EQUALS) || mInputContext.isKeyDown(OIS::KC_ADD))
								newheight = terrain->getHeightAtPoint(x, y) + addedHeight;
							else
								newheight = terrain->getHeightAtPoint(x, y) - addedHeight;
							terrain->setHeightAtPoint(x, y, newheight);

						}
					}
					if (mHeightUpdateCountDown == 0)
						mHeightUpdateCountDown = mHeightUpdateRate;
				}
				break;
			case MODE_EDIT_BLEND:
				{
					TerrainLayerBlendMap* layer = terrain->getLayerBlendMap(mLayerEdit);
					// we need image coords
					Real imgSize = terrain->getLayerBlendMapSize();
					long startx = (tsPos.x - mBrushSizeTerrainSpace) * imgSize;
					long starty = (tsPos.y - mBrushSizeTerrainSpace) * imgSize;
					long endx = (tsPos.x + mBrushSizeTerrainSpace) * imgSize;
					long endy= (tsPos.y + mBrushSizeTerrainSpace) * imgSize;
					startx = std::max(startx, 0L);
					starty = std::max(starty, 0L);
					endx = std::min(endx, (long)imgSize);
					endy = std::min(endy, (long)imgSize);
					for (long y = starty; y <= endy; ++y)
					{
						for (long x = startx; x <= endx; ++x)
						{
							Real tsXdist = (x / imgSize) - tsPos.x;
							Real tsYdist = (y / imgSize)  - tsPos.y;

							Real weight = std::min((Real)1.0, 
								Math::Sqrt(tsYdist * tsYdist + tsXdist * tsXdist) / Real(0.5 * mBrushSizeTerrainSpace));
							weight = 1.0 - (weight * weight);

							float paint = weight * timeElapsed;
							size_t imgY = imgSize - y;
							float val;
							if (mInputContext.isKeyDown(OIS::KC_EQUALS) || mInputContext.isKeyDown(OIS::KC_ADD))
								val = layer->getBlendValue(x, imgY) + paint;
							else
								val = layer->getBlendValue(x, imgY) - paint;
							val = Math::Clamp(val, 0.0f, 1.0f);
							layer->setBlendValue(x, imgY, val);

						}
					}

					layer->update();
				}
				break;
            case MODE_NORMAL:
            case MODE_COUNT:
                break;
			};
		}
#endif

	}
//////////////////////////////////////////////////////////////////////////
	bool frameRenderingQueued(const FrameEvent& evt)
    {
#pragma region [Funct mMode]
		if (mMode != MODE_NORMAL)
		{
			// fire ray
			Ray ray; 
			//ray = mCamera->getCameraToViewportRay(0.5, 0.5);
			ray = mTrayMgr->getCursorRay(mCamera);

			TerrainGroup::RayResult rayResult = mTerrainGroup->rayIntersects(ray);
			if (rayResult.hit)
			{
				mEditMarker->setVisible(true);
				mEditNode->setPosition(rayResult.position);

				// figure out which terrains this affects
				TerrainGroup::TerrainList terrainList;
				Real brushSizeWorldSpace = TERRAIN_WORLD_SIZE * mBrushSizeTerrainSpace;
				Sphere sphere(rayResult.position, brushSizeWorldSpace);
				mTerrainGroup->sphereIntersects(sphere, &terrainList);

				for (TerrainGroup::TerrainList::iterator ti = terrainList.begin();
					ti != terrainList.end(); ++ti)
					doTerrainModify(*ti, rayResult.position, evt.timeSinceLastFrame);
			}
 			else
 			{
 				mEditMarker->setVisible(false);
 			}
		}
#pragma endregion [Funct mMode]
//////////////////////////////////////////////////////////////////////////
#pragma region [Funct mFly]
  		if (!mFly)
  		{
  			// clamp to terrain
  			Vector3 characterPos = mChara->getCharacterPos();
  			Ray ray;
  			ray.setOrigin(Vector3(characterPos.x, mTerrainPos.y + 10000, characterPos.z));
  			ray.setDirection(Vector3::NEGATIVE_UNIT_Y);
  
  			TerrainGroup::RayResult rayResult = mTerrainGroup->rayIntersects(ray);
  			Real distanceAboveTerrain = 5;
  			Real fallSpeed = 300;
  			Real newy = characterPos.y;
  			if (rayResult.hit)
  			{				
				if (characterPos.y > rayResult.position.y + distanceAboveTerrain)
				{
					if (!(mChara->getAnime() == 11))
					{
						mFallVelocity += evt.timeSinceLastFrame * 20;
						mFallVelocity = std::min(mFallVelocity, fallSpeed);
						newy = characterPos.y - mFallVelocity * evt.timeSinceLastFrame;
					}
				}
				else if(mChara->checkJump())
				{
					mChara->setMinPos(true);
				}

				newy = std::max(rayResult.position.y + distanceAboveTerrain, newy);
				mChara->setCharacterPos(Vector3(characterPos.x, newy, characterPos.z));
  			}

			if(mChara->getHP() == 0)
			{
				if (mActiveChara)
				{
					engine->play3D("../../media/sound/death1.wav",
						vec3df(0,0,0), false, false, true);
					mChara->setDead();
					
					Vector3 deadVector = mChara->getCharacterPos();
					deadVector.y = mTerrainGroup->getHeightAtWorldPosition(deadVector) - 500; 
					mChara->setCharacterPos(deadVector);

					mActiveChara = false;
				}	
			}
			else if ((!mHPBot /*|| mWinChara*/) && !mFinal)
			{
				mChara->setWinAnime();
				mFinal = true;
				mWinChara = true;
				engine->removeSoundSource("../../media/sound/footsteps-running.wav");
				// play a single sound
				engine->play3D("../../media/sound/fireworks.mp3",
					vec3df(0,0,0), true, false, true);
			}
			else if (isCollision())
			{
				//mWinChara = true;
 				mChara->setSpeed(SUB_RUN_SPEED);
				LogManager::getSingleton().logMessage("True Collision!!!!!!");
			}
			else
			{
				if (mChara->getSpeed() == SUB_RUN_SPEED)
				{
					mChara->setSpeed(RUN_SPEED);
				}
				LogManager::getSingleton().logMessage("Not Collision!!!!!!");
			}
			//mChara->setHP(mChara->getHP() - 0.000000001);
			//mHPSlider->setValue(mChara->getHP());

			isCollision(mHPBot);
			//mHPBotSlider->setValue(mHPBot);
  		}
#pragma endregion [Funct mFly]
//////////////////////////////////////////////////////////////////////////
#pragma region [Funct mHeight]
		if (mHeightUpdateCountDown > 0)
		{
			mHeightUpdateCountDown -= evt.timeSinceLastFrame;
			if (mHeightUpdateCountDown <= 0)
			{
				mTerrainGroup->update();
				mHeightUpdateCountDown = 0;

			}

		}
#pragma endregion [Funct mHeight]
//////////////////////////////////////////////////////////////////////////
#pragma region [Funct build terrain isDerivedDataUpdateInProgress]
		if (mTerrainGroup->isDerivedDataUpdateInProgress())
		{
			mTrayMgr->moveWidgetToTray(mInfoLabel, TL_TOP, 0);
			mInfoLabel->show();
			if (mTerrainsImported)
			{
				mInfoLabel->setCaption("Building terrain, please wait...");
			}
			else
			{
				mInfoLabel->setCaption("Updating textures, patience...");
			}
		}
		else
		{
			mTrayMgr->removeWidgetFromTray(mInfoLabel);
			mInfoLabel->hide();
			if (mTerrainsImported)
			{
				saveTerrains(true);
				mTerrainsImported = false;
			}
		}
#pragma endregion [Funct build terrain isDerivedDataUpdateInProgress]
//////////////////////////////////////////////////////////////////////////
		//PTR TuanNA [Add Character- 3/1/2017]
 
		mChara->addTime(evt.timeSinceLastFrame);

		//PTR TuanNA [Wave Grass- 3/1/2017]
		waveGrass(evt.timeSinceLastFrame);

		return SdkGame::frameRenderingQueued(evt);  // don't forget the parent updates!
    }
//////////////////////////////////////////////////////////////////////////
	void saveTerrains(bool onlyIfModified)
	{
		mTerrainGroup->saveAllTerrains(onlyIfModified);
	}
//////////////////////////////////////////////////////////////////////////
	bool keyPressed (const OIS::KeyEvent &e)
	{
#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
		// relay input events to character controller
		if (mActiveChara && !mWinChara)
		{
			if (!mTrayMgr->isDialogVisible()) mChara->injectKeyDown(e);
		
			if ((e.key == OIS::KC_W || e.key == OIS::KC_A || e.key == OIS::KC_S || e.key == OIS::KC_D) && (mRun[0] + mRun[1] + mRun[2] + mRun[3] == 0))
			{
				engine->play3D("../../media/sound/footsteps-running.wav",
					vec3df(0,0,0), true, false, true);
			}

			if (e.key == OIS::KC_W )
			{
				mRun[0] = true;
			}else if (e.key == OIS::KC_A)
			{
				mRun[1] = true;
			}else if (e.key == OIS::KC_S)
			{
				mRun[2] = true;
			}else if (e.key == OIS::KC_D)
			{
				mRun[3] = true;
			}

			switch (e.key)
			{
			case OIS::KC_SPACE:
				engine->play3D("../../media/sound/jump.mp3",
					vec3df(0,0,0), false, false, true);
				break;
			case OIS::KC_Q:
				mWeapon = !mWeapon;
				break;
			case OIS::KC_M:
				// CTRL-S to save
				if (engine)
				{
					engine->setAllSoundsPaused(mMute);
					mMute = !mMute;
				}
				else
				{
					return SdkGame::keyPressed(e);
				}
				break;
			case OIS::KC_S:
				// CTRL-S to save
				if (mInputContext.isKeyDown(OIS::KC_LCONTROL) || mInputContext.isKeyDown(OIS::KC_RCONTROL))
				{
					saveTerrains(true);
				}
				else
					return SdkGame::keyPressed(e);
				break;
			case OIS::KC_F10:
				// dump
				{
					TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
					while (ti.hasMoreElements())
					{
						Ogre::uint32 tkey = ti.peekNextKey();
						TerrainGroup::TerrainSlot* ts = ti.getNext();
						if (ts->instance && ts->instance->isLoaded())
						{
							ts->instance->_dumpTextures("terrain_" + StringConverter::toString(tkey), ".png");
						}
					}
				}
				break;
				/*
			case OIS::KC_F7:
				// change terrain size
				if (mTerrainGroup->getTerrainSize() == 513)
					mTerrainGroup->setTerrainSize(1025);
				else
					mTerrainGroup->setTerrainSize(513);
				break;
			case OIS::KC_F8:
				// change terrain world size
				if (mTerrainGroup->getTerrainWorldSize() == TERRAIN_WORLD_SIZE)
					mTerrainGroup->setTerrainWorldSize(TERRAIN_WORLD_SIZE * 2);
				else
					mTerrainGroup->setTerrainWorldSize(TERRAIN_WORLD_SIZE);
				break;
				*/
	// 		case OIS::KC_Z:
	// 			mHPSlider->setValue(++mHP);
	// 			break;
			default:
			return SdkGame::keyPressed(e);	
			}

		}
#endif

		return true;
	}
//////////////////////////////////////////////////////////////////////////
	bool keyReleased(const OIS::KeyEvent& e)
	{
		// relay input events to character controller
		if (mActiveChara && !mWinChara)
		{
			if (!mTrayMgr->isDialogVisible()) mChara->injectKeyUp(e);

			if (e.key == OIS::KC_W )
			{
				mRun[0] = false;
			}else if (e.key == OIS::KC_A)
			{
				mRun[1] = false;
			}else if (e.key == OIS::KC_S)
			{
				mRun[2] = false;
			}else if (e.key == OIS::KC_D)
			{
				mRun[3] = false;
			}

			if ((e.key == OIS::KC_W || e.key == OIS::KC_A || e.key == OIS::KC_S || e.key == OIS::KC_D) && (mRun[0] + mRun[1] + mRun[2] + mRun[3] == 0))
			{
				engine->removeSoundSource("../../media/sound/footsteps-running.wav");
			}
		}
		return SdkGame::keyReleased(e);
	}
//////////////////////////////////////////////////////////////////////////
	void itemSelected(SelectMenu* menu)
	{
		if (menu == mEditMenu)
		{
			mMode = (Mode)mEditMenu->getSelectionIndex();
		}
		else if (menu == mShadowsMenu)
		{
			mShadowMode = (ShadowMode)mShadowsMenu->getSelectionIndex();
			changeShadows();
		}
	}
//////////////////////////////////////////////////////////////////////////
	void checkBoxToggled(CheckBox* box)
	{
		if (box == mFlyBox)
		{
			mFly = mFlyBox->isChecked();
		}
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

		if (mWeapon)
		{
			engine->play3D("../../media/sound/knife_hit1.wav",
				vec3df(0,0,0), false, false, true);
		}
		return SdkGame::mousePressed(evt, id);
	}
#endif
////////////////////////////////////////////////////////////////////////// 
//////////////////////////////////////////////////////////////////////////
protected:
	MaterialControlsContainer mMaterialControlsContainer;
	TerrainGlobalOptions* mTerrainGlobals;
	TerrainGroup* mTerrainGroup;
	bool mPaging;
	TerrainPaging* mTerrainPaging;
	PageManager* mPageManager;
#ifdef PAGING
	/// This class just pretends to provide prcedural page content to avoid page loading
	class DummyPageProvider : public PageProvider
	{
	public:
		bool prepareProceduralPage(Page* page, PagedWorldSection* section) { return true; }
		bool loadProceduralPage(Page* page, PagedWorldSection* section) { return true; }
		bool unloadProceduralPage(Page* page, PagedWorldSection* section) { return true; }
		bool unprepareProceduralPage(Page* page, PagedWorldSection* section) { return true; }
	};
	DummyPageProvider mDummyPageProvider;
#endif
	bool mFly;
	Real mFallVelocity;

	Mode mMode;
	ShadowMode mShadowMode;
	Ogre::uint8 mLayerEdit;
	Real mBrushSizeTerrainSpace;
	SceneNode* mEditNode;
	Entity* mEditMarker;
	Real mHeightUpdateCountDown;
	Real mHeightUpdateRate;
	Vector3 mTerrainPos;
	SelectMenu* mEditMenu;
	SelectMenu* mShadowsMenu;
	CheckBox* mFlyBox;
	OgreBites::Label* mInfoLabel;
	bool mTerrainsImported;
	ShadowCameraSetupPtr mPSSMSetup;
	SceneNode* mBodyNode1;
	Entity* mBodyEnt1;
	Ogre::Real mMechLength;

	typedef std::list<Entity*> EntityList;
	EntityList mHouseList;
	EntityList mBanianTreeList;
	EntityList mCharaList;

	//PTR TuanNA [Add grass into map- 11/12/2016]
	const Real GRASS_WIDTH;
	const Real GRASS_HEIGHT;
	StaticGeometry* mField;
	// start the sound engine with default parameters
	ISoundEngine* engine;
	ISound* music;
	
//////////////////////////////////////////////////////////////////////////
	void defineTerrain(long x, long y, bool flat = false)
	{
		// if a file is available, use it
		// if not, generate file from import

		// Usually in a real project you'll know whether the compact terrain data is
		// available or not; I'm doing it this way to save distribution size

		if (flat)
		{
			mTerrainGroup->defineTerrain(x, y, 0.0f);
		}
		else
		{
			String filename = mTerrainGroup->generateFilename(x, y);
			if (ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
			{
				mTerrainGroup->defineTerrain(x, y);
			}
			else
			{
				Image img;
				getTerrainImage(x % 2 != 0, y % 2 != 0, img);
				mTerrainGroup->defineTerrain(x, y, &img);
				mTerrainsImported = true;
			}

		}
	}
//////////////////////////////////////////////////////////////////////////
	void getTerrainImage(bool flipX, bool flipY, Image& img)
	{
		img.load("terrain.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		if (flipX)
			img.flipAroundY();
		if (flipY)
			img.flipAroundX();

	}
//////////////////////////////////////////////////////////////////////////
	void initBlendMaps(Terrain* terrain)
	{
		TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
		TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
		Real minHeight0 = 70;
		Real fadeDist0 = 40;
		Real minHeight1 = 70;
		Real fadeDist1 = 15;
		float* pBlend1 = blendMap1->getBlendPointer();
		float* pBlend0 = blendMap0->getBlendPointer();
		for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
		{
			for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
			{
				Real tx, ty;

				blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
				Real height = terrain->getHeightAtTerrainPosition(tx, ty);
				Real val = (height - minHeight0) / fadeDist0;
				val = Math::Clamp(val, (Real)0, (Real)1);
				*pBlend0++ = val;

				val = (height - minHeight1) / fadeDist1;
				val = Math::Clamp(val, (Real)0, (Real)1);
				*pBlend1++ = val;


			}
		}
		blendMap0->dirty();
		blendMap1->dirty();
		//blendMap0->loadImage("blendmap1.png", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		blendMap0->update();
		blendMap1->update();

		// set up a colour map
		/*
		if (!terrain->getGlobalColourMapEnabled())
		{
			terrain->setGlobalColourMapEnabled(true);
			Image colourMap;
			colourMap.load("testcolourmap.jpg", ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			terrain->getGlobalColourMap()->loadImage(colourMap);
		}
		*/

	}
//////////////////////////////////////////////////////////////////////////
	void configureTerrainDefaults(Light* l)
	{
		// Configure global
		mTerrainGlobals->setMaxPixelError(8);
		// testing composite map
		mTerrainGlobals->setCompositeMapDistance(3000);
		//mTerrainGlobals->setUseRayBoxDistanceCalculation(true);
		//mTerrainGlobals->getDefaultMaterialGenerator()->setDebugLevel(1);
		//mTerrainGlobals->setLightMapSize(256);

#if OGRE_NO_GLES3_SUPPORT == 1
        // Disable the lightmap for OpenGL ES 2.0. The minimum number of Gamers allowed is 8(as opposed to 16 on desktop).
        // Otherwise we will run over the limit by just one. The minimum was raised to 16 in GL ES 3.0.
        if (Ogre::Root::getSingletonPtr()->getRenderSystem()->getName().find("OpenGL ES 2") != String::npos)
        {
            TerrainMaterialGeneratorA::SM2Profile* matProfile =
                static_cast<TerrainMaterialGeneratorA::SM2Profile*>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
            matProfile->setLightmapEnabled(false);
        }
#endif
		// Important to set these so that the terrain knows what to use for derived (non-realtime) data
		mTerrainGlobals->setLightMapDirection(l->getDerivedDirection());
		mTerrainGlobals->setCompositeMapAmbient(mSceneMgr->getAmbientLight());
		//mTerrainGlobals->setCompositeMapAmbient(ColourValue::Red);
		mTerrainGlobals->setCompositeMapDiffuse(l->getDiffuseColour());

		// Configure default import settings for if we use imported image
		Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
		defaultimp.terrainSize = TERRAIN_SIZE;
		defaultimp.worldSize = TERRAIN_WORLD_SIZE;
		defaultimp.inputScale = 600;
		defaultimp.minBatchSize = 33;
		defaultimp.maxBatchSize = 65;
		// textures
		defaultimp.layerList.resize(3);
		defaultimp.layerList[0].worldSize = 100;
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
		defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
		defaultimp.layerList[1].worldSize = 30;
		defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
		defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
		defaultimp.layerList[2].worldSize = 200;
		defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
		defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");


	}
//////////////////////////////////////////////////////////////////////////
	void addTextureDebugOverlay(TrayLocation loc, TexturePtr tex, size_t i)
	{
		addTextureDebugOverlay(loc, tex->getName(), i);
	}
//////////////////////////////////////////////////////////////////////////
	void addTextureDebugOverlay(TrayLocation loc, const String& texname, size_t i)
	{
		// Create material
		String matName = "Ogre/DebugTexture" + StringConverter::toString(i);
		MaterialPtr debugMat = MaterialManager::getSingleton().getByName(matName);
		if (debugMat.isNull())
		{
			debugMat = MaterialManager::getSingleton().create(matName,
				ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
		}
		Pass* p = debugMat->getTechnique(0)->getPass(0);
		p->removeAllTextureUnitStates();
		p->setLightingEnabled(false);
		TextureUnitState *t = p->createTextureUnitState(texname);
		t->setTextureAddressingMode(TextureUnitState::TAM_CLAMP);

		// create template
		if (!OverlayManager::getSingleton().hasOverlayElement("Ogre/DebugTexOverlay", true))
		{
			OverlayElement* e = OverlayManager::getSingleton().createOverlayElement("Panel", "Ogre/DebugTexOverlay", true);
			e->setMetricsMode(GMM_PIXELS);
			e->setWidth(128);
			e->setHeight(128);
		}

		// add widget
		String widgetName = "DebugTex"+ StringConverter::toString(i);
		Widget* w = mTrayMgr->getWidget(widgetName);
		if (!w)
		{
			w = mTrayMgr->createDecorWidget(loc, widgetName, "Ogre/DebugTexOverlay");
		}
		w->getOverlayElement()->setMaterialName(matName);

	}
//////////////////////////////////////////////////////////////////////////
	void addTextureShadowDebugOverlay(TrayLocation loc, size_t num)
	{
		for (size_t i = 0; i < num; ++i)
		{
			TexturePtr shadowTex = mSceneMgr->getShadowTexture(i);
			addTextureDebugOverlay(loc, shadowTex, i);

		}

	}
//////////////////////////////////////////////////////////////////////////
	MaterialPtr buildDepthShadowMaterial(const String& textureName)
	{
		String matName = "DepthShadows/" + textureName;

		MaterialPtr ret = MaterialManager::getSingleton().getByName(matName);
		if (ret.isNull())
		{
			MaterialPtr baseMat = MaterialManager::getSingleton().getByName("Ogre/shadow/depth/integrated/pssm");
			ret = baseMat->clone(matName);
			Pass* p = ret->getTechnique(0)->getPass(0);
			p->getTextureUnitState("diffuse")->setTextureName(textureName);

			Vector4 splitPoints;
			const PSSMShadowCameraSetup::SplitPointList& splitPointList = 
				static_cast<PSSMShadowCameraSetup*>(mPSSMSetup.get())->getSplitPoints();
			for (int i = 0; i < 3; ++i)
			{
				splitPoints[i] = splitPointList[i];
			}
			p->getFragmentProgramParameters()->setNamedConstant("pssmSplitPoints", splitPoints);


		}

		return ret;
	}
//////////////////////////////////////////////////////////////////////////
	void changeShadows()
	{
		configureShadows(mShadowMode != SHADOWS_NONE, mShadowMode == SHADOWS_DEPTH);
	}
//////////////////////////////////////////////////////////////////////////
	void configureShadows(bool enabled, bool depthShadows)
	{
		TerrainMaterialGeneratorA::SM2Profile* matProfile = 
			static_cast<TerrainMaterialGeneratorA::SM2Profile*>(mTerrainGlobals->getDefaultMaterialGenerator()->getActiveProfile());
		matProfile->setReceiveDynamicShadowsEnabled(enabled);
#ifdef SHADOWS_IN_LOW_LOD_MATERIAL
		matProfile->setReceiveDynamicShadowsLowLod(true);
#else
		matProfile->setReceiveDynamicShadowsLowLod(false);
#endif

		// Default materials
		for (EntityList::iterator i = mHouseList.begin(); i != mHouseList.end(); ++i)
		{
			(*i)->setMaterialName("Examples/TudorHouse");
		}

		if (enabled)
		{
			// General scene setup
			mSceneMgr->setShadowTechnique(SHADOWTYPE_TEXTURE_ADDITIVE_INTEGRATED);
			mSceneMgr->setShadowFarDistance(3000);

			// 3 textures per directional light (PSSM)
			mSceneMgr->setShadowTextureCountPerLightType(Ogre::Light::LT_DIRECTIONAL, 3);

			if (mPSSMSetup.isNull())
			{
				// shadow camera setup
				PSSMShadowCameraSetup* pssmSetup = new PSSMShadowCameraSetup();
				pssmSetup->setSplitPadding(mCamera->getNearClipDistance());
				pssmSetup->calculateSplitPoints(3, mCamera->getNearClipDistance(), mSceneMgr->getShadowFarDistance());
				pssmSetup->setOptimalAdjustFactor(0, 2);
				pssmSetup->setOptimalAdjustFactor(1, 1);
				pssmSetup->setOptimalAdjustFactor(2, 0.5);

				mPSSMSetup.bind(pssmSetup);

			}
			mSceneMgr->setShadowCameraSetup(mPSSMSetup);

			if (depthShadows)
			{
				mSceneMgr->setShadowTextureCount(3);
				mSceneMgr->setShadowTextureConfig(0, 2048, 2048, PF_FLOAT32_R);
				mSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_FLOAT32_R);
				mSceneMgr->setShadowTextureConfig(2, 1024, 1024, PF_FLOAT32_R);
				mSceneMgr->setShadowTextureSelfShadow(true);
				mSceneMgr->setShadowCasterRenderBackFaces(true);

				MaterialPtr houseMat = buildDepthShadowMaterial("fw12b.jpg");
				for (EntityList::iterator i = mHouseList.begin(); i != mHouseList.end(); ++i)
				{
					(*i)->setMaterial(houseMat);
				}

			}
			else
			{
				mSceneMgr->setShadowTextureCount(3);
				mSceneMgr->setShadowTextureConfig(0, 2048, 2048, PF_X8B8G8R8);
				mSceneMgr->setShadowTextureConfig(1, 1024, 1024, PF_X8B8G8R8);
				mSceneMgr->setShadowTextureConfig(2, 1024, 1024, PF_X8B8G8R8);
				mSceneMgr->setShadowTextureSelfShadow(false);
				mSceneMgr->setShadowCasterRenderBackFaces(false);
				mSceneMgr->setShadowTextureCasterMaterial(StringUtil::BLANK);
			}

			matProfile->setReceiveDynamicShadowsDepth(depthShadows);
			matProfile->setReceiveDynamicShadowsPSSM(static_cast<PSSMShadowCameraSetup*>(mPSSMSetup.get()));

			//addTextureShadowDebugOverlay(TL_RIGHT, 3);


		}
		else
		{
			mSceneMgr->setShadowTechnique(SHADOWTYPE_NONE);
		}


	}
//////////////////////////////////////////////////////////////////////////
	void setupControls()
	{
		mTrayMgr->showCursor();

		// make room for the controls
		mTrayMgr->showLogo(TL_TOPRIGHT);
		mTrayMgr->showFrameStats(TL_TOPRIGHT);
		mTrayMgr->toggleAdvancedFrameStats();

		mInfoLabel = mTrayMgr->createLabel(TL_TOP, "TInfo", "", 350);

		mEditMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "EditMode", "Edit Mode", 370, 250, 3);
		mEditMenu->addItem("None");
		mEditMenu->addItem("Elevation");
		mEditMenu->addItem("Blend");
		mEditMenu->selectItem(0);  // no edit mode

		mFlyBox = mTrayMgr->createCheckBox(TL_BOTTOM, "Fly", "Fly");
		mFlyBox->setChecked(false, false);

		mShadowsMenu = mTrayMgr->createLongSelectMenu(TL_BOTTOM, "Shadows", "Shadows", 370, 250, 3);
		mShadowsMenu->addItem("None");
		mShadowsMenu->addItem("Colour Shadows");
		mShadowsMenu->addItem("Depth Shadows");
		mShadowsMenu->selectItem(0);  // no edit mode

		// a friendly reminder
		StringVector names;
		names.push_back("Help");
		mTrayMgr->createParamsPanel(TL_TOPLEFT, "Help", 100, names)->setParamValue(0, "H/F1");
	}
//////////////////////////////////////////////////////////////////////////
	//PTR TuanNA begin comment
	//[Init GrassVertex- 11/12/2016]
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(push, 1)
#endif
	struct GrassVertex
	{
		float x, y, z;
		float nx, ny, nz;
		float u, v;
	};
#if OGRE_COMPILER == OGRE_COMPILER_MSVC
#	pragma pack(pop)
#endif
	//PTR TuanNA end comment
//////////////////////////////////////////////////////////////////////////
/////////////-------------------------------------////////////////////////
////////////////////////////////////////////////////////////////////////// 
//PTR TuanNA begin comment
//[SETTUP CONTENT APP- 3/1/2017]
	void setupContent()
	{
		//PTR TuanNA [Create Map game- 3/1/2017]
		createscene();
		createCharacter();
		createSound();
		//createControl();
	}
//PTR TuanNA end comment
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
	void isCollision(int& HPBot) 
	{
		// fire ray
		// execute the query, returns a vector of hits
		RaySceneQuery* mRaySceneQuery = mSceneMgr->createRayQuery(Ray());

		Ray ray;

		ray.setOrigin(mChara->getCharacterPos());
		ray.setDirection(mChara->getGoalDirection());

		// create a query object
		mRaySceneQuery->setRay(ray);

		RaySceneQueryResult &result = mRaySceneQuery->execute();
		RaySceneQueryResult::iterator itr;
		for (itr = result.begin(); itr != result.end(); itr++) {

			if ((itr->movable->getName() != "") && itr->distance < 100) 
			{
				LogManager::getSingleton().logMessage("That is " + itr->movable->getName());

				if (itr->movable->getName().compare("RiceCooker0")==0 && itr->distance<mMechLength/2) 
				{
					 HPBot -= 20;
					 // play a single sound
					 engine->play3D("../../media/sound/bell.wav",
						 vec3df(0,0,0), false, false, true);
					 e0->setVisible(false);
				}
				else if (itr->movable->getName().compare("RiceCooker1")==0 && itr->distance<mMechLength/2)
				{
					HPBot -= 20;
					// play a single sound
					engine->play3D("../../media/sound/bell.wav",
						vec3df(0,0,0), false, false, true);
					e1->setVisible(false);
				}
				else if (itr->movable->getName().compare("RiceCooker2")==0 && itr->distance<mMechLength/2)
				{
					HPBot -= 20;
					// play a single sound
					engine->play3D("../../media/sound/bell.wav",
						vec3df(0,0,0), false, false, true);
					e2->setVisible(false);
				}
				else if (itr->movable->getName().compare("RiceCooker3")==0 && itr->distance<mMechLength/2)
				{
					HPBot -= 20;
					// play a single sound
					engine->play3D("../../media/sound/bell.wav",
						vec3df(0,0,0), false, false, true);
					e3->setVisible(false);
				}
				else if (itr->movable->getName().compare("RiceCooker4")==0 && itr->distance<mMechLength/2)
				{
					HPBot -= 20;
					// play a single sound
					engine->play3D("../../media/sound/bell.wav",
						vec3df(0,0,0), false, false, true);
					e4->setVisible(false);
				}
			}
		}
	}
////////////////////////////////////////////////////////////////////////// 
////////////////////////////////////////////////////////////////////////// 
	bool isCollision() 
	{
		// fire ray
		// execute the query, returns a vector of hits
		RaySceneQuery* mRaySceneQuery = mSceneMgr->createRayQuery(Ray());

		Ray ray;

		ray.setOrigin(mChara->getCharacterPos());
		ray.setDirection(mChara->getGoalDirection());

		// create a query object
		mRaySceneQuery->setRay(ray);

		RaySceneQueryResult &result = mRaySceneQuery->execute();
		RaySceneQueryResult::iterator itr;
		for (itr = result.begin(); itr != result.end(); itr++) {

			if ((itr->movable->getName() != "") && itr->distance < 100) {
				LogManager::getSingleton().logMessage("That is " + itr->movable->getName());

				if ((itr->movable->getName().compare("OceanSurface")!=0 && itr->movable->getName().compare("Banian-tree0")!=0 && itr->movable->getName().compare("Banian-tree1")!=0 && itr->movable->getName().compare("Banian-tree2")!=0 && itr->movable->getName().compare("Banian-tree3")!=0 && itr->movable->getName().compare("Banian-tree4")!=0 && itr->movable->getName().compare("Cathedral")!=0 && itr->movable->getName().compare("SinbadSword1")!=0 && itr->movable->getName().compare("SinbadSword2")!=0 && itr->movable->getName().compare("SinbadBody")!=0 && itr->movable->getName().compare("MainCamera")!=0 && itr->movable->getName().compare("Ogre/MO5")!=0) && itr->distance<mMechLength/2) {
					return true; 
				}
			}
		}
		return false;
	}
////////////////////////////////////////////////////////////////////////// 
//////////////-------------------------------------//////////////////////// 
	void createControl()
	{
		mHPSlider = mTrayMgr->createThickSlider(TL_TOP, "HPSlider", "HP", 250, 80, 0, 100, 0);
		mHPSlider->setRange(0, 100, 100);
		sliderMoved(mHPSlider);
		//mHPSlider->setValue(mChara->getHP());

		mHPBotSlider = mTrayMgr->createThickSlider(TL_BOTTOMLEFT, "HPBotSlider", "HP Bot", 250, 80, 0, 100, 0);
		mHPBotSlider->setRange(0, 100, 100);
		sliderMoved(mHPBotSlider);
		//mHPBotSlider->setValue(mHPBot);
	}
//////////////////////////////////////////////////////////////////////////
/*-----------------------------------------------------------------------------
| Handles Game slider changes.
-----------------------------------------------------------------------------*/
	virtual void sliderMoved(Slider* slider)
		{
			// format the caption to be fraction style
			Ogre::String denom = "/" + Ogre::StringConverter::toString(100);
			slider->setValueCaption(slider->getValueCaption() + denom);
		}
//////////////-------------------------------------//////////////////////// 
//////////////////////////////////////////////////////////////////////////
	void createCharacter()
	{
		// disable default camera control so the character can do its own
		mCameraMan->setStyle(CS_MANUAL);
		
		mChara = new SinbadCharacterController(mCamera);

		mChara->setCharacterPos(Vector3(-1922, 0, 2037));
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(Vector3(-1922, 0, 2037));
		sn->attachObject(mSceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus"));

		// create main model
		mBodyNode1 = mSceneMgr->getRootSceneNode()->createChildSceneNode(mTerrainPos + Vector3(100, mTerrainGroup->getHeightAtWorldPosition(mTerrainPos) + 5, 0));
		mBodyEnt1 = mSceneMgr->createEntity("SinbadBody1", "Sinbad.mesh");

  		SubEntity* subEntity = mBodyEnt1->getSubEntity(6);
  		subEntity->setMaterialName("Sinbad/RedClothes");

		mBodyNode1->attachObject(mBodyEnt1);
		Ogre::AxisAlignedBox box = mBodyEnt1->getBoundingBox();
		mBodyNode1->showBoundingBox(true);
		mBodyNode1->setScale(0.5,0.5,0.5);
		mMechLength = box.getSize().x;

		//Python
		// create a child node and attach an ogre head and some smoke to it
// 		Quaternion rot;
// 		Entity* e = mSceneMgr->createEntity("Python1", "snake.mesh");
// 		Vector3 entPos = Vector3(1900, 0, 7100);
// 		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
// 		SceneNode* sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
// 		rot.FromAngleAxis(Degree(180), Vector3::UNIT_Y);
// 		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
// 		sn->setScale(Vector3(3, 3, 3));
// 		sn->attachObject(e);
// 		mCharaList.push_back(e);
// 		//sn->attachObject(mSceneMgr->createParticleSystem("Smoke", "Examples/Smoke"));
// 		// create a green nimbus around the ogre head
// 		// 
// 		sn->attachObject(mSceneMgr->createParticleSystem("Aureola", "Examples/Aureola"));
// 
// 		e = mSceneMgr->createEntity("Bow1", "bow.mesh");
// 		entPos.y += 65;
// 		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
// 		sn->setScale(Vector3(0.1, 0.1, 0.1));
// 		sn->attachObject(e);
// 		mCharaList.push_back(e);
// 		sn->attachObject(mSceneMgr->createParticleSystem("Nimbus", "Examples/GreenyNimbus"));

		//axe_1.mesh - riu
		//princes_1.mesh - cong chua
		// girl.mesh- con thuy te
		// pipa_2.mesh - dan than
		// RiceCooker.mesh- noi com
		// wood.mesh- dong cui
		// eagle.mesh- dai bang tinh
		//Banian-tree0
		e = mSceneMgr->createEntity("Axe1", "axe_1.mesh");
		entPos = Vector3(-2000, 0, 2010);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e);
		mCharaList.push_back(e);
// 
// 		e = mSceneMgr->createEntity("Pipa1", "pipa_2.mesh");
// 		entPos = Vector3(-2000, 0, 2020);
// 		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
// 		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
// 		sn->setScale(Vector3(0.3, 0.3, 0.3));
// 		sn->attachObject(e);
// 		mCharaList.push_back(e);

		e0 = mSceneMgr->createEntity("RiceCooker0", "RiceCooker.mesh");
		entPos = Vector3(-1950, 0, 2037);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e0);
		mCharaList.push_back(e0);

		e = mSceneMgr->createEntity("wood1", "wood.mesh");
		entPos = Vector3(-2000, 0, 2050);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e);
		mCharaList.push_back(e);

		////-3456, 0, -739/ Con Thuy Te
		//e = mSceneMgr->createEntity("girl1", "girl.mesh");
		//entPos = Vector3(-3456, 0, -739);
		//entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		//sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		//sn->setScale(Vector3(0.05, 0.05, 0.05));
		//sn->attachObject(e);
		//mCharaList.push_back(e);

		////Cong chua- -4283, 0, -264
		//e = mSceneMgr->createEntity("princes1", "princes_1.mesh");
		//entPos = Vector3(-3466, 0, -739);
		//entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		//sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		//sn->setScale(Vector3(5, 5, 5));
		//sn->attachObject(e);
		//mCharaList.push_back(e);

		////eagle- -4305, 0, -673
		//e = mSceneMgr->createEntity("eagle1", "eagle.mesh");
		//entPos = Vector3(-4305, 0, -673);
		//entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 10;
		//sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		//sn->setScale(Vector3(15, 15, 15));
		//sn->attachObject(e);
		//mCharaList.push_back(e);
		//sn->attachObject(mSceneMgr->createParticleSystem("Aureola2", "Examples/Aureola"));

		////king- 2418, 311, 3507
		//e = mSceneMgr->createEntity("king1", "king.mesh");
		//entPos = Vector3(2408, 298, 3508);
		//rot.FromAngleAxis(Degree(180), Vector3::UNIT_Y);
		//sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		//sn->setScale(Vector3(0.12, 0.12, 0.12));
		//sn->attachObject(e);
		//mCharaList.push_back(e);

		////Cong chua- -4283, 0, -264
		//e = mSceneMgr->createEntity("princes2", "princes_1.mesh");
		//entPos = Vector3(2408, 303, 3500);
		//rot.FromAngleAxis(Degree(-90), Vector3::UNIT_Y);
		//sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		//sn->setScale(Vector3(5, 5, 5));
		//sn->attachObject(e);
		//mCharaList.push_back(e);
	}
////////////////////////////////////////////////////////////////////////// 
	void createscene()
	{
#pragma region [Create Terrain]
		bool blankTerrain = false;
		//blankTerrain = true;

		mTerrainGlobals = OGRE_NEW TerrainGlobalOptions();

		ResourceGroupManager::getSingleton().createResourceGroup("Terrain");
		ResourceGroupManager::getSingleton().addResourceLocation(mFSLayer->getWritablePath(""), "FileSystem", "Terrain", false, false);

		mEditMarker = mSceneMgr->createEntity("editMarker", "sphere.mesh");
		mEditNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		mEditNode->attachObject(mEditMarker);
		mEditNode->setScale(0.05, 0.05, 0.05);

		setupControls();

		mCameraMan->setTopSpeed(50);

		setDragLook(false);

		MaterialManager::getSingleton().setDefaultTextureFiltering(TFO_ANISOTROPIC);
		MaterialManager::getSingleton().setDefaultAnisotropy(7);

		mSceneMgr->setFog(FOG_LINEAR, ColourValue(0.7, 0.7, 0.8), 0, 10000, 25000);

		LogManager::getSingleton().setLogDetail(LL_BOREME);

		Vector3 lightdir(0.55, -0.3, 0.75);
		lightdir.normalise();


		Light* l = mSceneMgr->createLight("tstLight");
		l->setType(Light::LT_DIRECTIONAL);
		l->setDirection(lightdir);
		l->setDiffuseColour(ColourValue::White);
		l->setSpecularColour(ColourValue(0.4, 0.4, 0.4));

		mSceneMgr->setAmbientLight(ColourValue(0.2, 0.2, 0.2));


		mTerrainGroup = OGRE_NEW TerrainGroup(mSceneMgr, Terrain::ALIGN_X_Z, TERRAIN_SIZE, TERRAIN_WORLD_SIZE);
		mTerrainGroup->setFilenameConvention(TERRAIN_FILE_PREFIX, TERRAIN_FILE_SUFFIX);
		mTerrainGroup->setOrigin(mTerrainPos);
		mTerrainGroup->setResourceGroup("Terrain");

		configureTerrainDefaults(l);
#ifdef PAGING
		// Paging setup
		mPageManager = OGRE_NEW PageManager();
		// Since we're not loading any pages from .page files, we need a way just 
		// to say we've loaded them without them actually being loaded
		mPageManager->setPageProvider(&mDummyPageProvider);
		mPageManager->addCamera(mCamera);
		mTerrainPaging = OGRE_NEW TerrainPaging(mPageManager);
		PagedWorld* world = mPageManager->createWorld();
		mTerrainPaging->createWorldSection(world, mTerrainGroup, 2000, 3000, 
			TERRAIN_PAGE_MIN_X, TERRAIN_PAGE_MIN_Y, 
			TERRAIN_PAGE_MAX_X, TERRAIN_PAGE_MAX_Y);
#else
		for (long x = TERRAIN_PAGE_MIN_X; x <= TERRAIN_PAGE_MAX_X; ++x)
			for (long y = TERRAIN_PAGE_MIN_Y; y <= TERRAIN_PAGE_MAX_Y; ++y)
				defineTerrain(x, y, blankTerrain);
		// sync load since we want everything in place when we start
		mTerrainGroup->loadAllTerrains(true);
#endif

		if (mTerrainsImported)
		{
			TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
			while(ti.hasMoreElements())
			{
				Terrain* t = ti.getNext()->instance;
				initBlendMaps(t);
			}
		}

		mTerrainGroup->freeTemporaryResources();
#pragma endregion [Create Terrain]
		//PTR TuanNA begin comment
		//[Add grass into map- 11/12/2016]
		//createGrassMesh(String grassName, String grassMaterialName)
#pragma region [Create Grass]
		// type grass in enum GrassType
		int id = 1;
		for (id; id <= MAX_GRASS; id++)
		{
			String grassMaterialName = "Examples/GrassBlades" + StringConverter::toString(id);
			String grassName = "grass" + StringConverter::toString(id);
			createGrassMesh(grassName, grassMaterialName);

		}

		//PTR TuanNA [Create Entity- 12/12/2016]
		Entity* grass1 = mSceneMgr->createEntity("Grass1", "grass1");
		Entity* grass2 = mSceneMgr->createEntity("Grass2", "grass2");
		Entity* grass3 = mSceneMgr->createEntity("Grass3", "grass3");
		Entity* grass4 = mSceneMgr->createEntity("Grass4", "grass4");
		Entity* grass5 = mSceneMgr->createEntity("Grass5", "grass5");
		Entity* grass6 = mSceneMgr->createEntity("Grass6", "grass6");
		Entity* grass7 = mSceneMgr->createEntity("Grass7", "grass7");
		Entity* grass8 = mSceneMgr->createEntity("Grass8", "grass8");
		Entity* grass9 = mSceneMgr->createEntity("Grass9", "grass9");
		Entity* grass10 = mSceneMgr->createEntity("Grass10", "grass10");
		Entity* grass11 = mSceneMgr->createEntity("Grass11", "grass11");

		// create a static geometry field, which we will populate with grass
		mField = mSceneMgr->createStaticGeometry("Field");
		mField->setRegionDimensions(Vector3(140, 140, 140));
		mField->setOrigin(Vector3(70, 70, 70));

		// add grass uniformly throughout the field, with some random variations
		for (int x = -280; x < 280; x += 50)
		{
			for (int z = -280; z < 280; z += 50)
			{ 
				Vector3 pos(mTerrainPos.x + 2043 + x, 0, mTerrainPos.z + 1715 + z);
				//Set heigh for grass always on surface
				pos.y = mTerrainGroup->getHeightAtWorldPosition(pos) + mTerrainPos.y;
				Quaternion ori(Degree(Math::RangeRandom(0, 359)), Vector3::UNIT_Y);
				Vector3 scale(0.5, Math::RangeRandom(0.42, 0.6), 0.5);

				mField->addEntity(grass1, pos, ori, scale);
			}
		}

		//Max grass in group
		int groupGrass = MAX_GROUP_GRASS;

		// add grass uniformly throughout the field, with some random variations
		for (int x = -TERRAIN_WORLD_SIZE/2; x < TERRAIN_WORLD_SIZE/2; x += Math::RangeRandom(200, 500))
		{
			for (int z = -TERRAIN_WORLD_SIZE/2; z < TERRAIN_WORLD_SIZE/2; z += Math::RangeRandom(200, 500))
			{
				Vector3 pos(mTerrainPos.x + Math::RangeRandom(-50, 50) + x + 50, 0, mTerrainPos.z + Math::RangeRandom(-50, 50) + z + 50);
				//Set heigh for grass always on surface
				pos.y = mTerrainGroup->getHeightAtWorldPosition(pos) + mTerrainPos.y;
				Quaternion ori(Degree(Math::RangeRandom(0, 359)), Vector3::UNIT_Y);
				int scaleGrass = Math::RangeRandom(0.5, 2);
				Vector3 scale(1*scaleGrass, Math::RangeRandom(0.85, 1.15)*scaleGrass, 1*scaleGrass);

				if (groupGrass > 0)
				{
					switch (id)
					{
					case GRASS_RED_FLOWER:
						mField->addEntity(grass1, pos, ori, scale);
						break;
					case GRASS_YELLOW_FLOWER:
						mField->addEntity(grass2, pos, ori, scale);
						break;
					case GRASS_ALOE_VERA:
						mField->addEntity(grass3, pos, ori, scale);
						break;
					case GRASS_DIEP:
						mField->addEntity(grass4, pos, ori, scale);
						break;
					case GRASS_DAO_FLOWER:
						mField->addEntity(grass5, pos, ori, scale);
						break;
					case GRASS_ANH_TUC:
						mField->addEntity(grass6, pos, ori, scale);
						break;
					case GRASS_MAI:
						mField->addEntity(grass7, pos, ori, scale);
						break;
					case GRASS_GANG:
						mField->addEntity(grass8, pos, ori, scale);
						break;
					case GRASS_LAU:
						mField->addEntity(grass9, pos, ori, scale);
						break;
					case GRASS_LAU_2:
						mField->addEntity(grass10, pos, ori, scale);
						break;
					case GRASS_SAN_HO:
						mField->addEntity(grass11, pos, ori, scale);
						break;
					}
					groupGrass--;
				} 
				else
				{
					id = Math::RangeRandom(0, 11);
					groupGrass = MAX_GROUP_GRASS;
				}
			}
		}

		mField->build();  // build our static geometry (bake the grass into it)
#pragma endregion [Create Grass]
		//PTR TuanNA end comment

#pragma region [Create House]
		// create a few entities on the terrain
		e = mSceneMgr->createEntity("tudorhouse.mesh");
		entPos = Vector3(mTerrainPos.x + 1850, 0, mTerrainPos.z + 1478);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5 + mTerrainPos.y;
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.12, 0.12, 0.12));
		sn->attachObject(e);
		mHouseList.push_back(e);

		//Banian-tree0
		e = mSceneMgr->createEntity("Banian-tree0", "tree.09.mesh");
		entPos = Vector3(-2000, 0, 2000);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 250;
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(1, 1, 1));
		sn->attachObject(e);
		mBanianTreeList.push_back(e);

		//Temple house
		e = mSceneMgr->createEntity("Temple-house", "highlanderhouse.01.mesh");
		entPos = Vector3(1728, 0, 7036);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		rot.FromAngleAxis(Degree(90), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(5, 5, 5));
		sn->attachObject(e);

		//Banian-tree1
		e = mSceneMgr->createEntity("Banian-tree1", "tree.09.mesh");
		entPos = Vector3(1889, 0, 7213);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 125;
		rot.FromAngleAxis(Degree(90), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(0.5, 0.5, 0.5));
		sn->attachObject(e);
		mBanianTreeList.push_back(e);

		e1 = mSceneMgr->createEntity("RiceCooker1", "RiceCooker.mesh");
		entPos = Vector3(1728 + 122, 0, 7036 + 164 + 30);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e1);
		mCharaList.push_back(e1);

		// Create the cathedral - this will be the static scene center map 87/10/2774
		e = mSceneMgr->createEntity("Cathedral", "sibenik.mesh");
		entPos = Vector3(4500, 0, 4500);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) - 5;
		rot.FromAngleAxis(Degree(-135), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(50, 50, 50));
		sn->attachObject(e);

		mHouseList.push_back(e);

		//House right back map 7000/10/-1000
		e = mSceneMgr->createEntity("tudorhouse.mesh");
		entPos = Vector3(6900, 0, -900);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5;
		//rot.FromAngleAxis(Degree(Math::RangeRandom(-180, 180)), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos/*, rot*/);
		sn->setScale(Vector3(0.12, 0.12, 0.12));
		sn->attachObject(e);
		mHouseList.push_back(e);

		//Banian-tree2
		e = mSceneMgr->createEntity("Banian-tree2", "tree.09.mesh");
		entPos = Vector3(6900 + 50, 0, -900 + 84);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 125;
		rot.FromAngleAxis(Degree(90), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(0.5, 0.5, 0.5));
		sn->attachObject(e);
		mBanianTreeList.push_back(e);

		e2 = mSceneMgr->createEntity("RiceCooker2", "RiceCooker.mesh");
		entPos = Vector3(6950, 0, -748);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e2);
		mCharaList.push_back(e2);

		//House right back map 7000/10/11000
		e = mSceneMgr->createEntity("tudorhouse.mesh");
		entPos = Vector3(6900, 0, 10900);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5;
		rot.FromAngleAxis(Degree(180), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(0.12, 0.12, 0.12));
		sn->attachObject(e);
		mHouseList.push_back(e);

		//Banian-tree3
		e = mSceneMgr->createEntity("Banian-tree3", "tree.09.mesh");
		entPos = Vector3(6943, 0, 10940);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 125;
		rot.FromAngleAxis(Degree(90), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(0.5, 0.5, 0.5));
		sn->attachObject(e);
		mBanianTreeList.push_back(e);

		e3 = mSceneMgr->createEntity("RiceCooker3", "RiceCooker.mesh");
		entPos = Vector3(6900 + 22, 0, 10900 + 64 + 30);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e3);
		mCharaList.push_back(e3);

		//House left front map -5000/10/11000
		e = mSceneMgr->createEntity("tudorhouse.mesh");
		entPos = Vector3(-4900, 0, 10900);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 65.5;
		rot.FromAngleAxis(Degree(180), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(0.12, 0.12, 0.12));
		sn->attachObject(e);
		mHouseList.push_back(e);

		//Banian-tree4
		e = mSceneMgr->createEntity("Banian-tree4", "tree.09.mesh");
		entPos = Vector3(-4839, 0, 10966);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos) + 125;
		rot.FromAngleAxis(Degree(90), Vector3::UNIT_Y);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos, rot);
		sn->setScale(Vector3(0.5, 0.5, 0.5));
		sn->attachObject(e);
		mBanianTreeList.push_back(e);
		e4 = mSceneMgr->createEntity("RiceCooker4", "RiceCooker.mesh");
		entPos = Vector3(-4900 + 22, 0, 10900 + 64 + 30);
		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
		sn->setScale(Vector3(0.1, 0.1, 0.1));
		sn->attachObject(e4);
		mCharaList.push_back(e4);
#pragma endregion [Create House]
#pragma region [Create Skybox]		
		mSceneMgr->setSkyBox(true, "Examples/CloudyNoonSkyBox");//Blue Sky- Morning
#pragma endregion [Create Skybox]
// #pragma region [Create Ocean]
// 		loadAllMaterialControlFiles(mMaterialControlsContainer);
// 		//Define a plane mesh that will be used for the ocean surface
// 		Ogre::Plane oceanSurface;
// 		oceanSurface.normal = Ogre::Vector3::UNIT_Y;
// 		oceanSurface.d = 20;
// 		Ogre::MeshManager::getSingleton().createPlane("OceanSurface",
// 			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
// 			oceanSurface,
// 			TERRAIN_WORLD_SIZE*2, TERRAIN_WORLD_SIZE*2, 50, 50, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
// 
// 		Ogre::Entity*         mOceanSurfaceEnt;
// 		mOceanSurfaceEnt = mSceneMgr->createEntity( "OceanSurface", "OceanSurface" );
// 		mOceanSurfaceEnt->setMaterialName(mMaterialControlsContainer[0].getMaterialName());
// 		entPos = Vector3(1000, 0, 5000);
// 		entPos.y = mTerrainGroup->getHeightAtWorldPosition(entPos);
// 		sn = mSceneMgr->getRootSceneNode()->createChildSceneNode(entPos);
// 		sn->attachObject(mOceanSurfaceEnt);
// #pragma endregion [Create Ocean]
	}
//////////////////////////////////////////////////////////////////////////
	void createGrassMesh(String grassName, String grassMaterialName)
	{
		MeshPtr mesh = MeshManager::getSingleton().createManual(grassName, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

		// create a submesh with the grass material
		SubMesh* sm = mesh->createSubMesh();
		sm->setMaterialName(grassMaterialName);//PTR TuanNA [grassMaterialName = "Examples/GrassBlades"- 11/12/2016]
		sm->useSharedVertices = false;
		sm->vertexData = OGRE_NEW VertexData();
		sm->vertexData->vertexStart = 0;
		sm->vertexData->vertexCount = 12;
		sm->indexData->indexCount = 18;

#if defined(INCLUDE_RTSHADER_SYSTEM)
		MaterialPtr grassMat = MaterialManager::getSingleton().getByName(grassMaterialName);
		grassMat->getTechnique(0)->setSchemeName(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
#endif

		// specify a vertex format declaration for our mesh: 3 floats for position, 3 floats for normal, 2 floats for UV
		VertexDeclaration* decl = sm->vertexData->vertexDeclaration;
		decl->addElement(0, 0, VET_FLOAT3, VES_POSITION);
		decl->addElement(0, sizeof(float) * 3, VET_FLOAT3, VES_NORMAL);
		decl->addElement(0, sizeof(float) * 6, VET_FLOAT2, VES_TEXTURE_COORDINATES, 0);

		// create a vertex buffer
		HardwareVertexBufferSharedPtr vb = HardwareBufferManager::getSingleton().createVertexBuffer
			(decl->getVertexSize(0), sm->vertexData->vertexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		GrassVertex* verts = (GrassVertex*)vb->lock(HardwareBuffer::HBL_DISCARD);  // start filling in vertex data

		for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
		{
			// planes intersect along the Y axis with 60 degrees between them
			Real x = Math::Cos(Degree(i * 60)) * GRASS_WIDTH / 2;
			Real z = Math::Sin(Degree(i * 60)) * GRASS_WIDTH / 2;

			for (unsigned int j = 0; j < 4; j++)  // each plane has 4 vertices
			{
				GrassVertex& vert = verts[i * 4 + j];

				vert.x = j < 2 ? -x : x;
				vert.y = j % 2 ? 0 : GRASS_HEIGHT;
				vert.z = j < 2 ? -z : z;

				// all normals point straight up
				vert.nx = 0;
				vert.ny = 1;
				vert.nz = 0;

				vert.u = j < 2 ? 0 : 1;
				vert.v = j % 2;
			}
		}

		vb->unlock();  // commit vertex changes

		sm->vertexData->vertexBufferBinding->setBinding(0, vb);  // bind vertex buffer to our submesh

		// create an index buffer
		sm->indexData->indexBuffer = HardwareBufferManager::getSingleton().createIndexBuffer
			(HardwareIndexBuffer::IT_16BIT, sm->indexData->indexCount, HardwareBuffer::HBU_STATIC_WRITE_ONLY);

		// start filling in index data
		Ogre::uint16* indices = (Ogre::uint16*)sm->indexData->indexBuffer->lock(HardwareBuffer::HBL_DISCARD);

		for (unsigned int i = 0; i < 3; i++)  // each grass mesh consists of 3 planes
		{
			unsigned int off = i * 4;  // each plane consists of 2 triangles

			*indices++ = 0 + off;
			*indices++ = 3 + off;
			*indices++ = 1 + off;

			*indices++ = 0 + off;
			*indices++ = 2 + off;
			*indices++ = 3 + off;
		}

		sm->indexData->indexBuffer->unlock();  // commit index changes
	}

	//PTR TuanNA begin comment
	//[Add grass into map- 11/12/2016]
	void waveGrass(Real timeElapsed)
	{
		static Real xinc = Math::PI * 0.3;
		static Real zinc = Math::PI * 0.44;
		static Real xpos = Math::RangeRandom(-Math::PI, Math::PI);
		static Real zpos = Math::RangeRandom(-Math::PI, Math::PI);
		static Vector4 offset(0, 0, 0, 0);

		xpos += xinc * timeElapsed;
		zpos += zinc * timeElapsed;

		// update vertex program parameters by binding a value to each renderable
		StaticGeometry::RegionIterator regs =  mField->getRegionIterator();
		while (regs.hasMoreElements())
		{
			StaticGeometry::Region* reg = regs.getNext();

			// a little randomness
			xpos += reg->getCentre().x * 0.001;
			zpos += reg->getCentre().z * 0.001;
			offset.x = Math::Sin(xpos) * 4;
			offset.z = Math::Sin(zpos) * 4;

			StaticGeometry::Region::LODIterator lods = reg->getLODIterator();
			while (lods.hasMoreElements())
			{
				StaticGeometry::LODBucket::MaterialIterator mats = lods.getNext()->getMaterialIterator();
				while (mats.hasMoreElements())
				{
					StaticGeometry::MaterialBucket::GeometryIterator geoms = mats.getNext()->getGeometryIterator();
					while (geoms.hasMoreElements()) geoms.getNext()->setCustomParameter(999, offset);
				}
			}
		}
	}
	//PTR TuanNA end comment
//////////////////////////////////////////////////////////////////////////
	void _shutdown()
	{
		if (mTerrainPaging)
		{
			OGRE_DELETE mTerrainPaging;
			mTerrainPaging = 0;
			OGRE_DELETE mPageManager;
			mPageManager = 0;
		}
		else if(mTerrainGroup)
		{
			OGRE_DELETE mTerrainGroup;
			mTerrainGroup = 0;
		}

		OGRE_DELETE mTerrainGlobals;
		mTerrainGlobals = 0;
		mFly = 0;
		mActiveChara = true;
		mWinChara = false;
		mFinal = true;
		mHPBot = 100;

		ResourceGroupManager::getSingleton().destroyResourceGroup("Terrain");

		mHouseList.clear();
		mBanianTreeList.clear();

		mCharaList.clear();

		SdkGame::_shutdown();
	}
//////////////////////////////////////////////////////////////////////////
	//PTR TuanNA begin comment
	//[Clean up Grass and Ocean Surface- 11/12/2016]
	void cleanupContent()
	{
		//PTR TuanNA [Remove OceanSurface- 3/1/2017]
		MeshManager::getSingleton().remove("OceanSurface");

		//PTR TuanNA [Remove Grass- 3/1/2017]
		for (int id = 1; id <= MAX_GRASS; id++)
		{
			String grassName = "grass" + StringConverter::toString(id);
			MeshManager::getSingleton().remove(grassName);
		}

		// clean up character controller and the floor mesh
		if (mChara)
		{
			delete mChara;
			mChara = 0;
		}

		engine->drop(); // delete engine
	}
	//PTR TuanNA end comment

	//Init Main Character
	SinbadCharacterController* mChara;
	Slider* mHPSlider;
	Slider* mHPBotSlider;
	int mHPBot;
	Entity* e;
	Vector3 entPos;
	Quaternion rot;
	SceneNode* sn;
	bool mMute;
	bool mRun[4];
	bool mWeapon;
	bool mActiveChara;
	bool mWinChara;
	bool mFinal;
	Entity* e0; 
	Entity* e1; 
	Entity* e2; 
	Entity* e3; 
	Entity* e4;
};

#endif
