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
| Filename   : GameBrowser.h                                                           
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
| Create File: 21:29:20 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:29:20 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#ifndef __GameBrowser_H__
#define __GameBrowser_H__

#include "GameContext.h"
#include "GamePlugin.h"
#include "SdkTrays.h"

#include "../res/Resource.h" // PTR TuanNA [Add include- 10/7/2016]

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#include "macUtils.h"
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WINRT
// For WinRT we only support running from the cache file.
#       define ENABLE_SHADERS_CACHE_LOAD 1
#endif

#define ENABLE_SHADERS_CACHE_SAVE 0
#define ENABLE_SHADERS_CACHE_LOAD 0

#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
#include <android_native_app_glue.h>
#include "Android/OgreAPKFileSystemArchive.h"
#include "Android/OgreAPKZipArchive.h"
#endif

#ifdef OGRE_STATIC_LIB
#   ifdef OGRE_BUILD_PLUGIN_BSP
#       include "BSP.h"
#   endif
#   ifdef INCLUDE_RTSHADER_SYSTEM
#       include "ShaderSystem.h"
#   endif
#	include "DualQuaternion.h"
#   include "DeferredShadingDemo.h"
#   include "Instancing.h"
#   include "NewInstancing.h"
#   include "TextureArray.h"
#   include "SSAO.h"
#   include "OceanDemo.h"
#	ifdef OGRE_BUILD_COMPONENT_VOLUME
#		include "VolumeCSG.h"
#		include "VolumeTerrain.h"
#	endif
#   ifdef OGRE_BUILD_COMPONENT_TERRAIN
#       include "EndlessWorld.h"
#       include "Terrain.h"
#   endif
#   include "CelShading.h"
#   include "Compositor.h"
#   include "CubeMapping.h"
#   include "Dot3Bump.h"
#   include "Fresnel.h"
#   include "Water.h"
//#   include "AtomicCounters.h"
#   include "BezierPatch.h"
#   include "CameraTrack.h"
#   include "CharacterGame.h"
#   include "DynTex.h"
#   include "FacialAnimation.h"
#   include "Grass.h"
#   include "Lighting.h"
#   include "MeshLod.h"
#   include "ParticleFX.h"
#	include "PNTrianglesTessellation.h"
#   include "Shadows.h"
#   include "SkeletalAnimation.h"
#   include "SkyBox.h"
#   include "SkyDome.h"
#   include "SkyPlane.h"
#   include "Smoke.h"
#   include "SphereMapping.h"
#	include "Tesselation.h"
#   include "TextureFX.h"
#   include "Transparency.h"
#   if GAMES_INCLUDE_PLAYPEN
#    include "PlayPen.h"
#    include "PlayPenTestPlugin.h"
     PlayPenPlugin* playPenPlugin = 0;
     PlaypenTestPlugin* playPenTestPlugin = 0;
#   endif
#   ifdef INCLUDE_RTSHADER_SYSTEM
#       include "OgreRTShaderSystem.h"
// Remove the comment below in order to make the RTSS use valid path for writing down the generated shaders.
// If cache path is not set - all shaders are generated to system memory.
//#define _RTSS_WRITE_SHADERS_TO_DISK
#   endif // INCLUDE_RTSHADER_SYSTEM
typedef std::map<String, OgreBites::SdkGame *> PluginMap;
#endif // OGRE_STATIC_LIB

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
#   ifdef __OBJC__
#       import <UIKit/UIKit.h>
#   endif

namespace OgreBites
{
    class GameBrowser;
}

@interface GameBrowserGestureView : UIView
{
    OgreBites::GameBrowser *mBrowser;
}
@property (assign) OgreBites::GameBrowser *mBrowser;

@end
#endif

namespace OgreBites
{
#ifdef INCLUDE_RTSHADER_SYSTEM

/** This class demonstrates basic usage of the RTShader system.
It sub class the material manager listener class and when a target scheme callback
is invoked with the shader generator scheme it tries to create an equivalent shader
based technique based on the default technique of the given material.
*/
class ShaderGeneratorTechniqueResolverListener : public Ogre::MaterialManager::Listener
{
public:

	ShaderGeneratorTechniqueResolverListener(Ogre::RTShader::ShaderGenerator* pShaderGenerator)
	{
		mShaderGenerator = pShaderGenerator;			
	}

	/** This is the hook point where shader based technique will be created.
	It will be called whenever the material manager won't find appropriate technique
	that satisfy the target scheme name. If the scheme name is out target RT Shader System
	scheme name we will try to create shader generated technique for it. 
	*/
	virtual Ogre::Technique* handleSchemeNotFound(unsigned short schemeIndex, 
		const Ogre::String& schemeName, Ogre::Material* originalMaterial, unsigned short lodIndex, 
		const Ogre::Renderable* rend)
	{	
		Ogre::Technique* generatedTech = NULL;

		// Case this is the default shader generator scheme.
		if (schemeName == Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME)
		{
			bool techniqueCreated;

			// Create shader generated technique for this material.
			techniqueCreated = mShaderGenerator->createShaderBasedTechnique(
				originalMaterial->getName(), 
				Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
				schemeName);	

			// Case technique registration succeeded.
			if (techniqueCreated)
			{
				// Force creating the shaders for the generated technique.
				mShaderGenerator->validateMaterial(schemeName, originalMaterial->getName());
				
				// Grab the generated technique.
				Ogre::Material::TechniqueIterator itTech = originalMaterial->getTechniqueIterator();

				while (itTech.hasMoreElements())
				{
					Ogre::Technique* curTech = itTech.getNext();

					if (curTech->getSchemeName() == schemeName)
					{
						generatedTech = curTech;
						break;
					}
				}				
			}
		}

		return generatedTech;
	}

protected:	
	Ogre::RTShader::ShaderGenerator*	mShaderGenerator;			// The shader generator instance.		
};
#endif // INCLUDE_RTSHADER_SYSTEM


	/*=============================================================================
	| The OGRE Game Browser. Features a menu accessible from all Games,
	| dynamic configuration, resource reloading, node labeling, and more.
	=============================================================================*/
	class GameBrowser : public GameContext, public SdkTrayListener
	{
	public:

		GameBrowser(bool nograb = false, int startGameIndex = -1) : GameContext()
		{
            mIsShuttingDown = false;
            mNoGrabInput = nograb;
			mTrayMgr = 0;
			mLastViewCategory = 0;
			mLastViewTitle = 0;
			mLastGameIndex = -1;
            mStartGameIndex = startGameIndex;
			mCategoryMenu = 0;
			mGameMenu = 0;
			mGameSlider = 0;
			mTitleLabel = 0;
			mDescBox = 0;
			mRendererMenu = 0;
			mCarouselPlace = 0.0f;
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
            mNaClInstance = 0;
            mNaClSwapCallback = 0;
            mOisFactory = 0;
            mInitWidth = 0;
            mInitHeight = 0;
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
			mGestureView = 0;
#endif
#ifdef INCLUDE_RTSHADER_SYSTEM
			mShaderGenerator	 = NULL;		
			mMaterialMgrListener = NULL;
#endif // INCLUDE_RTSHADER_SYSTEM
		}

		/*-----------------------------------------------------------------------------
		| init data members needed only by WinRT
		-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
		void initAppForWinRT( Windows::UI::Core::CoreWindow^ nativeWindow, InputContext inputContext)
		{
			mNativeWindow = nativeWindow;
#   if (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
			mNativeControl = nullptr;
#	endif // (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
			mInputContext = inputContext;
		}
#	if (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
		void initAppForWinRT( Windows::UI::Xaml::Shapes::Rectangle ^ nativeControl, InputContext inputContext)
		{
			mNativeWindow = nullptr;
			mNativeControl = nativeControl;
			mInputContext = inputContext;
		}
#	endif // (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
#endif // (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
		/*-----------------------------------------------------------------------------
		| init data members needed only by NaCl
		-----------------------------------------------------------------------------*/
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
		void initAppForNaCl( pp::Instance* naClInstance, pp::CompletionCallback* naClSwapCallback, OIS::FactoryCreator * oisFactory, Ogre::uint32 initWidth, Ogre::uint32 initHeight )
		{
            mNaClInstance = naClInstance;
            mNaClSwapCallback = naClSwapCallback;
            mOisFactory = oisFactory;
            mInitWidth = initWidth;
            mInitHeight = initHeight;
		}

        void createInputDevices()
        {
            mInputMgr->addFactoryCreator(mOisFactory);
            GameContext::createInputDevices();
        }
#endif

		/*-----------------------------------------------------------------------------
		| init pre-created window for android
		-----------------------------------------------------------------------------*/
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
		void initAppForAndroid(Ogre::RenderWindow *window, struct android_app* app, OIS::MultiTouch *mouse, OIS::Keyboard *keyboard)
		{
			mWindow = window;
			mInputContext.mMultiTouch = mouse;
			mInputContext.mKeyboard = keyboard;
            
            if(app != NULL)
            {
                mAssetMgr = app->activity->assetManager;
                Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKFileSystemArchiveFactory(app->activity->assetManager) );
                Ogre::ArchiveManager::getSingleton().addArchiveFactory( new Ogre::APKZipArchiveFactory(app->activity->assetManager) );
            }
		}
#endif

        virtual void loadStartUpGame()
        {
            if (mStartGameIndex != -1)
            {
                runGameByIndex(mStartGameIndex);
                mStartGameIndex = -1;
            }
        }

        virtual void runGameByIndex(int idx)
        {
            runGame(Ogre::any_cast<Game*>(mThumbs[idx]->getUserAny()));
        }

		/*-----------------------------------------------------------------------------
		| Extends runGame to handle creation and destruction of dummy scene.
		-----------------------------------------------------------------------------*/
		virtual void runGame(Game* s)
		{
			if (mCurrentGame)  // Game quitting
			{
#ifdef INCLUDE_RTSHADER_SYSTEM
                mShaderGenerator->removeAllShaderBasedTechniques(); // clear techniques from the RTSS
#endif
				mCurrentGame->_shutdown();
				mCurrentGame = 0;
				mGamePaused = false;     // don't pause next Game

#ifdef INCLUDE_RTSHADER_SYSTEM
                if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION))
                {
                    destroyDummyScene();
                    destroyRTShaderSystem();
                }
#endif

				// create dummy scene and modify controls
				createDummyScene();
				mTrayMgr->showBackdrop("SdkTrays/Bands");
				mTrayMgr->showAll();
				((Button*)mTrayMgr->getWidget("StartStop"))->setCaption("Start Game"); //PTR TuanNA [Change label button- 3/11/2016]
			}

			if (s)  // Game starting
			{
				// destroy dummy scene and modify controls
				((Button*)mTrayMgr->getWidget("StartStop"))->setCaption("Exit Game"); //PTR TuanNA [Change label button- 3/11/2016]
				mTrayMgr->showBackdrop("SdkTrays/Shade");
				mTrayMgr->hideAll();
				destroyDummyScene();

				try
				{
#ifdef INCLUDE_RTSHADER_SYSTEM
                    if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION))
                    {
                        createDummyScene();
                    }

					s->setShaderGenerator(mShaderGenerator);
#endif
					GameContext::runGame(s);
				}
				catch (Ogre::Exception e)   // if failed to start, show error and fall back to menu
				{
                    destroyDummyScene();

					s->_shutdown();

					createDummyScene();
					mTrayMgr->showBackdrop("SdkTrays/Bands");
					mTrayMgr->showAll();
					((Button*)mTrayMgr->getWidget("StartStop"))->setCaption("Start Game"); //PTR TuanNA [Change label button- 3/11/2016]

					mTrayMgr->showOkDialog("Error!", e.getDescription() + "\nSource: " + e.getSource());
				}
			}
		}

		/*-----------------------------------------------------------------------------
		| Extends frameRenderingQueued to update tray manager and carousel.
		-----------------------------------------------------------------------------*/
		bool frameRenderingQueued(const Ogre::FrameEvent& evt)
		{
			// don't do all these calculations when Game's running or when in configuration screen or when no Games loaded
			if (!mLoadedGames.empty() && mTitleLabel->getTrayLocation() != TL_NONE && (!mCurrentGame || mGamePaused))
			{
				// makes the carousel spin smoothly toward its right position
				Ogre::Real carouselOffset = mGameMenu->getSelectionIndex() - mCarouselPlace;
				if ((carouselOffset <= 0.001) && (carouselOffset >= -0.001)) mCarouselPlace = mGameMenu->getSelectionIndex();
				else mCarouselPlace += carouselOffset * Ogre::Math::Clamp<Ogre::Real>(evt.timeSinceLastFrame * 15.0, -1.0, 1.0);

				// update the thumbnail positions based on carousel state
				for (int i = 0; i < (int)mThumbs.size(); i++)
				{
					Ogre::Real thumbOffset = mCarouselPlace - i;
					Ogre::Real phase = (thumbOffset / 2.0) - 2.8;

					if (thumbOffset < -5 || thumbOffset > 4)    // prevent thumbnails from wrapping around in a circle
					{
						mThumbs[i]->hide();
						continue;
					}
					else mThumbs[i]->show();

					Ogre::Real left = Ogre::Math::Cos(phase) * 200.0;
					Ogre::Real top = Ogre::Math::Sin(phase) * 200.0;
					Ogre::Real scale = 1.0 / Ogre::Math::Pow((Ogre::Math::Abs(thumbOffset) + 1.0), 0.75);

					Ogre::BorderPanelOverlayElement* frame =
						(Ogre::BorderPanelOverlayElement*)mThumbs[i]->getChildIterator().getNext();

					mThumbs[i]->setDimensions(128.0 * scale, 96.0 * scale);
					frame->setDimensions(mThumbs[i]->getWidth() + 16.0, mThumbs[i]->getHeight() + 16.0);
					mThumbs[i]->setPosition((int)(left - 80.0 - (mThumbs[i]->getWidth() / 2.0)),
						(int)(top - 5.0 - (mThumbs[i]->getHeight() / 2.0)));

					if (i == mGameMenu->getSelectionIndex()) frame->setBorderMaterialName("SdkTrays/Frame/Over");
					else frame->setBorderMaterialName("SdkTrays/Frame");
				}
			}

			mTrayMgr->frameRenderingQueued(evt);

			try
			{
				return GameContext::frameRenderingQueued(evt);
			}
			catch (Ogre::Exception e)   // show error and fall back to menu
			{
				runGame(0);
				mTrayMgr->showOkDialog("Error!", e.getDescription() + "\nSource: " + e.getSource());
			}

			return true;
		}

		/*-----------------------------------------------------------------------------
		| Handles confirmation dialog responses.
		-----------------------------------------------------------------------------*/
		virtual void yesNoDialogClosed(const Ogre::DisplayString& question, bool yesHit)
		{
			if (question.substr(0, 14) == "This will stop" && yesHit)   // confirm unloading of Games
			{
				runGame(0);
				buttonHit((Button*)mTrayMgr->getWidget("UnloadReload"));
			}
		}

		/*-----------------------------------------------------------------------------
		| Handles button widget events.
		-----------------------------------------------------------------------------*/
		virtual void buttonHit(Button* b)
		{
			if (b->getName() == "StartStop")   // start or stop Game
			{
				if (b->getCaption() == "Start Game") //PTR TuanNA [Change Label button- 3/11/2016]
				{
					if (mLoadedGames.empty()) mTrayMgr->showOkDialog("Error!", "No Game selected!");
					// use the Game pointer we stored inside the thumbnail
					else runGame(Ogre::any_cast<Game*>(mThumbs[mGameMenu->getSelectionIndex()]->getUserAny()));
				}
				else runGame(0);
			}
			else if (b->getName() == "UnloadReload")   // unload or reload Game plugins and update controls
			{
				if (b->getCaption() == "Disconnect Game") //PTR TuanNA [Change Label button- 3/11/2016]
				{
					if (mCurrentGame) mTrayMgr->showYesNoDialog("Warning!", "This will stop the current Game. Unload anyway?");
					else
					{
						// save off current view and try to restore it on the next reload
						mLastViewTitle = mGameMenu->getSelectionIndex();
						mLastViewCategory = mCategoryMenu->getSelectionIndex();

						unloadGames();
						populateGameMenus();
						b->setCaption("Connect Game");//PTR TuanNA [Change Label button- 3/11/2016]
					}
				}
				else
				{
					loadGames();
					populateGameMenus();
					if (!mLoadedGames.empty()) b->setCaption("Disconnect Game"); //PTR TuanNA [Change Label button- 3/11/2016]

					try  // attempt to restore the last view before unloading Games
					{
						mCategoryMenu->selectItem(mLastViewCategory);
						mGameMenu->selectItem(mLastViewTitle);
					}
					catch (Ogre::Exception e) {}
				}
			}
			else if (b->getName() == "Configure")   // enter configuration screen
			{
				mTrayMgr->removeWidgetFromTray("StartStop");
				mTrayMgr->removeWidgetFromTray("UnloadReload");
				mTrayMgr->removeWidgetFromTray("Configure");
				mTrayMgr->removeWidgetFromTray("Quit");
				mTrayMgr->moveWidgetToTray("Apply", TL_RIGHT);
				mTrayMgr->moveWidgetToTray("Back", TL_RIGHT);

				for (unsigned int i = 0; i < mThumbs.size(); i++)
				{
					mThumbs[i]->hide();
				}

				while (mTrayMgr->getTrayContainer(TL_CENTER)->isVisible())
				{
					mTrayMgr->removeWidgetFromTray(TL_CENTER, 0);
				}

				while (mTrayMgr->getTrayContainer(TL_LEFT)->isVisible())
				{
					mTrayMgr->removeWidgetFromTray(TL_LEFT, 0);
				}

				mTrayMgr->moveWidgetToTray("ConfigLabel", TL_LEFT);
				mTrayMgr->moveWidgetToTray(mRendererMenu, TL_LEFT);
				mTrayMgr->moveWidgetToTray("ConfigSeparator", TL_LEFT);

				mRendererMenu->selectItem(mRoot->getRenderSystem()->getName());

				windowResized(mWindow);
			}
			else if (b->getName() == "Back")   // leave configuration screen
			{
				while (mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()) > 3)
				{
					mTrayMgr->destroyWidget(mRendererMenu->getTrayLocation(), 3);
				}

				while (mTrayMgr->getNumWidgets(TL_NONE) != 0)
				{
					mTrayMgr->moveWidgetToTray(TL_NONE, 0, TL_LEFT);
				}

				mTrayMgr->removeWidgetFromTray("Apply");
				mTrayMgr->removeWidgetFromTray("Back");
				mTrayMgr->removeWidgetFromTray("ConfigLabel");
				mTrayMgr->removeWidgetFromTray(mRendererMenu);
				mTrayMgr->removeWidgetFromTray("ConfigSeparator");

				mTrayMgr->moveWidgetToTray("StartStop", TL_RIGHT);
				mTrayMgr->moveWidgetToTray("UnloadReload", TL_RIGHT);
				mTrayMgr->moveWidgetToTray("Configure", TL_RIGHT);
				mTrayMgr->moveWidgetToTray("Quit", TL_RIGHT);

				windowResized(mWindow);
			}
			else if (b->getName() == "Apply")   // apply any changes made in the configuration screen
			{
				bool reset = false;

				Ogre::ConfigOptionMap& options =
					mRoot->getRenderSystemByName(mRendererMenu->getSelectedItem())->getConfigOptions();

				Ogre::NameValuePairList newOptions;

				// collect new settings and decide if a reset is needed

				if (mRendererMenu->getSelectedItem() != mRoot->getRenderSystem()->getName()) reset = true;

				for (unsigned int i = 3; i < mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()); i++)
				{
					SelectMenu* menu = (SelectMenu*)mTrayMgr->getWidget(mRendererMenu->getTrayLocation(), i);
					if (menu->getSelectedItem() != options[menu->getCaption()].currentValue) reset = true;
					newOptions[menu->getCaption()] = menu->getSelectedItem();
				}

				// reset with new settings if necessary
				if (reset) reconfigure(mRendererMenu->getSelectedItem(), newOptions);
			}
			else
            {
                mRoot->queueEndRendering();   // exit browser

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE) && __LP64__
                // Set the shutting down flag and sleep a bit so the displaylink thread can shut itself down
                // Note: It is essential that you yield to the CVDisplayLink thread. Otherwise it will 
                // continue to run which will result in either a crash or kernel panic.
                mIsShuttingDown = true;
                struct timespec ts;
                ts.tv_sec = 0;
                ts.tv_nsec = 1000;
                nanosleep(&ts, NULL);

				mRoot->saveConfig();
				shutdown();
				if (mRoot)
				{
					OGRE_DELETE mOverlaySystem;
					OGRE_DELETE mRoot;
				}
#endif
            }
		}

		/*-----------------------------------------------------------------------------
		| Handles menu item selection changes.
		-----------------------------------------------------------------------------*/
		virtual void itemSelected(SelectMenu* menu)
		{
			if (menu == mCategoryMenu)      // category changed, so update the Game menu, carousel, and slider
			{
				for (unsigned int i = 0; i < mThumbs.size(); i++)    // destroy all thumbnails in carousel
				{
					Ogre::MaterialManager::getSingleton().remove(mThumbs[i]->getName());
					Widget::nukeOverlayElement(mThumbs[i]);
				}
				mThumbs.clear();

				Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();
				Ogre::String selectedCategory;

				if (menu->getSelectionIndex() != -1) selectedCategory = menu->getSelectedItem();
				else
				{
					mTitleLabel->setCaption("");
					mDescBox->setText("");
				}

				bool all = selectedCategory == "All";
				Ogre::StringVector GameTitles;
				Ogre::MaterialPtr templateMat = Ogre::MaterialManager::getSingleton().getByName("SdkTrays/GameThumbnail");

				// populate the Game menu and carousel with filtered Games
				for (GameSet::iterator i = mLoadedGames.begin(); i != mLoadedGames.end(); i++)
				{
					Ogre::NameValuePairList& info = (*i)->getInfo();

					if (all || info["Category"] == selectedCategory)
					{
						Ogre::String name = "SdkTrays/GameThumb" + Ogre::StringConverter::toString(GameTitles.size() + 1);

						// clone a new material for Game thumbnail
						Ogre::MaterialPtr newMat = templateMat->clone(name);

						Ogre::TextureUnitState* tus = newMat->getTechnique(0)->getPass(0)->getTextureUnitState(0);
						if (Ogre::ResourceGroupManager::getSingleton().resourceExists("Essential", info["Thumbnail"]))
							tus->setTextureName(info["Thumbnail"]);
						else 
                            tus->setTextureName("thumb_error.png");

						// create Game thumbnail overlay
						Ogre::BorderPanelOverlayElement* bp = (Ogre::BorderPanelOverlayElement*)
							om.createOverlayElementFromTemplate("SdkTrays/Picture", "BorderPanel", name);
						bp->setHorizontalAlignment(Ogre::GHA_RIGHT);
						bp->setVerticalAlignment(Ogre::GVA_CENTER);
						bp->setMaterialName(name);
						bp->setUserAny(Ogre::Any(*i));
						mTrayMgr->getTraysLayer()->add2D(bp);

						// add Game thumbnail and title
						mThumbs.push_back(bp);
						GameTitles.push_back((*i)->getInfo()["Title"]);
					}
				}

				mCarouselPlace = 0;  // reset carousel

				mGameMenu->setItems(GameTitles);
				if (mGameMenu->getNumItems() != 0) itemSelected(mGameMenu);

				mGameSlider->setRange(1, GameTitles.size(), GameTitles.size());
			}
			else if (menu == mGameMenu)    // Game changed, so update slider, label and description
			{
				if (mGameSlider->getValue() != menu->getSelectionIndex() + 1)
					mGameSlider->setValue(menu->getSelectionIndex() + 1); 

				Game* s = Ogre::any_cast<Game*>(mThumbs[menu->getSelectionIndex()]->getUserAny());
				mTitleLabel->setCaption(menu->getSelectedItem()); 
				mDescBox->setText("Map: " + s->getInfo()["Category"] + "\nDescription: " + s->getInfo()["Description"]); //PTR TuanNA [Change label to Map- 3/11/2016]

				if (mCurrentGame != s) ((Button*)mTrayMgr->getWidget("StartStop"))->setCaption("Start Game"); //PTR TuanNA [Change label- 3/11/2016]
				else ((Button*)mTrayMgr->getWidget("StartStop"))->setCaption("Exit Game"); //PTR TuanNA [Change label- 3/11/2016]
			}
			else if (menu == mRendererMenu)    // renderer selected, so update all settings
			{
				while (mTrayMgr->getNumWidgets(mRendererMenu->getTrayLocation()) > 3)
				{
					mTrayMgr->destroyWidget(mRendererMenu->getTrayLocation(), 3);
				}

				Ogre::ConfigOptionMap& options = mRoot->getRenderSystemByName(menu->getSelectedItem())->getConfigOptions();

				unsigned int i = 0;

				// create all the config option select menus
				for (Ogre::ConfigOptionMap::iterator it = options.begin(); it != options.end(); it++)
				{
					i++;
					SelectMenu* optionMenu = mTrayMgr->createLongSelectMenu
						(TL_LEFT, "ConfigOption" + Ogre::StringConverter::toString(i), it->first, 450, 240, 10);
					optionMenu->setItems(it->second.possibleValues);
					
					// if the current config value is not in the menu, add it
					try
					{
						optionMenu->selectItem(it->second.currentValue);
					}
					catch (Ogre::Exception e)
					{
						optionMenu->addItem(it->second.currentValue);
						optionMenu->selectItem(it->second.currentValue);
					}
				}

				windowResized(mWindow);
			}
		}

		/*-----------------------------------------------------------------------------
		| Handles Game slider changes.
		-----------------------------------------------------------------------------*/
		virtual void sliderMoved(Slider* slider)
		{
			// format the caption to be fraction style
			Ogre::String denom = "/" + Ogre::StringConverter::toString(mGameMenu->getNumItems());
			slider->setValueCaption(slider->getValueCaption() + denom);

			// tell the Game menu to change if it hasn't already
			if (mGameMenu->getSelectionIndex() != -1 && mGameMenu->getSelectionIndex() != slider->getValue() - 1)
				mGameMenu->selectItem(slider->getValue() - 1);
		}

		/*-----------------------------------------------------------------------------
		| Handles keypresses.
		-----------------------------------------------------------------------------*/
		virtual bool keyPressed(const OIS::KeyEvent& evt)
		{
			if (mTrayMgr->isDialogVisible()) return true;  // ignore keypresses when dialog is showing

			if (evt.key == OIS::KC_ESCAPE)
			{
#if (OGRE_PLATFORM == OGRE_PLATFORM_WINRT) && (OGRE_WINRT_TARGET_TYPE == PHONE)
				// If there is a quit button, assume that we intended to press it via 'ESC'.
				if (mTrayMgr->areTraysVisible())
				{
					Widget *pWidget = mTrayMgr->getWidget("Quit");
					if (pWidget)
					{
						buttonHit((Button*)pWidget);  // on phone, quit entirely.
						return false;  // now act as if we didn't handle the button to get AppModel to exit.
					}
				}
#endif // (OGRE_PLATFORM == OGRE_PLATFORM_WINRT) && (OGRE_WINRT_TARGET_TYPE == PHONE)
				if (mTitleLabel->getTrayLocation() != TL_NONE)
				{
					// if we're in the main screen and a Game's running, toggle Game pause state
					if (mCurrentGame)
					{
						if (mGamePaused)
						{
							mTrayMgr->hideAll();
							unpauseCurrentGame();
						}
						else
						{
							pauseCurrentGame();
							mTrayMgr->showAll();
						}
					}
				}
				else buttonHit((Button*)mTrayMgr->getWidget("Back"));  // if we're in config, just go back
			}
			else if ((evt.key == OIS::KC_UP || evt.key == OIS::KC_DOWN) && mTitleLabel->getTrayLocation() != TL_NONE)
			{
				// if we're in the main screen, use the up and down arrow keys to cycle through Games
				int newIndex = mGameMenu->getSelectionIndex() + (evt.key == OIS::KC_UP ? -1 : 1);
				mGameMenu->selectItem(Ogre::Math::Clamp<int>(newIndex, 0, mGameMenu->getNumItems() - 1));
			}
			else if (evt.key == OIS::KC_RETURN)   // start or stop Game
			{
				if (!mLoadedGames.empty() && (mGamePaused || mCurrentGame == 0))
				{
					Game* newGame = Ogre::any_cast<Game*>(mThumbs[mGameMenu->getSelectionIndex()]->getUserAny());
					runGame(newGame == mCurrentGame ? 0 : newGame);
				}
			}
#if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
            else if (evt.key == OIS::KC_M)   // change orientation mode
            {
                unsigned int orientationMode = (unsigned int)mWindow->getViewport(0)->getOrientationMode();
                orientationMode++;
                if (orientationMode >= 4)
                    orientationMode = 0;
                mWindow->getViewport(0)->setOrientationMode((Ogre::OrientationMode)orientationMode);                
            }
#endif
			else if(evt.key == OIS::KC_F9)   // toggle full screen
			{
				// Make sure we use the window size as originally requested, NOT the
				// current window size (which may have altered to fit desktop)
				const Ogre::ConfigOptionMap::iterator opti = 
					mRoot->getRenderSystem()->getConfigOptions().find("Video Mode");
				Ogre::StringVector vmopts = Ogre::StringUtil::split(opti->second.currentValue, " x");
				unsigned int w = Ogre::StringConverter::parseUnsignedInt(vmopts[0]);
				unsigned int h = Ogre::StringConverter::parseUnsignedInt(vmopts[1]);
				mWindow->setFullscreen(!mWindow->isFullScreen(), w, h);
			}

			try
			{
				return GameContext::keyPressed(evt);
			}
			catch (Ogre::Exception e)   // show error and fall back to menu
			{
				runGame(0);
				mTrayMgr->showOkDialog("Error!", e.getDescription() + "\nSource: " + e.getSource());
			}

			return true;
		}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
		void motionBegan( void )
        {
        }
        
		void motionEnded( void )
        {
			if (mTrayMgr->isDialogVisible()) return;  // ignore keypresses when dialog is showing
            
            if (mTitleLabel->getTrayLocation() != TL_NONE)
            {
                // if we're in the main screen and a Game's running, toggle Game pause state
                if (mCurrentGame)
                {
                    if (mGamePaused)
                    {
                        mTrayMgr->hideAll();
                        unpauseCurrentGame();
                    }
                    else
                    {
                        pauseCurrentGame();
                        mTrayMgr->showAll();
                    }
                }
            }
            else buttonHit((Button*)mTrayMgr->getWidget("Back"));  // if we're in config, just go back

        }
        
		void motionCancelled( void )
        {
        }
#endif

		/*-----------------------------------------------------------------------------
		| Extends mousePressed to inject mouse press into tray manager, and to check
		| for thumbnail clicks, just because we can.
		-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchPressed(const OIS::MultiTouchEvent& evt)
#else
		virtual bool mousePressed(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
#endif
		{
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
            OIS::MultiTouchState state = evt.state;
    #if (OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0) || (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
            transformInputState(state);
    #endif
            OIS::MultiTouchEvent orientedEvt((OIS::Object*)evt.device, state);
#else
            OIS::MouseState state = evt.state;
    #if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
            transformInputState(state);
    #endif
            OIS::MouseEvent orientedEvt((OIS::Object*)evt.device, state);
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
			if (mTrayMgr->injectMouseDown(orientedEvt)) return true;
#else
			if (mTrayMgr->injectMouseDown(orientedEvt, id)) return true;
#endif
            
			if (mTitleLabel->getTrayLocation() != TL_NONE)
			{
				for (unsigned int i = 0; i < mThumbs.size(); i++)
				{
					if (mThumbs[i]->isVisible() && Widget::isCursorOver(mThumbs[i],
                            Ogre::Vector2(mTrayMgr->getCursorContainer()->getLeft(),
                                          mTrayMgr->getCursorContainer()->getTop()), 0))
					{
						mGameMenu->selectItem(i);
						break;
					}
				}
			}
            
			try
			{
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
				return GameContext::touchPressed(orientedEvt);
#else
				return GameContext::mousePressed(orientedEvt, id);
#endif
			}
			catch (Ogre::Exception e)   // show error and fall back to menu
			{
				runGame(0);
				mTrayMgr->showOkDialog("Error!", e.getDescription() + "\nSource: " + e.getSource());
			}
            
			return true;
		}

		/*-----------------------------------------------------------------------------
		| Extends mouseReleased to inject mouse release into tray manager.
		-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchReleased(const OIS::MultiTouchEvent& evt)
#else
		virtual bool mouseReleased(const OIS::MouseEvent& evt, OIS::MouseButtonID id)
#endif
		{
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
            OIS::MultiTouchState state = evt.state;
    #if (OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0) || (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
            transformInputState(state);
    #endif
            OIS::MultiTouchEvent orientedEvt((OIS::Object*)evt.device, state);
#else
            OIS::MouseState state = evt.state;
    #if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
            transformInputState(state);
    #endif
            OIS::MouseEvent orientedEvt((OIS::Object*)evt.device, state);
#endif

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
			if (mTrayMgr->injectMouseUp(orientedEvt)) return true;
#else
			if (mTrayMgr->injectMouseUp(orientedEvt, id)) return true;
#endif

			try
			{
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
				return GameContext::touchReleased(orientedEvt);
#else
				return GameContext::mouseReleased(orientedEvt, id);
#endif
			}
			catch (Ogre::Exception e)   // show error and fall back to menu
			{
				runGame(0);
				mTrayMgr->showOkDialog("Error!", e.getDescription() + "\nSource: " + e.getSource());
			}
            
			return true;
		}

		/*-----------------------------------------------------------------------------
		| Extends mouseMoved to inject mouse position into tray manager, and checks
		| for mouse wheel movements to slide the carousel, because we can.
		-----------------------------------------------------------------------------*/
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
		virtual bool touchMoved(const OIS::MultiTouchEvent& evt)
#else
		virtual bool mouseMoved(const OIS::MouseEvent& evt)
#endif
		{
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
            OIS::MultiTouchState state = evt.state;
    #if (OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0) || (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
            transformInputState(state);
    #endif
            OIS::MultiTouchEvent orientedEvt((OIS::Object*)evt.device, state);
#else
            OIS::MouseState state = evt.state;
    #if OGRE_NO_VIEWPORT_ORIENTATIONMODE == 0
            transformInputState(state);
    #endif
            OIS::MouseEvent orientedEvt((OIS::Object*)evt.device, state);
#endif

			if (mTrayMgr->injectMouseMove(orientedEvt)) return true;
            
			if (!(mCurrentGame && !mGamePaused) && mTitleLabel->getTrayLocation() != TL_NONE &&
				orientedEvt.state.Z.rel != 0 && mGameMenu->getNumItems() != 0)
			{
				int newIndex = mGameMenu->getSelectionIndex() - orientedEvt.state.Z.rel / Ogre::Math::Abs(orientedEvt.state.Z.rel);
				mGameMenu->selectItem(Ogre::Math::Clamp<int>(newIndex, 0, mGameMenu->getNumItems() - 1));
			}
            
			try
			{
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
				return GameContext::touchMoved(orientedEvt);
#else
				return GameContext::mouseMoved(orientedEvt);
#endif
			}
			catch (Ogre::Exception e)   // show error and fall back to menu
			{
				runGame(0);
				mTrayMgr->showOkDialog("Error!", e.getDescription() + "\nSource: " + e.getSource());
			}
            
			return true;
		}

#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
        /*-----------------------------------------------------------------------------
         | Extends touchCancelled to inject an event that a touch was cancelled.
         -----------------------------------------------------------------------------*/
		virtual bool touchCancelled(const OIS::MultiTouchEvent& evt)
        {
            return true;
        }
#endif
		/*-----------------------------------------------------------------------------
		| Extends windowResized to best fit menus on screen. We basically move the
		| menu tray to the left for higher resolutions and move it to the center
		| for lower resolutions.
		-----------------------------------------------------------------------------*/
		virtual void windowResized(Ogre::RenderWindow* rw)
		{
			if (!mTrayMgr) return;

			Ogre::OverlayContainer* center = mTrayMgr->getTrayContainer(TL_CENTER);
			Ogre::OverlayContainer* left = mTrayMgr->getTrayContainer(TL_LEFT);

			if (center->isVisible() && rw->getWidth() < 1280 - center->getWidth())
			{
				while (center->isVisible())
				{
					mTrayMgr->moveWidgetToTray(mTrayMgr->getWidget(TL_CENTER, 0), TL_LEFT);
				}
			}
			else if (left->isVisible() && rw->getWidth() >= 1280 - left->getWidth())
			{
				while (left->isVisible())
				{
					mTrayMgr->moveWidgetToTray(mTrayMgr->getWidget(TL_LEFT, 0), TL_CENTER);
				}
			}

			GameContext::windowResized(rw);
		}

        /*-----------------------------------------------------------------------------
         | Extends setup to create dummy scene and tray interface.
         -----------------------------------------------------------------------------*/
		virtual void setup()
		{
            if(mWindow == NULL)
                mWindow = createWindow();
            
			setupInput(mNoGrabInput);
			locateResources();

#ifdef OGRE_STATIC_LIB
            // Check if the render system supports any shader profiles.
            // Don't load Games that require shaders if we don't have any shader support, GL ES 1.x for example.
            const RenderSystemCapabilities* caps = mRoot->getRenderSystem()->getCapabilities();
            RenderSystemCapabilities::ShaderProfiles profiles = caps->getSupportedShaderProfiles();
#if defined(INCLUDE_RTSHADER_SYSTEM)
            bool hasProgrammableGPU = (!profiles.empty());
#endif

//            mPluginNameMap["Game_AtomicCounters"]     = (OgreBites::SdkGame *) OGRE_NEW Game_AtomicCounters();
            mPluginNameMap["Game_BezierPatch"]        = (OgreBites::SdkGame *) OGRE_NEW Game_BezierPatch();
            mPluginNameMap["Game_CameraTrack"]        = (OgreBites::SdkGame *) OGRE_NEW Game_CameraTrack();
            mPluginNameMap["Game_Character"]          = (OgreBites::SdkGame *) OGRE_NEW Game_Character();
#   if OGRE_PLATFORM != OGRE_PLATFORM_WINRT
            mPluginNameMap["Game_DynTex"]             = (OgreBites::SdkGame *) OGRE_NEW Game_DynTex();
            mPluginNameMap["Game_FacialAnimation"]    = (OgreBites::SdkGame *) OGRE_NEW Game_FacialAnimation();
            mPluginNameMap["Game_Grass"]              = (OgreBites::SdkGame *) OGRE_NEW Game_Grass();
			
			mPluginNameMap["Game_DualQuaternion"]     = (OgreBites::SdkGame *) OGRE_NEW Game_DualQuaternion();
 			mPluginNameMap["Game_Instancing"]			= (OgreBites::SdkGame *) OGRE_NEW Game_Instancing();
            mPluginNameMap["Game_NewInstancing"]		= (OgreBites::SdkGame *) OGRE_NEW Game_NewInstancing();
            mPluginNameMap["Game_TextureArray"]       = (OgreBites::SdkGame *) OGRE_NEW Game_TextureArray();
			mPluginNameMap["Game_Tesselation"]		= (OgreBites::SdkGame *) OGRE_NEW Game_Tesselation();
			mPluginNameMap["Game_PNTriangles"]		= (OgreBites::SdkGame *) OGRE_NEW Game_PNTriangles();

#			if defined(OGRE_BUILD_COMPONENT_VOLUME) && OGRE_PLATFORM != OGRE_PLATFORM_NACL
            mPluginNameMap["Game_VolumeCSG"]          = (OgreBites::SdkGame *) OGRE_NEW Game_VolumeCSG();
            mPluginNameMap["Game_VolumeTerrain"]      = (OgreBites::SdkGame *) OGRE_NEW Game_VolumeTerrain();
#			endif
            mPluginNameMap["Game_Shadows"]            = (OgreBites::SdkGame *) OGRE_NEW Game_Shadows();
            mPluginNameMap["Game_Lighting"]           = (OgreBites::SdkGame *) OGRE_NEW Game_Lighting();
            mPluginNameMap["Game_MeshLod"]            = (OgreBites::SdkGame *) OGRE_NEW Game_MeshLod();
            mPluginNameMap["Game_ParticleFX"]         = (OgreBites::SdkGame *) OGRE_NEW Game_ParticleFX();
            mPluginNameMap["Game_Smoke"]              = (OgreBites::SdkGame *) OGRE_NEW Game_Smoke();
#	endif // OGRE_PLATFORM_WINRT
			mPluginNameMap["Game_SkeletalAnimation"]  = (OgreBites::SdkGame *) OGRE_NEW Game_SkeletalAnimation();
            mPluginNameMap["Game_SkyBox"]             = (OgreBites::SdkGame *) OGRE_NEW Game_SkyBox();
            mPluginNameMap["Game_SkyDome"]            = (OgreBites::SdkGame *) OGRE_NEW Game_SkyDome();
            mPluginNameMap["Game_SkyPlane"]           = (OgreBites::SdkGame *) OGRE_NEW Game_SkyPlane();
            mPluginNameMap["Game_SphereMapping"]      = (OgreBites::SdkGame *) OGRE_NEW Game_SphereMapping();
            mPluginNameMap["Game_TextureFX"]          = (OgreBites::SdkGame *) OGRE_NEW Game_TextureFX();
            mPluginNameMap["Game_Transparency"]       = (OgreBites::SdkGame *) OGRE_NEW Game_Transparency();

#if defined(INCLUDE_RTSHADER_SYSTEM) && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
            if(hasProgrammableGPU)
            {
#   ifdef OGRE_BUILD_PLUGIN_BSP
				mPluginNameMap["Game_BSP"]                = (OgreBites::SdkGame *) OGRE_NEW Game_BSP();
#   endif
                mPluginNameMap["Game_CelShading"]         = (OgreBites::SdkGame *) OGRE_NEW Game_CelShading();
                mPluginNameMap["Game_Compositor"]         = (OgreBites::SdkGame *) OGRE_NEW Game_Compositor();
                mPluginNameMap["Game_CubeMapping"]        = (OgreBites::SdkGame *) OGRE_NEW Game_CubeMapping();
                mPluginNameMap["Game_DeferredShading"]    = (OgreBites::SdkGame *) OGRE_NEW Game_DeferredShading();
				mPluginNameMap["Game_SSAO"]               = (OgreBites::SdkGame *) OGRE_NEW Game_SSAO();
                mPluginNameMap["Game_ShaderSystem"]       = (OgreBites::SdkGame *) OGRE_NEW Game_ShaderSystem();
                mPluginNameMap["Game_Ocean"]              = (OgreBites::SdkGame *) OGRE_NEW Game_Ocean();
                mPluginNameMap["Game_Water"]              = (OgreBites::SdkGame *) OGRE_NEW Game_Water();
#   ifdef OGRE_BUILD_COMPONENT_TERRAIN
                mPluginNameMap["Game_Terrain"]            = (OgreBites::SdkGame *) OGRE_NEW Game_Terrain();
                mPluginNameMap["Game_EndlessWorld"]       = (OgreBites::SdkGame *) OGRE_NEW Game_EndlessWorld();
#   endif
                mPluginNameMap["Game_Dot3Bump"]           = (OgreBites::SdkGame *) OGRE_NEW Game_Dot3Bump();
                mPluginNameMap["Game_Fresnel"]            = (OgreBites::SdkGame *) OGRE_NEW Game_Fresnel();
            }
#endif
#endif

			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Essential");
			mTrayMgr = new SdkTrayManager("BrowserControls", mWindow, mInputContext, this);
			mTrayMgr->showBackdrop("SdkTrays/Bands");
			mTrayMgr->getTrayContainer(TL_NONE)->hide();

#if ENABLE_SHADERS_CACHE_SAVE == 1
            if(Ogre::GpuProgramManager::getSingleton().canGetCompiledShaderBuffer())
                Ogre::GpuProgramManager::getSingleton().setSaveMicrocodesToCache(true);
#endif
#if	ENABLE_SHADERS_CACHE_LOAD == 1
			// Load for a package version of the shaders.
			Ogre::String path = getShaderCacheFileName();
			FILE * inFile = NULL;
			inFile = fopen(path.c_str(), "rb");
			// If that does not exist, see if there is a version in the writable location.
			if (!inFile)
			{
				path = mFSLayer->getWritablePath(getShaderCacheFileName());
				inFile = fopen(path.c_str(), "rb");
			}
            if (inFile)
            {
                Ogre::LogManager::getSingleton().logMessage("Loading shader cache from ");
				Ogre::LogManager::getSingleton().logMessage(path.c_str());
            	Ogre::DataStreamPtr istream(new Ogre::FileHandleDataStream(path.c_str(), inFile, Ogre::DataStream::READ));
				Ogre::GpuProgramManager::getSingleton().loadMicrocodeCache(istream);
            }
#endif

			createDummyScene();

#ifdef INCLUDE_RTSHADER_SYSTEM
            if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION) == false)
            {
                Ogre::RTShader::ShaderGenerator::getSingletonPtr()->addSceneManager(mRoot->getSceneManager("DummyScene"));
            }
#endif // INCLUDE_RTSHADER_SYSTEM

			loadResources();


			Game* startupGame = loadGames();
            
			Ogre::TextureManager::getSingleton().setDefaultNumMipmaps(5);
            
			// adds context as listener to process context-level (above the Game level) events
			mRoot->addFrameListener(this);
			Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);
            
			// create template material for Game thumbnails
			Ogre::MaterialPtr thumbMat = Ogre::MaterialManager::getSingleton().create("SdkTrays/GameThumbnail", "Essential");
			thumbMat->getTechnique(0)->getPass(0)->createTextureUnitState();
            
			setupWidgets();
			windowResized(mWindow);   // adjust menus for resolution

			// if this is our first time running, and there's a startup Game, run it
			if (startupGame && mFirstRun){
				runGame(startupGame);
			}
		}

		/*-----------------------------------------------------------------------------
		| Notify the window size changed or it was moved
		-----------------------------------------------------------------------------*/
		virtual void windowMovedOrResized()
		{
#if (OGRE_PLATFORM == OGRE_PLATFORM_WINRT) && (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)

			if(mNativeControl)
			{
				// in WinRT.Xaml case Ogre::RenderWindow is actually brush
				// applied to native control and we need resize this brush manually
				mWindow->resize(mNativeControl->ActualWidth, mNativeControl->ActualHeight);
			}
#endif // (OGRE_PLATFORM == OGRE_PLATFORM_WINRT) && (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
			mWindow->windowMovedOrResized();	// notify window
			windowResized(mWindow);				// notify window event listeners
		}
        
	protected:

		/*-----------------------------------------------------------------------------
		| Restores config instead of using a dialog to save time.
		| If that fails, the config dialog is shown.
		-----------------------------------------------------------------------------*/
		virtual bool oneTimeConfig()
		{
			if (!mRoot->restoreConfig()) return mRoot->showConfigDialog();
			return true;
		}		

		/*-----------------------------------------------------------------------------
		| Overrides the default window title.
		-----------------------------------------------------------------------------*/
		virtual Ogre::RenderWindow* createWindow()
		{
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
			Ogre::RenderWindow* res = mRoot->initialise(false, "OGRE Game Browser");
			Ogre::NameValuePairList miscParams;
            miscParams["pp::Instance"] = Ogre::StringConverter::toString((unsigned long)mNaClInstance);
            miscParams["SwapCallback"] = Ogre::StringConverter::toString((unsigned long)mNaClSwapCallback);
            // create 1x1 window - we will resize later
            return mRoot->createRenderWindow("OGRE Game Browser Window", mInitWidth, mInitHeight, false, &miscParams);

#elif (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
			Ogre::RenderWindow* res = mRoot->initialise(false, "OGRE Game Browser");
			Ogre::NameValuePairList miscParams;
			if(mNativeWindow.Get())
			{
				miscParams["externalWindowHandle"] = Ogre::StringConverter::toString((size_t)reinterpret_cast<void*>(mNativeWindow.Get()));
				res = mRoot->createRenderWindow("OGRE Game Browser Window", mNativeWindow->Bounds.Width, mNativeWindow->Bounds.Height, false, &miscParams);
			}
#	if (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
			else if(mNativeControl)
			{
				miscParams["windowType"] = "SurfaceImageSource";
				res = mRoot->createRenderWindow("OGRE Game Browser Window", mNativeControl->ActualWidth, mNativeControl->ActualHeight, false, &miscParams);
				void* pUnk = NULL;
				res->getCustomAttribute("ImageBrush", &pUnk);
				mNativeControl->Fill = reinterpret_cast<Windows::UI::Xaml::Media::ImageBrush^>(pUnk);
			}
#	endif // (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)

			return res;

#elif OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
			return NULL;
#else
			//PTR TuanNA [- 3/11/2016]Ogre::RenderWindow* res = mRoot->initialise(true, "OGRE Game Browser");
			Ogre::RenderWindow* res = mRoot->initialise(true, "Legend Of Thach Sanh"); //PTR TuanNA [- 3/11/2016]
			
			//PTR TuanNA begin comment
			//[Add icon to app- 10/7/2016]
			#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
			HWND hwnd;
			res->getCustomAttribute("WINDOW", (void*)&hwnd);
			LONG iconID   = (LONG)LoadIcon( GetModuleHandle(0), MAKEINTRESOURCE(IDI_APPICON) );
			SetClassLong( hwnd, GCL_HICON, iconID );
			#endif
			//PTR TuanNA end comment


#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            mGestureView = [[GameBrowserGestureView alloc] init];
            mGestureView.mBrowser = this;
            
            [[[UIApplication sharedApplication] keyWindow] addSubview:mGestureView];
#endif
			
			return res;
#endif
        }

		/*-----------------------------------------------------------------------------
		| Initialises only the browser's resources and those most commonly used
		| by Games. This way, additional special content can be initialised by
		| the Games that use them, so startup time is unaffected.
		-----------------------------------------------------------------------------*/
		virtual void loadResources()
		{
#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
			mTrayMgr->showLoadingBar(1, 0);
#endif
			Ogre::ResourceGroupManager::getSingleton().initialiseResourceGroup("Popular");
#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
			mTrayMgr->hideLoadingBar();
#endif
		}

		/*-----------------------------------------------------------------------------
		| Creates dummy scene to allow rendering GUI in viewport.
		-----------------------------------------------------------------------------*/
		virtual void createDummyScene()
		{
			mWindow->removeAllViewports();
			Ogre::SceneManager* sm = mRoot->createSceneManager(Ogre::ST_GENERIC, "DummyScene");
			sm->addRenderQueueListener(mOverlaySystem);
			Ogre::Camera* cam = sm->createCamera("DummyCamera");
			mWindow->addViewport(cam);
#ifdef INCLUDE_RTSHADER_SYSTEM
			// Initialize shader generator.
			// Must be before resource loading in order to allow parsing extended material attributes.
			bool success = initialiseRTShaderSystem(sm);
			if (!success) 
			{
				OGRE_EXCEPT(Ogre::Exception::ERR_FILE_NOT_FOUND, 
					"Shader Generator Initialization failed - Core shader libs path not found", 
					"GameBrowser::createDummyScene");
			}
			if(mRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_FIXED_FUNCTION) == false)
			{
				//newViewport->setMaterialScheme(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);
				
				// creates shaders for base material BaseWhite using the RTSS
				Ogre::MaterialPtr baseWhite = Ogre::MaterialManager::getSingleton().getByName("BaseWhite", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
				baseWhite->setLightingEnabled(false);
				mShaderGenerator->createShaderBasedTechnique(
					"BaseWhite", 
					Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
					Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
			    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
					"BaseWhite");
                if(baseWhite->getNumTechniques() > 1)
                {
				    baseWhite->getTechnique(0)->getPass(0)->setVertexProgram(
				    baseWhite->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
				    baseWhite->getTechnique(0)->getPass(0)->setFragmentProgram(
				    baseWhite->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
                }

				// creates shaders for base material BaseWhiteNoLighting using the RTSS
				mShaderGenerator->createShaderBasedTechnique(
					"BaseWhiteNoLighting", 
					Ogre::MaterialManager::DEFAULT_SCHEME_NAME, 
					Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME);	
			    mShaderGenerator->validateMaterial(Ogre::RTShader::ShaderGenerator::DEFAULT_SCHEME_NAME, 
					"BaseWhiteNoLighting");
				Ogre::MaterialPtr baseWhiteNoLighting = Ogre::MaterialManager::getSingleton().getByName("BaseWhiteNoLighting", Ogre::ResourceGroupManager::INTERNAL_RESOURCE_GROUP_NAME);
                if(baseWhite->getNumTechniques() > 1)
                {
				    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setVertexProgram(
				    baseWhiteNoLighting->getTechnique(1)->getPass(0)->getVertexProgram()->getName());
				    baseWhiteNoLighting->getTechnique(0)->getPass(0)->setFragmentProgram(
				    baseWhiteNoLighting->getTechnique(1)->getPass(0)->getFragmentProgram()->getName());
                }
			}
#endif // INCLUDE_RTSHADER_SYSTEM
		}

		/*-----------------------------------------------------------------------------
		| Loads Game plugins from a configuration file.
		-----------------------------------------------------------------------------*/
		virtual Game* loadGames()
		{
			Game* startupGame = 0;

			Ogre::StringVector unloadedGamePlugins;

#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
            Ogre::String startupGameTitle = "";
            Ogre::String GameDir = "";
            Ogre::StringVector GameList;
            GameList.push_back("Game_BezierPatch");
            GameList.push_back("Game_CameraTrack");
            GameList.push_back("Game_CelShading");
            GameList.push_back("Game_Character");     
            GameList.push_back("Game_Compositor");     
            GameList.push_back("Game_CubeMapping");    
            GameList.push_back("Game_Dot3Bump");
            GameList.push_back("Game_DynTex");      
            GameList.push_back("Game_FacialAnimation");
            GameList.push_back("Game_Fresnel");
            GameList.push_back("Game_ParticleFX");
#   ifdef INCLUDE_RTSHADER_SYSTEM
            GameList.push_back("Game_ShaderSystem");
#	endif
            GameList.push_back("Game_Lighting");
            GameList.push_back("Game_MeshLod");
            GameList.push_back("Game_SkyBox"); 
            GameList.push_back("Game_SkyDome"); 
            GameList.push_back("Game_SkyPlane"); 
            GameList.push_back("Game_Smoke");
            GameList.push_back("Game_Water");
			GameList.push_back("Game_PNTriangles");
			GameList.push_back("Game_Tesselation");
            GameList.push_back("Game_Transparency");
            GameList.push_back("Game_TextureFX");
#else
			Ogre::ConfigFile cfg;
#if OGRE_PLATFORM == OGRE_PLATFORM_ANDROID
            cfg.load(openAPKFile(mFSLayer->getConfigFilePath("Games.cfg")));
#else
			cfg.load(mFSLayer->getConfigFilePath("games.cfg"));
#endif
			//PTR TuanNA [Change Export name- 15/11/2016]
			Ogre::String GameDir = cfg.getSetting("GameFolder");        // Mac OS X just uses Resources/ directory
			Ogre::StringVector GameList = cfg.getMultiSetting("GamePlugin");
			Ogre::String startupGameTitle = cfg.getSetting("StartupGame");

			#if OGRE_PLATFORM != OGRE_PLATFORM_APPLE && OGRE_PLATFORM != OGRE_PLATFORM_APPLE_IOS
			if (GameDir.empty()) GameDir = ".";   // user didn't specify plugins folder, try current one
			#endif

			// add slash or backslash based on platform
			char lastChar = GameDir[GameDir.length() - 1];
			if (lastChar != '/' && lastChar != '\\')
			{
				#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32 || (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
				GameDir += "\\";
				#elif OGRE_PLATFORM == OGRE_PLATFORM_LINUX
				GameDir += "/";
				#endif
			}
#endif

#ifdef GAMES_INCLUDE_PLAYPEN
#  ifdef OGRE_STATIC_LIB
			playPenPlugin = OGRE_NEW PlayPenPlugin();
			mRoot->installPlugin(playPenPlugin);
			GameSet newGames = playPenPlugin->getGames();
			for (GameSet::iterator j = newGames.begin(); j != newGames.end(); j++)
			{
				Ogre::NameValuePairList& info = (*j)->getInfo();   // acquire custom Game info
				Ogre::NameValuePairList::iterator k;

				// give Game default title and category if none found
				k= info.find("Title");
				if (k == info.end() || k->second.empty()) info["Title"] = "Untitled";
				k = info.find("Category");
				if (k == info.end() || k->second.empty()) info["Category"] = "Unsorted";
				k = info.find("Thumbnail");
				if (k == info.end() || k->second.empty()) info["Thumbnail"] = "thumb_error.png";
				mGameCategories.insert(info["Category"]);   // add Game category
				if (info["Title"] == startupGameTitle) startupGame = *j;   // we found the startup Game
                GameList.push_back(info["Title"]);
                mPluginNameMap[info["Title"]] = (OgreBites::SdkGame *)(*j);
			}

			playPenTestPlugin = OGRE_NEW PlaypenTestPlugin();
			mRoot->installPlugin(playPenTestPlugin);
            newGames = playPenTestPlugin->getGames();
			for (GameSet::iterator j = newGames.begin(); j != newGames.end(); j++)
			{
				Ogre::NameValuePairList& info = (*j)->getInfo();   // acquire custom Game info
				Ogre::NameValuePairList::iterator k;
                
				// give Game default title and category if none found
				k= info.find("Title");
				if (k == info.end() || k->second.empty()) info["Title"] = "Untitled";
				k = info.find("Category");
				if (k == info.end() || k->second.empty()) info["Category"] = "Unsorted";
				k = info.find("Thumbnail");
				if (k == info.end() || k->second.empty()) info["Thumbnail"] = "thumb_error.png";
				mGameCategories.insert(info["Category"]);   // add Game category
				if (info["Title"] == startupGameTitle) startupGame = *j;   // we found the startup Game
                GameList.push_back(info["Title"]);
                mPluginNameMap[info["Title"]] = (OgreBites::SdkGame *)(*j);
			}
#  else
#    if OGRE_DEBUG_MODE && !(OGRE_PLATFORM == OGRE_PLATFORM_APPLE || OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS)
			GameList.push_back("PlayPen_d");
#    else
			GameList.push_back("PlayPen");
			GameList.push_back("PlayPenTests");
#    endif
#  endif
#endif

			// loop through all Game plugins...
			for (Ogre::StringVector::iterator i = GameList.begin(); i != GameList.end(); i++)
			{
				try   // try to load the plugin
				{
#ifdef OGRE_STATIC_LIB
					String GameName = *i;
					// in debug, remove any suffix
					if(StringUtil::endsWith(GameName, "_d"))
						GameName = GameName.substr(0, GameName.length()-2);

                    OgreBites::SdkGame *pluginInstance = (OgreBites::SdkGame *) mPluginNameMap[GameName];
                    if(pluginInstance)
                    {
                        OgreBites::GamePlugin* sp = OGRE_NEW GamePlugin(pluginInstance->getInfo()["Title"] + " Game");

                        sp->addGame(pluginInstance);
                        mRoot->installPlugin(sp);
                    }
#else
					mRoot->loadPlugin(GameDir + *i);
#endif
				}
				catch (Ogre::Exception e)   // plugin couldn't be loaded
				{
					unloadedGamePlugins.push_back(GameDir + *i);
					continue;
				}

				Ogre::Plugin* p = mRoot->getInstalledPlugins().back();   // acquire plugin instance
				GamePlugin* sp = dynamic_cast<GamePlugin*>(p);

				if (!sp)  // this is not a GamePlugin, so unload it
				{
					//unloadedGamePlugins.push_back(GameDir + *i); 
#ifdef OGRE_STATIC_LIB
					//mRoot->uninstallPlugin(p);
#else
					//mRoot->unloadPlugin(GameDir + *i);
#endif
					continue;
				}
                
				mLoadedGamePlugins.push_back(GameDir + *i);   // add to records

				// go through every Game in the plugin...
				GameSet newGames = sp->getGames();
				for (GameSet::iterator j = newGames.begin(); j != newGames.end(); j++)
				{
					Ogre::NameValuePairList& info = (*j)->getInfo();   // acquire custom Game info
					Ogre::NameValuePairList::iterator k;

					// give Game default title and category if none found
					k= info.find("Title");
					if (k == info.end() || k->second.empty()) info["Title"] = "Untitled";
					k = info.find("Category");
					if (k == info.end() || k->second.empty()) info["Category"] = "Unsorted";
					k = info.find("Thumbnail");
					if (k == info.end() || k->second.empty()) info["Thumbnail"] = "thumb_error.png";

					mLoadedGames.insert(*j);                    // add Game only after ensuring title for sorting
					mGameCategories.insert(info["Category"]);   // add Game category

					if (info["Title"] == startupGameTitle) startupGame = *j;   // we found the startup Game
				}
			}

			if (!mLoadedGames.empty()) mGameCategories.insert("All");   // insert a category for all Games

			if (!unloadedGamePlugins.empty())   // show error message summarising missing or invalid plugins
			{
				Ogre::String message = "These requested Game plugins were either missing, corrupt or invalid:";

				for (unsigned int i = 0; i < unloadedGamePlugins.size(); i++)
				{
					message += "\n- " + unloadedGamePlugins[i];
				}

				mTrayMgr->showOkDialog("Error!", message);
			}

			return startupGame;
		}

		/*-----------------------------------------------------------------------------
		| Unloads any loaded Game plugins.
		-----------------------------------------------------------------------------*/
		virtual void unloadGames()
		{
#ifdef INCLUDE_RTSHADER_SYSTEM
            mShaderGenerator->removeAllShaderBasedTechniques(); // clear techniques from the RTSS
#endif
#ifdef OGRE_STATIC_LIB
            const Ogre::Root::PluginInstanceList pluginList = mRoot->getInstalledPlugins();
            for(unsigned int i = 0; i < pluginList.size(); i++)
            {
				GamePlugin* sp = dynamic_cast<GamePlugin*>(pluginList[i]);

                // This is a Game plugin so we can unload it
                if(sp)
                    mRoot->uninstallPlugin(pluginList[i]);
            }
#  ifdef GAMES_INCLUDE_PLAYPEN
			mRoot->uninstallPlugin(playPenPlugin);
			delete playPenPlugin;
			mRoot->uninstallPlugin(playPenTestPlugin);
			delete playPenTestPlugin;
#  endif
#else
            for (unsigned int i = 0; i < mLoadedGamePlugins.size(); i++)
            {
                mRoot->unloadPlugin(mLoadedGamePlugins[i]);
            }
#endif

			mLoadedGames.clear();
			mLoadedGamePlugins.clear();
			mGameCategories.clear();
		}

		/*-----------------------------------------------------------------------------
		| Sets up main page for browsing Games.
		-----------------------------------------------------------------------------*/
		virtual void setupWidgets()
		{
			mTrayMgr->destroyAllWidgets();

			// create main navigation tray
			mTrayMgr->showLogo(TL_RIGHT);
			mTrayMgr->createSeparator(TL_RIGHT, "LogoSep");
			//PTR TuanNA [Change Label button- 3/11/2016]mTrayMgr->createButton(TL_RIGHT, "StartStop", "Start Game", 120);
			mTrayMgr->createButton(TL_RIGHT, "StartStop", "Start Game", 120);//PTR TuanNA [Change Label button- 3/11/2016]
#if OGRE_PLATFORM != OGRE_PLATFORM_NACL
#	if	OGRE_PLATFORM != OGRE_PLATFORM_WINRT
			mTrayMgr->createButton(TL_RIGHT, "UnloadReload", mLoadedGames.empty() ? "Connect Game" : "Disconnect Game"); //PTR TuanNA [Change Label button- 3/11/2016]
            mTrayMgr->createButton(TL_RIGHT, "Configure", "Configure");
#	endif // OGRE_PLATFORM_WINRT
			mTrayMgr->createButton(TL_RIGHT, "Quit", "Quit");
#endif // OGRE_PLATFORM_NACL

			// create Game viewing controls
			mTitleLabel = mTrayMgr->createLabel(TL_LEFT, "GameTitle", "");
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID)
			mDescBox = mTrayMgr->createTextBox(TL_LEFT, "GameInfo", "Game Info", 120, 100);
			mCategoryMenu = mTrayMgr->createThickSelectMenu(TL_LEFT, "CategoryMenu", "Select Category", 120, 10); 
			mGameMenu = mTrayMgr->createThickSelectMenu(TL_LEFT, "GameMenu", "Select Game", 120, 10);
			mGameSlider = mTrayMgr->createThickSlider(TL_LEFT, "GameSlider", "Slide Games", 120, 42, 0, 0, 0);
#else
			mDescBox = mTrayMgr->createTextBox(TL_LEFT, "GameInfo", "Game Info", 250, 208); //PTR TuanNA [Change Label button- 3/11/2016]
			mCategoryMenu = mTrayMgr->createThickSelectMenu(TL_LEFT, "CategoryMenu", "Select Difficulty Level", 250, 10); //PTR TuanNA [Change Label button- 3/11/2016]
			mGameMenu = mTrayMgr->createThickSelectMenu(TL_LEFT, "GameMenu", "Select Game", 250, 10); //PTR TuanNA [Change Label button- 3/11/2016]
			mGameSlider = mTrayMgr->createThickSlider(TL_LEFT, "GameSlider", "Slide Game", 250, 80, 0, 0, 0); //PTR TuanNA [Change Label button- 3/11/2016]
#endif
			/* Sliders do not notify their listeners on creation, so we manually call the callback here
			to format the slider value correctly. */
			sliderMoved(mGameSlider);

			// create configuration screen button tray
			mTrayMgr->createButton(TL_NONE, "Apply", "Apply Changes");
			mTrayMgr->createButton(TL_NONE, "Back", "Go Back");

			// create configuration screen label and renderer menu
			mTrayMgr->createLabel(TL_NONE, "ConfigLabel", "Configuration");
#if (OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS) || (OGRE_PLATFORM == OGRE_PLATFORM_ANDROID) || (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
			mRendererMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "RendererMenu", "Render System", 216, 115, 10);
#else
			mRendererMenu = mTrayMgr->createLongSelectMenu(TL_NONE, "RendererMenu", "Render System", 450, 240, 10);
#endif
			mTrayMgr->createSeparator(TL_NONE, "ConfigSeparator");

			// populate render system names
			Ogre::StringVector rsNames;
			Ogre::RenderSystemList rsList = mRoot->getAvailableRenderers();
			for (unsigned int i = 0; i < rsList.size(); i++)
			{
				rsNames.push_back(rsList[i]->getName());
			}
			mRendererMenu->setItems(rsNames);

			populateGameMenus();
		}

		/*-----------------------------------------------------------------------------
		| Populates home menus with loaded Games.
		-----------------------------------------------------------------------------*/
		virtual void populateGameMenus()
		{
			Ogre::StringVector categories;
			for (std::set<Ogre::String>::iterator i = mGameCategories.begin(); i != mGameCategories.end(); i++)
				categories.push_back(*i);

			mCategoryMenu->setItems(categories);
			if (mCategoryMenu->getNumItems() != 0)
                mCategoryMenu->selectItem(0);
			else
                itemSelected(mCategoryMenu);   // if there are no items, we can't select one, so manually invoke callback
		}

		/*-----------------------------------------------------------------------------
		| Overrides to recover by last Game's index instead.
		-----------------------------------------------------------------------------*/
		virtual void recoverLastGame()
		{
			// restore the view while we're at it too
			mCategoryMenu->selectItem(mLastViewCategory);
			mGameMenu->selectItem(mLastViewTitle);

			if (mLastGameIndex != -1)
			{
				int index = -1;
				for (GameSet::iterator i = mLoadedGames.begin(); i != mLoadedGames.end(); i++)
				{
					index++;
					if (index == mLastGameIndex)
					{
						runGame(*i);
						(*i)->restoreState(mLastGameState);
						mLastGame = 0;
						mLastGameIndex = -1;
						mLastGameState.clear();
					}
				}

				pauseCurrentGame();
				mTrayMgr->showAll();
			}

			buttonHit((Button*)mTrayMgr->getWidget("Configure"));
		}

		/*-----------------------------------------------------------------------------
		| Extends reconfigure to save the view and the index of last Game run.
		-----------------------------------------------------------------------------*/
		virtual void reconfigure(const Ogre::String& renderer, Ogre::NameValuePairList& options)
		{
			mLastViewCategory = mCategoryMenu->getSelectionIndex();
			mLastViewTitle = mGameMenu->getSelectionIndex();

			mLastGameIndex = -1;
			unsigned int index = -1;
			for (GameSet::iterator i = mLoadedGames.begin(); i != mLoadedGames.end(); i++)
			{
				index++;
				if (*i == mCurrentGame)
				{
					mLastGameIndex = index;
					break;
				}
			}

			GameContext::reconfigure(renderer, options);
		}
    public:
		/*-----------------------------------------------------------------------------
		| Extends shutdown to destroy dummy scene and tray interface.
		-----------------------------------------------------------------------------*/
		virtual void shutdown()
		{
#if ENABLE_SHADERS_CACHE_SAVE == 1
			if (Ogre::GpuProgramManager::getSingleton().isCacheDirty())
			{
				Ogre::String path = mFSLayer->getWritablePath(getShaderCacheFileName());
				FILE * outFile = fopen(path.c_str(), "wb");
				if (outFile)
				{
					Ogre::LogManager::getSingleton().logMessage("Writing shader cache to ");
					Ogre::LogManager::getSingleton().logMessage(path.c_str());
            		Ogre::DataStreamPtr ostream(new Ogre::FileHandleDataStream(path.c_str(), outFile, Ogre::DataStream::WRITE));
            		Ogre::GpuProgramManager::getSingleton().saveMicrocodeCache(ostream);
            		ostream->close();
				}
            }
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
            [mGestureView release];
#endif
			if (mTrayMgr)
			{
				delete mTrayMgr;
				mTrayMgr = 0;
			}

			if (!mCurrentGame && mRoot->getRenderSystem() != NULL) destroyDummyScene();

			mCategoryMenu = 0;
			mGameMenu = 0;
			mGameSlider = 0;
			mTitleLabel = 0;
			mDescBox = 0;
			mRendererMenu = 0;
			mHiddenOverlays.clear();
			mThumbs.clear();
			mCarouselPlace = 0;
            mWindow = 0;

			GameContext::shutdown();

			unloadGames();

#ifdef INCLUDE_RTSHADER_SYSTEM
			// Destroy the RT Shader System.
			destroyRTShaderSystem();
#endif // INCLUDE_RTSHADER_SYSTEM

		}
    protected:
		/*-----------------------------------------------------------------------------
		| Destroys dummy scene.
		-----------------------------------------------------------------------------*/
		virtual void destroyDummyScene()
		{
            if(!mRoot->hasSceneManager("DummyScene"))
                return;

			Ogre::SceneManager*  dummyScene = mRoot->getSceneManager("DummyScene");
#ifdef INCLUDE_RTSHADER_SYSTEM
			mShaderGenerator->removeSceneManager(dummyScene);
#endif
			dummyScene->removeRenderQueueListener(mOverlaySystem);
			mWindow->removeAllViewports();
			mRoot->destroySceneManager(dummyScene);
		}	

		/*-----------------------------------------------------------------------------
		| Extend to temporarily hide a Game's overlays while in the pause menu.
		-----------------------------------------------------------------------------*/
		virtual void pauseCurrentGame()
		{
			GameContext::pauseCurrentGame();

			Ogre::OverlayManager::OverlayMapIterator it = Ogre::OverlayManager::getSingleton().getOverlayIterator();
			mHiddenOverlays.clear();

			while (it.hasMoreElements())
			{
				Ogre::Overlay* o = it.getNext();
				if (o->isVisible())                  // later, we don't want to unhide the initially hidden overlays
				{
					mHiddenOverlays.push_back(o);    // save off hidden overlays so we can unhide them later
					o->hide();
				}
			}
		}

		/*-----------------------------------------------------------------------------
		| Extend to unhide all of Game's temporarily hidden overlays.
		-----------------------------------------------------------------------------*/
		virtual void unpauseCurrentGame()
		{
			GameContext::unpauseCurrentGame();

			for (std::vector<Ogre::Overlay*>::iterator i = mHiddenOverlays.begin(); i != mHiddenOverlays.end(); i++)
			{
				(*i)->show();
			}

			mHiddenOverlays.clear();
		}

		/*-----------------------------------------------------------------------------
		| Get the name of the RTSS shader cache file
		-----------------------------------------------------------------------------*/
		virtual Ogre::String getShaderCacheFileName()
		{
#if OGRE_DEBUG_MODE
			return "cache_d.bin";
#else
			return "cache.bin";
#endif
		}

#ifdef INCLUDE_RTSHADER_SYSTEM

		/*-----------------------------------------------------------------------------
		| Initialize the RT Shader system.	
		-----------------------------------------------------------------------------*/
		virtual bool initialiseRTShaderSystem(Ogre::SceneManager* sceneMgr)
		{			
			if (Ogre::RTShader::ShaderGenerator::initialize())
			{
				mShaderGenerator = Ogre::RTShader::ShaderGenerator::getSingletonPtr();

				mShaderGenerator->addSceneManager(sceneMgr);

#if OGRE_PLATFORM != OGRE_PLATFORM_ANDROID && OGRE_PLATFORM != OGRE_PLATFORM_NACL && OGRE_PLATFORM != OGRE_PLATFORM_WINRT
				// Setup core libraries and shader cache path.
				Ogre::StringVector groupVector = Ogre::ResourceGroupManager::getSingleton().getResourceGroups();
				Ogre::StringVector::iterator itGroup = groupVector.begin();
				Ogre::StringVector::iterator itGroupEnd = groupVector.end();
				Ogre::String shaderCoreLibsPath;
				Ogre::String shaderCachePath;
			
				for (; itGroup != itGroupEnd; ++itGroup)
				{
					Ogre::ResourceGroupManager::LocationList resLocationsList = Ogre::ResourceGroupManager::getSingleton().getResourceLocationList(*itGroup);
					Ogre::ResourceGroupManager::LocationList::iterator it = resLocationsList.begin();
					Ogre::ResourceGroupManager::LocationList::iterator itEnd = resLocationsList.end();
					bool coreLibsFound = false;

					// Try to find the location of the core shader lib functions and use it
					// as shader cache path as well - this will reduce the number of generated files
					// when running from different directories.
					for (; it != itEnd; ++it)
					{
						if ((*it)->archive->getName().find("RTShaderLib") != Ogre::String::npos)
						{
							shaderCoreLibsPath = (*it)->archive->getName() + "/cache/";
							shaderCachePath = shaderCoreLibsPath;
							coreLibsFound = true;
							break;
						}
					}
					// Core libs path found in the current group.
					if (coreLibsFound) 
						break; 
				}

				// Core shader libs not found -> shader generating will fail.
				if (shaderCoreLibsPath.empty())			
					return false;			
								
#ifdef _RTSS_WRITE_SHADERS_TO_DISK
				// Set shader cache path.
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
                shaderCachePath = Ogre::macCachePath();
#elif OGRE_PLATFORM == OGRE_PLATFORM_APPLE
                shaderCachePath = Ogre::macCachePath() + "/org.ogre3d.RTShaderCache";
#endif
				mShaderGenerator->setShaderCachePath(shaderCachePath);		
#endif
#endif
				// Create and register the material manager listener if it doesn't exist yet.
				if (mMaterialMgrListener == NULL) {
					mMaterialMgrListener = new ShaderGeneratorTechniqueResolverListener(mShaderGenerator);
					Ogre::MaterialManager::getSingleton().addListener(mMaterialMgrListener);
				}
			}

			return true;
		}

		/*-----------------------------------------------------------------------------
		| Destroy the RT Shader system.
		-----------------------------------------------------------------------------*/
		virtual void destroyRTShaderSystem()
		{
			// Restore default scheme.
			Ogre::MaterialManager::getSingleton().setActiveScheme(Ogre::MaterialManager::DEFAULT_SCHEME_NAME);

			// Unregister the material manager listener.
			if (mMaterialMgrListener != NULL)
			{			
				Ogre::MaterialManager::getSingleton().removeListener(mMaterialMgrListener);
				delete mMaterialMgrListener;
				mMaterialMgrListener = NULL;
			}

			// Destroy RTShader system.
			if (mShaderGenerator != NULL)
			{				
				Ogre::RTShader::ShaderGenerator::destroy();
				mShaderGenerator = NULL;
			}
		}
#endif // INCLUDE_RTSHADER_SYSTEM

        bool mNoGrabInput;                             // don't grab input devices
		SdkTrayManager* mTrayMgr;                      // SDK tray interface
#ifdef OGRE_STATIC_LIB
        PluginMap mPluginNameMap;                      // A structure to map plugin names to class types
#endif
		Ogre::StringVector mLoadedGamePlugins;       // loaded Game plugins
		std::set<Ogre::String> mGameCategories;      // Game categories
		GameSet mLoadedGames;                      // loaded Games
		SelectMenu* mCategoryMenu;                     // Game category select menu
		SelectMenu* mGameMenu;                       // Game select menu
		Slider* mGameSlider;                         // Game slider bar
		Label* mTitleLabel;                            // Game title label
		TextBox* mDescBox;                             // Game description box
		SelectMenu* mRendererMenu;                     // render system selection menu
		std::vector<Ogre::Overlay*> mHiddenOverlays;   // Game overlays hidden for pausing
		std::vector<Ogre::OverlayContainer*> mThumbs;  // Game thumbnails
		Ogre::Real mCarouselPlace;                     // current state of carousel
		int mLastViewTitle;                            // last Game title viewed
		int mLastViewCategory;                         // last Game category viewed
		int mLastGameIndex;                          // index of last Game running
		int mStartGameIndex;                         // directly starts the Game with the given index
#if (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
		Platform::Agile<Windows::UI::Core::CoreWindow> mNativeWindow;
#	if (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
		Windows::UI::Xaml::Shapes::Rectangle^ mNativeControl;
#	endif // (OGRE_WINRT_TARGET_TYPE == DESKTOP_APP)
#endif // (OGRE_PLATFORM == OGRE_PLATFORM_WINRT)
#if OGRE_PLATFORM == OGRE_PLATFORM_NACL
        pp::Instance* mNaClInstance;
        pp::CompletionCallback* mNaClSwapCallback;
        OIS::FactoryCreator * mOisFactory;
        Ogre::uint32 mInitWidth;
        Ogre::uint32 mInitHeight;
#endif
#ifdef INCLUDE_RTSHADER_SYSTEM
		Ogre::RTShader::ShaderGenerator*			mShaderGenerator;			// The Shader generator instance.
		ShaderGeneratorTechniqueResolverListener*	mMaterialMgrListener;		// Shader generator material manager listener.	
#endif // INCLUDE_RTSHADER_SYSTEM
    public:
#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS
        GameBrowserGestureView *mGestureView;
#endif
        bool mIsShuttingDown;
	};
}

#if OGRE_PLATFORM == OGRE_PLATFORM_APPLE_IOS

@implementation GameBrowserGestureView

@synthesize mBrowser;

- (BOOL)canBecomeFirstResponder
{
    return YES;
}

- (void)dealloc {
    [super dealloc];
}

- (void)motionBegan:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if(mBrowser && event.type == UIEventTypeMotion && event.subtype == UIEventSubtypeMotionShake)
        mBrowser->motionBegan();
        
    if ([super respondsToSelector:@selector(motionBegan:withEvent:)]) {
        [super motionBegan:motion withEvent:event];
    }
}

- (void)motionEnded:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if(mBrowser && event.type == UIEventTypeMotion && event.subtype == UIEventSubtypeMotionShake)
        mBrowser->motionEnded();
        
    if ([super respondsToSelector:@selector(motionEnded:withEvent:)]) {
        [super motionEnded:motion withEvent:event];
    }
}

- (void)motionCancelled:(UIEventSubtype)motion withEvent:(UIEvent *)event {
    if(mBrowser && event.type == UIEventTypeMotion && event.subtype == UIEventSubtypeMotionShake)
        mBrowser->motionCancelled();
        
    if ([super respondsToSelector:@selector(motionCancelled:withEvent:)]) {
        [super motionCancelled:motion withEvent:event];
    }
}
@end

#endif

#endif
