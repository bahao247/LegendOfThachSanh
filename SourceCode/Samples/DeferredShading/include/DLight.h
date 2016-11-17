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
| Filename   : DLight.h                                                           
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
| Create File: 21:35:01 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:35:01 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef H_WJ_DLight
#define H_WJ_DLight

#include "OgreSimpleRenderable.h"
#include "MaterialGenerator.h"

/** Deferred light geometry. Each instance matches a normal light.
	Should not be created by the user.
	XXX support other types of light other than point lights.
 */
class DLight: public Ogre::SimpleRenderable
{
public:
	DLight(MaterialGenerator *gen, Ogre::Light* parentLight);
	~DLight();

	/** Update the information from the light that matches this one 
	 */
	void updateFromParent();

	/** Update the information that is related to the camera
	 */
	void updateFromCamera(Ogre::Camera* camera);

	/** Does this light cast shadows?
	*/
	virtual bool getCastChadows() const;

	/** @copydoc MovableObject::getBoundingRadius */
	virtual Ogre::Real getBoundingRadius(void) const;
	/** @copydoc Renderable::getSquaredViewDepth */
	virtual Ogre::Real getSquaredViewDepth(const Ogre::Camera*) const;
	/** @copydoc Renderable::getMaterial */
	virtual const Ogre::MaterialPtr& getMaterial(void) const;
	/** @copydoc Renderable::getBoundingRadius */
	virtual void getWorldTransforms(Ogre::Matrix4* xform) const;
protected:

	/** Check if the camera is inside a light
	*/
	bool isCameraInsideLight(Ogre::Camera* camera);

	/** Create geometry for this light.
	*/
	void rebuildGeometry(float radius);

	/** Create a sphere geometry.
	*/
	void createSphere(float radius, int nRings, int nSegments);

	/** Create a rectangle.
	*/
	void createRectangle2D();
	
	/** Create a cone.
	*/
	void createCone(float radius, float height, int nVerticesInBase);

	/** Set constant, linear, quadratic Attenuation terms 
	 */
	void setAttenuation(float c, float b, float a);

	/** Set the specular colour
	 */
	void setSpecularColour(const Ogre::ColourValue &col);

	/// The light that this DLight renders
	Ogre::Light* mParentLight;
	/// Mode to ignore world orientation/position
	bool bIgnoreWorld;
	/// Bounding sphere radius
	float mRadius;
	/// Deferred shading system this minilight is part of
	MaterialGenerator *mGenerator;
	/// Material permutation
	Ogre::uint32 mPermutation;
};

#endif
