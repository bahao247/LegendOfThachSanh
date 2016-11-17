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
| Filename   : DeferredLightCP.h                                                           
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
| Create File: 21:35:24 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:35:24 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef _DEFERRED_LIGHT_CP_H
#define _DEFERRED_LIGHT_CP_H

#include "OgreCompositorInstance.h"
#include "OgreCustomCompositionPass.h"

#include "DLight.h"
#include "MaterialGenerator.h"
#include "AmbientLight.h"

//The render operation that will be called each frame in the custom composition pass
//This is the class that will send the actual render calls of the spheres (point lights),
//cones (spotlights) and quads (directional lights) after the GBuffer has been constructed
class DeferredLightRenderOperation : public Ogre::CompositorInstance::RenderSystemOperation
{
public:
	DeferredLightRenderOperation(Ogre::CompositorInstance* instance, const Ogre::CompositionPass* pass);
	
	/** @copydoc CompositorInstance::RenderSystemOperation::execute */
	virtual void execute(Ogre::SceneManager *sm, Ogre::RenderSystem *rs);

	virtual ~DeferredLightRenderOperation();
private:

	/** Create a new deferred light
	 */
	DLight *createDLight(Ogre::Light* light);
	
	//The texture names of the GBuffer components
	Ogre::String mTexName0;
	Ogre::String mTexName1;

	//The material generator for the light geometry
	MaterialGenerator* mLightMaterialGenerator;

	//The map of deferred light geometries already constructed
	typedef std::map<Ogre::Light*, DLight*> LightsMap;
	LightsMap mLights;

	//The ambient light used to render the scene
	AmbientLight* mAmbientLight;

	//The viewport that we are rendering to
	Ogre::Viewport* mViewport;
};

//The custom composition pass that is used for rendering the light geometry
//This class needs to be registered with the CompositorManager
class DeferredLightCompositionPass : public Ogre::CustomCompositionPass
{
public:

	/** @copydoc CustomCompositionPass::createOperation */
	virtual Ogre::CompositorInstance::RenderSystemOperation* createOperation(
		Ogre::CompositorInstance* instance, const Ogre::CompositionPass* pass)
	{
		return OGRE_NEW DeferredLightRenderOperation(instance, pass);
	}

protected:
	virtual ~DeferredLightCompositionPass() {}
};

#endif
