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
| Filename   : LightMaterialGenerator.h                                                           
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
| Create File: 21:34:37 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:34:37 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef H_WJ_LightMaterialGenerator
#define H_WJ_LightMaterialGenerator

#include "MaterialGenerator.h"

class LightMaterialGenerator: public MaterialGenerator
{
public:
	/** Permutation of light materials
	 */
	enum MaterialID
	{
		MI_POINT			= 0x01, // Is a point light
		MI_SPOTLIGHT		= 0x02, // Is a spot light
        MI_DIRECTIONAL		= 0x04, // Is a directional light
		
		MI_ATTENUATED		= 0x08, // Rendered attenuated
		MI_SPECULAR			= 0x10, // Specular component is calculated
		MI_SHADOW_CASTER	= 0x20	// Will cast shadows
	};

	LightMaterialGenerator();
	virtual ~LightMaterialGenerator();
};

#endif
