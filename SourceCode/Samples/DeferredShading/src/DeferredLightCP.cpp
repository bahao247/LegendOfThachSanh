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
| Filename   : DeferredLightCP.cpp                                                           
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
| Create File: 21:36:37 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:36:37 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#include "DeferredLightCP.h"

#include "Ogre.h"
using namespace Ogre;

#include "LightMaterialGenerator.h"

//-----------------------------------------------------------------------
DeferredLightRenderOperation::DeferredLightRenderOperation(
	CompositorInstance* instance, const CompositionPass* pass)
{
	mViewport = instance->getChain()->getViewport();
	
	//Get the names of the GBuffer textures
	const CompositionPass::InputTex& input0 = pass->getInput(0);
	mTexName0 = instance->getTextureInstanceName(input0.name, input0.mrtIndex);
	const CompositionPass::InputTex& input1 = pass->getInput(1);
	mTexName1 = instance->getTextureInstanceName(input1.name, input1.mrtIndex);

	// Create lights material generator
	mLightMaterialGenerator = new LightMaterialGenerator();
	
	// Create the ambient light
	mAmbientLight = new AmbientLight();
	const MaterialPtr& mat = mAmbientLight->getMaterial();
	mat->load();
}
//-----------------------------------------------------------------------
DLight* DeferredLightRenderOperation::createDLight(Ogre::Light* light)
{
	DLight *rv = new DLight(mLightMaterialGenerator,light);
	mLights[light] = rv;
	return rv;
}
//-----------------------------------------------------------------------
void injectTechnique(SceneManager* sm, Technique* tech, Renderable* rend, const Ogre::LightList* lightList)
{
    for(unsigned short i=0; i<tech->getNumPasses(); ++i)
	{
		Ogre::Pass* pass = tech->getPass(i);
        if (lightList != 0) 
		{
			sm->_injectRenderWithPass(pass, rend, false, false, lightList);
		} 
		else
		{
			sm->_injectRenderWithPass(pass, rend, false);
		}
		
	}
}
//-----------------------------------------------------------------------
void DeferredLightRenderOperation::execute(SceneManager *sm, RenderSystem *rs)
{
    Ogre::Camera* cam = mViewport->getCamera();

	mAmbientLight->updateFromCamera(cam);
    Technique* tech = mAmbientLight->getMaterial()->getBestTechnique();
	injectTechnique(sm, tech, mAmbientLight, 0);

	const LightList& lightList = sm->_getLightsAffectingFrustum();
    for (LightList::const_iterator it = lightList.begin(); it != lightList.end(); it++) 
	{
        Light* light = *it;
		Ogre::LightList ll;
		ll.push_back(light);

		//if (++i != 2) continue;
        //if (light->getType() != Light::LT_DIRECTIONAL) continue;
		//if (light->getDiffuseColour() != ColourValue::Red) continue;

		LightsMap::iterator dLightIt = mLights.find(light);
		DLight* dLight = 0;
		if (dLightIt == mLights.end()) 
		{
			dLight = createDLight(light);
		}
		else 
		{
			dLight = dLightIt->second;
			dLight->updateFromParent();
		}
		dLight->updateFromCamera(cam);
		tech = dLight->getMaterial()->getBestTechnique();

		//Update shadow texture
		if (dLight->getCastChadows())
		{
			SceneManager::RenderContext* context = sm->_pauseRendering();

			sm->prepareShadowTextures(cam, mViewport, &ll);
			sm->_resumeRendering(context);
			
			Pass* pass = tech->getPass(0);
			TextureUnitState* tus = pass->getTextureUnitState("ShadowMap");
			assert(tus);
			const TexturePtr& shadowTex = sm->getShadowTexture(0);
			if (tus->_getTexturePtr() != shadowTex)
			{
				tus->_setTexturePtr(shadowTex);
			}
			
		}
		
        injectTechnique(sm, tech, dLight, &ll);
	}
}
//-----------------------------------------------------------------------
DeferredLightRenderOperation::~DeferredLightRenderOperation()
{
	for (LightsMap::iterator it = mLights.begin(); it != mLights.end(); ++it)
	{
		delete it->second;
	}
	mLights.clear();
	
	delete mAmbientLight;
	delete mLightMaterialGenerator;
}
//-----------------------------------------------------------------------
