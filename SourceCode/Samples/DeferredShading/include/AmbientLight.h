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
| Filename   : AmbientLight.h                                                           
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
| Create File: 21:35:30 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:35:30 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef _AMBIENTLIGHT_H
#define _AMBIENTLIGHT_H

#include "OgreSimpleRenderable.h"

// Renderable for rendering Ambient component and also to
// establish the depths

// Just instantiation is sufficient
// Note that instantiation is necessary to at least establish the depths
// even if the current ambient colour is 0

// its ambient colour is same as the scene's ambient colour

// XXX Could make this a singleton/make it private to the DeferredShadingSystem e.g.

class AmbientLight : public Ogre::SimpleRenderable

{
public:
	AmbientLight();
	~AmbientLight();

	/** @copydoc MovableObject::getBoundingRadius */
	virtual Ogre::Real getBoundingRadius(void) const;
	/** @copydoc Renderable::getSquaredViewDepth */
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const;
	/** @copydoc Renderable::getMaterial */
	virtual const Ogre::MaterialPtr& getMaterial(void) const;

	virtual void getWorldTransforms(Ogre::Matrix4* xform) const;

	void updateFromCamera(Ogre::Camera* camera);
protected:
	Ogre::Real mRadius;
	Ogre::MaterialPtr mMatPtr;
};
 
#endif
