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
| Filename   : MaterialControls.h                                                           
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
| Create File: 21:32:47 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:32:47 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/

#ifndef __MaterialControls_H__
#define __MaterialControls_H__

#include "OgreString.h"

enum ShaderValType
{
	GPU_VERTEX, GPU_FRAGMENT, MAT_SPECULAR, MAT_DIFFUSE, MAT_AMBIENT, MAT_SHININESS, MAT_EMISSIVE
};

//---------------------------------------------------------------------------
struct ShaderControl
{
    Ogre::String Name;
	Ogre::String ParamName;
	ShaderValType ValType;
	float MinVal;
	float MaxVal;
	size_t ElementIndex;
	mutable size_t PhysicalIndex;

	float getRange(void) const { return MaxVal - MinVal; }
	float convertParamToScrollPosition(const float val) const { return val - MinVal; }
	float convertScrollPositionToParam(const float val) const { return val + MinVal; }
};

typedef Ogre::vector<ShaderControl>::type ShaderControlsContainer;
typedef ShaderControlsContainer::iterator ShaderControlIterator;
// used for materials that have user controls

//---------------------------------------------------------------------------
class MaterialControls
{
public:
    MaterialControls(const Ogre::String& displayName, const Ogre::String& materialName)
        : mDisplayName(displayName)
        , mMaterialName(materialName)
    {
    };

    ~MaterialControls(void){}

    const Ogre::String& getDisplayName(void) const { return mDisplayName; }
    const Ogre::String& getMaterialName(void) const { return mMaterialName; }
    size_t getShaderControlCount(void) const { return mShaderControlsContainer.size(); }
    const ShaderControl& getShaderControl(const size_t idx) const
    {
        assert( idx < mShaderControlsContainer.size() );
        return mShaderControlsContainer[idx];
    }
    /** add a new control by passing a string parameter

    @param
      params is a string using the following format:
        "<Control Name>, <Shader parameter name>, <Parameter Type>, <Min Val>, <Max Val>, <Parameter Sub Index>"

        <Control Name> is the string displayed for the control name on screen
        <Shader parameter name> is the name of the variable in the shader
        <Parameter Type> can be GPU_VERTEX, GPU_FRAGMENT
        <Min Val> minimum value that parameter can be
        <Max Val> maximum value that parameter can be
        <Parameter Sub Index> index into the the float array of the parameter.  All GPU parameters are assumed to be float[4].

    */
    void addControl(const Ogre::String& params);

protected:

    Ogre::String mDisplayName;
    Ogre::String mMaterialName;

    ShaderControlsContainer mShaderControlsContainer;
};

typedef Ogre::vector<MaterialControls>::type MaterialControlsContainer;
typedef MaterialControlsContainer::iterator MaterialControlsIterator;

//---------------------------------------------------------------------------
/** loads material shader controls from a configuration file
    A .controls file is made up of the following:

    [<material display name>]
    material = <material name>
    control = <Control Name>, <Shader parameter name>, <Parameter Type>, <Min Val>, <Max Val>, <Parameter Sub Index>

    <material display name> is what is displayed in the material combo box.
    <material name> is the name of the material in the material script.
    control is the shader control associated with the material. The order
    of the contol definitions in the .controls file determines their order
    when displayed in the controls window.

    you can have multiple .controls files or put them all in one.
*/
void loadMaterialControlsFile(MaterialControlsContainer& controlsContainer, const Ogre::String& filename);
/** load all control files found in resource paths
*/
void loadAllMaterialControlFiles(MaterialControlsContainer& controlsContainer);

#endif // __MaterialControls_H__
