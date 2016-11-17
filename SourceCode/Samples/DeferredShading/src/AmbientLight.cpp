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
| Filename   : AmbientLight.cpp                                                           
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
| Create File: 21:36:42 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:36:42 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/


#include "AmbientLight.h"
#include "GeomUtils.h"
#include "OgreMaterialManager.h"
#include "OgreRoot.h"
#include "OgreRenderSystem.h"

using namespace Ogre;

AmbientLight::AmbientLight()
{
	setRenderQueueGroup(RENDER_QUEUE_2);

	mRenderOp.vertexData = new VertexData();
	mRenderOp.indexData = 0;

	GeomUtils::createQuad(mRenderOp.vertexData);

	mRenderOp.operationType = RenderOperation::OT_TRIANGLE_STRIP; 
	mRenderOp.useIndexes = false; 

	// Set bounding
	setBoundingBox(AxisAlignedBox(-10000,-10000,-10000,10000,10000,10000));
	mRadius = 15000;

	mMatPtr = MaterialManager::getSingleton().getByName("DeferredShading/AmbientLight");
	assert(mMatPtr.isNull()==false);
	mMatPtr->load();

    // Explicitly bind Gamers for OpenGL
    if(Root::getSingleton().getRenderSystem()->getName().find("OpenGL 3+") != String::npos)
    {
        mMatPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("Tex0", 0);
        mMatPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant("Tex1", 1);
    }

    //This shader needs to be aware if its running under OpenGL or DirectX.
    //Real depthFactor = (Root::getSingleton().getRenderSystem()->getName() ==
    //    "OpenGL Rendering Subsystem") ? 2.0 : 1.0;
    //mMatPtr->getTechnique(0)->getPass(0)->getFragmentProgramParameters()->setNamedConstant(
    //        "depthFactor", depthFactor);
}

AmbientLight::~AmbientLight()
{
	// need to release IndexData and vertexData created for renderable
	delete mRenderOp.indexData;
	delete mRenderOp.vertexData;
}

/** @copydoc MovableObject::getBoundingRadius */
Real AmbientLight::getBoundingRadius(void) const
{
	return mRadius;

}
/** @copydoc Renderable::getSquaredViewDepth */
Real AmbientLight::getSquaredViewDepth(const Camera*) const
{
	return 0.0;
}

const MaterialPtr& AmbientLight::getMaterial(void) const
{
	return mMatPtr;
}

void AmbientLight::getWorldTransforms(Ogre::Matrix4* xform) const
{
	*xform = Matrix4::IDENTITY;
}
void AmbientLight::updateFromCamera(Ogre::Camera* camera)
{
	Ogre::Technique* tech = getMaterial()->getBestTechnique();
	Ogre::Vector3 farCorner = camera->getViewMatrix(true) * camera->getWorldSpaceCorners()[4];

	for (unsigned short i=0; i<tech->getNumPasses(); i++) 
	{
		Ogre::Pass* pass = tech->getPass(i);
		// get the vertex shader parameters
		Ogre::GpuProgramParametersSharedPtr params = pass->getVertexProgramParameters();
		// set the camera's far-top-right corner
		if (params->_findNamedConstantDefinition("farCorner"))
			params->setNamedConstant("farCorner", farCorner);
	    
		params = pass->getFragmentProgramParameters();
		if (params->_findNamedConstantDefinition("farCorner"))
			params->setNamedConstant("farCorner", farCorner);
	}
}
