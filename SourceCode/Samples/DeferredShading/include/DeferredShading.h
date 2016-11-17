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
| Filename   : DeferredShading.h                                                           
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
| Create File: 21:35:16 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:35:16 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef H_DeferredShadingSystem
#define H_DeferredShadingSystem

#include "Ogre.h"
#include "SdkGame.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#include "OgreCompositorInstance.h"
#include "OgreSceneManager.h"
#include "OgreSceneNode.h"
#include "OgreMaterial.h"
#include "OgreRenderTargetListener.h"
#include "GeomUtils.h"

using namespace Ogre;
using namespace OgreBites;

/** System to manage Deferred Shading for a camera/render target.
 *  @note With the changes to the compositor framework, this class just
 *		selects which compositors to enable.
 */
class DeferredShadingSystem : public Ogre::RenderTargetListener
{
public:
	DeferredShadingSystem(Ogre::Viewport *vp, Ogre::SceneManager *sm, Ogre::Camera *cam);
	~DeferredShadingSystem();

	enum DSMode
	{
		DSM_SHOWLIT = 0,     // The deferred shading mode
		DSM_SHOWCOLOUR = 1,  // Show diffuse (for debugging)
		DSM_SHOWNORMALS = 2, // Show normals (for debugging)
		DSM_SHOWDSP = 3,	 // Show depth and specular channel (for debugging)
		DSM_COUNT = 4
	};

	//The first render queue that does get rendered into the GBuffer
	//place objects (like skies) that should be before gbuffer before this one.
	static const Ogre::uint8 PRE_GBUFFER_RENDER_QUEUE;
	
	//The first render queue that does not get rendered into the GBuffer
	//place transparent (or other non gbuffer) objects after this one
	static const Ogre::uint8 POST_GBUFFER_RENDER_QUEUE;

	void initialize();

	/** Set rendering mode (one of DSMode)
	 */
	void setMode(DSMode mode);

	DSMode getMode(void) const;

	/** Set screen space ambient occlusion mode
	 */
	void setSSAO(bool ssao);
	
	bool getSSAO() const;

	/** Activate or deactivate system
	 */
	void setActive(bool active);
	
protected:
	Ogre::Viewport *mViewport;
	Ogre::SceneManager *mSceneMgr;
	Ogre::Camera *mCamera;
	
	Ogre::CompositorInstance *mGBufferInstance;
	// Filters
	Ogre::CompositorInstance *mInstance[DSM_COUNT];
	Ogre::CompositorInstance* mSSAOInstance;
	// Active/inactive
	bool mActive;
	DSMode mCurrentMode;
	bool mSSAO;

	//Used to unregister compositor logics and free memory
	typedef map<String, CompositorLogic*>::type CompositorLogicMap;
	CompositorLogicMap mCompositorLogics;

	void createResources();
	
	void logCurrentMode(void);
};

const ColourValue Game_COLORS[] = 
{   ColourValue::Red, ColourValue::Green, ColourValue::Blue, 
    ColourValue::White, ColourValue(1,1,0,1), ColourValue(1,0,1,1)
};

#endif
