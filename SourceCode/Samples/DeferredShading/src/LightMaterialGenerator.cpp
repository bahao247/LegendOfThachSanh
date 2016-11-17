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
| Filename   : LightMaterialGenerator.cpp                                                           
---------------------------------------------------------------------------------------
| Written in : C++                                                     
---------------------------------------------------------------------------------------
| Compiler   : Microsoft Visual C++ 2010 Service Pack 1.                               
---------------------------------------------------------------------------------------
| Engine     : Ogre3D; ....................................                                                                     
---------------------------------------------------------------------------------------
| Written by : Ba Hao Nguyen � Sports editors, IT.                                
---------------------------------------------------------------------------------------
| Note       : ....................................                                
---------------------------------------------------------------------------------------
| Create File: 21:35:58 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
| Last Update: 21:35:58 15/10/2016 by bahao                                                             
---------------------------------------------------------------------------------------
*/
#include "LightMaterialGenerator.h"

#include "OgreStringConverter.h"
#include "OgreException.h"
#include "OgreMaterialManager.h"

#include "OgrePass.h"
#include "OgreTechnique.h"

#include "OgreGpuProgramManager.h"
#include "OgreHighLevelGpuProgram.h"
#include "OgreHighLevelGpuProgramManager.h"

#include "DLight.h"

using namespace Ogre;

//CG
class LightMaterialGeneratorCG : public MaterialGenerator::Impl
{
public:
	typedef MaterialGenerator::Perm Perm;
	LightMaterialGeneratorCG(const String &baseName):
	    mBaseName(baseName) 
	{

	}
	virtual ~LightMaterialGeneratorCG()
	{

	}

	virtual GpuProgramPtr generateVertexShader(Perm permutation)
	{
        String programName = "DeferredShading/post/";

		if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
		{
			programName += "vs";
		}
		else
		{
			programName += "LightMaterial_vs";
		}

		GpuProgramPtr ptr = HighLevelGpuProgramManager::getSingleton().getByName(programName);
		assert(!ptr.isNull());
		return ptr;
	}

	virtual GpuProgramPtr generateFragmentShader(Perm permutation)
	{
		/// Create shader
		if (mMasterSource.empty())
		{
			DataStreamPtr ptrMasterSource = ResourceGroupManager::getSingleton().openResource(
				 "DeferredShading/post/LightMaterial_ps.cg"
				, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
			assert(ptrMasterSource.isNull()==false);
			mMasterSource = ptrMasterSource->getAsString();
		}

		assert(mMasterSource.empty()==false);

		// Create name
		String name = mBaseName+StringConverter::toString(permutation)+"_ps";		

		// Create shader object
		HighLevelGpuProgramPtr ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(
			name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			"cg", GPT_FRAGMENT_PROGRAM);
		ptrProgram->setSource(mMasterSource);
		ptrProgram->setParameter("entry_point","main");
	    ptrProgram->setParameter("profiles","ps_2_x arbfp1");
		// set up the preprocessor defines
		// Important to do this before any call to get parameters, i.e. before the program gets loaded
		ptrProgram->setParameter("compile_arguments", getPPDefines(permutation));

		setUpBaseParameters(ptrProgram->getDefaultParameters());

		return GpuProgramPtr(ptrProgram);
	}

	virtual MaterialPtr generateTemplateMaterial(Perm permutation)
	{
		String materialName = mBaseName;
	
        if(permutation & LightMaterialGenerator::MI_DIRECTIONAL)
		{   
			materialName += "Quad";
		}
		else
		{
			materialName += "Geometry";
		}

		if(permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
		{
			materialName += "Shadow";
		}
		return MaterialManager::getSingleton().getByName(materialName);
	}

	protected:
		String mBaseName;
        String mMasterSource;
		// Utility method
		String getPPDefines(Perm permutation)
		{
			String strPPD;

			//Get the type of light
			String lightType;
			if (permutation & LightMaterialGenerator::MI_POINT)
			{
				lightType = "POINT";
			}
			else if (permutation & LightMaterialGenerator::MI_SPOTLIGHT)
			{
				lightType = "SPOT";
			}
			else if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
			{
				lightType = "DIRECTIONAL";
			}
			else
			{
				assert(false && "Permutation must have a light type");
			}
			strPPD += "-DLIGHT_TYPE=LIGHT_" + lightType + " ";

			//Optional parameters
            if (permutation & LightMaterialGenerator::MI_SPECULAR)
			{
				strPPD += "-DIS_SPECULAR ";
			}
			if (permutation & LightMaterialGenerator::MI_ATTENUATED)
			{
				strPPD += "-DIS_ATTENUATED ";
			}
			if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
			{
				strPPD += "-DIS_SHADOW_CASTER ";
			}
			return strPPD;
		}

		void setUpBaseParameters(const GpuProgramParametersSharedPtr& params)
		{
			assert(params.isNull()==false);

			struct AutoParamPair { String name; GpuProgramParameters::AutoConstantType type; };	

			//A list of auto params that might be present in the shaders generated
			static const AutoParamPair AUTO_PARAMS[] = {
				{ "vpWidth",			GpuProgramParameters::ACT_VIEWPORT_WIDTH },
				{ "vpHeight",			GpuProgramParameters::ACT_VIEWPORT_HEIGHT },
				{ "worldView",			GpuProgramParameters::ACT_WORLDVIEW_MATRIX },
				{ "invProj",			GpuProgramParameters::ACT_INVERSE_PROJECTION_MATRIX },
				{ "invView",			GpuProgramParameters::ACT_INVERSE_VIEW_MATRIX },
				{ "flip",				GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING },
				{ "lightDiffuseColor",	GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR },
				{ "lightSpecularColor", GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR },
				{ "lightFalloff",		GpuProgramParameters::ACT_LIGHT_ATTENUATION },
				{ "lightPos",			GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE },
				{ "lightDir",			GpuProgramParameters::ACT_LIGHT_DIRECTION_VIEW_SPACE },
				{ "spotParams",			GpuProgramParameters::ACT_SPOTLIGHT_PARAMS },
				{ "farClipDistance",	GpuProgramParameters::ACT_FAR_CLIP_DISTANCE },
				{ "shadowViewProjMat",	GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX }
			};
			int numParams = sizeof(AUTO_PARAMS) / sizeof(AutoParamPair);

			for (int i=0; i<numParams; i++)
			{
				if (params->_findNamedConstantDefinition(AUTO_PARAMS[i].name))
				{
					params->setNamedAutoConstant(AUTO_PARAMS[i].name, AUTO_PARAMS[i].type);
				}
			}
		}
};

//GLSL
class LightMaterialGeneratorGLSL : public MaterialGenerator::Impl
{
public:
	typedef MaterialGenerator::Perm Perm;
	LightMaterialGeneratorGLSL(const String &baseName):
    mBaseName(baseName)
	{

	}
	virtual ~LightMaterialGeneratorGLSL()
	{

	}

	virtual GpuProgramPtr generateVertexShader(Perm permutation)
	{
        String programName = "DeferredShading/post/";

		if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
		{
			programName += "vs";
		}
		else
		{
			programName += "LightMaterial_vs";
		}

		GpuProgramPtr ptr = HighLevelGpuProgramManager::getSingleton().getByName(programName);
		assert(!ptr.isNull());
		return ptr;
	}

	virtual GpuProgramPtr generateFragmentShader(Perm permutation)
	{
		/// Create shader
		if (mMasterSource.empty())
		{
			DataStreamPtr ptrMasterSource;
            if(GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
                ptrMasterSource = ResourceGroupManager::getSingleton().openResource("DeferredShading/post/LightMaterial_ps.glsles",
                                                                                    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
            else
                ptrMasterSource = ResourceGroupManager::getSingleton().openResource("DeferredShading/post/LightMaterial_ps.glsl",
                                                                                    ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);

			assert(ptrMasterSource.isNull()==false);
			mMasterSource = ptrMasterSource->getAsString();
		}

		assert(mMasterSource.empty()==false);

		// Create name
		String name = mBaseName+StringConverter::toString(permutation)+"_ps";

		// Create shader object
		HighLevelGpuProgramPtr ptrProgram;
        if(GpuProgramManager::getSingleton().isSyntaxSupported("glsles"))
        {
            ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                                  "glsles", GPT_FRAGMENT_PROGRAM);
            ptrProgram->setParameter("profiles", "glsles");
        }
        else
        {
            ptrProgram = HighLevelGpuProgramManager::getSingleton().createProgram(name, ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
                                                                                  "glsl", GPT_FRAGMENT_PROGRAM);
            ptrProgram->setParameter("profiles", "glsl150");
        }
        ptrProgram->setSource(mMasterSource);
		// set up the preprocessor defines
		// Important to do this before any call to get parameters, i.e. before the program gets loaded
		ptrProgram->setParameter("preprocessor_defines", getPPDefines(permutation));

		setUpBaseParameters(ptrProgram->getDefaultParameters());

        // Bind Gamers
		GpuProgramParametersSharedPtr params = ptrProgram->getDefaultParameters();
        int numGamers = 0;
        params->setNamedConstant("Tex0", (int)numGamers++);
        params->setNamedConstant("Tex1", (int)numGamers++);

        if(permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
            params->setNamedConstant("ShadowTex", (int)numGamers++);

		return GpuProgramPtr(ptrProgram);
	}

	virtual MaterialPtr generateTemplateMaterial(Perm permutation)
	{
		String materialName = mBaseName;

        if(permutation & LightMaterialGenerator::MI_DIRECTIONAL)
		{
			materialName += "Quad";
		}
		else
		{
			materialName += "Geometry";
		}

		if(permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
		{
			materialName += "Shadow";
		}
		return MaterialManager::getSingleton().getByName(materialName);
	}

protected:
    String mBaseName;
    String mMasterSource;
    // Utility method
    String getPPDefines(Perm permutation)
    {
        String strPPD;

        //Get the type of light
        Ogre::uint lightType = 0;
        if (permutation & LightMaterialGenerator::MI_POINT)
        {
            lightType = 1;
        }
        else if (permutation & LightMaterialGenerator::MI_SPOTLIGHT)
        {
            lightType = 2;
        }
        else if (permutation & LightMaterialGenerator::MI_DIRECTIONAL)
        {
            lightType = 3;
        }
        else
        {
            assert(false && "Permutation must have a light type");
        }
        strPPD += "LIGHT_TYPE=" + StringConverter::toString(lightType);

        //Optional parameters
        if (permutation & LightMaterialGenerator::MI_SPECULAR)
        {
            strPPD += ",IS_SPECULAR=1";
        }
        if (permutation & LightMaterialGenerator::MI_ATTENUATED)
        {
            strPPD += ",IS_ATTENUATED=1";
        }
        if (permutation & LightMaterialGenerator::MI_SHADOW_CASTER)
        {
            strPPD += ",IS_SHADOW_CASTER=1";
        }
        return strPPD;
    }

    void setUpBaseParameters(const GpuProgramParametersSharedPtr& params)
    {
        assert(params.isNull()==false);

        struct AutoParamPair { String name; GpuProgramParameters::AutoConstantType type; };

        //A list of auto params that might be present in the shaders generated
        static const AutoParamPair AUTO_PARAMS[] = {
            { "vpWidth",            GpuProgramParameters::ACT_VIEWPORT_WIDTH },
            { "vpHeight",           GpuProgramParameters::ACT_VIEWPORT_HEIGHT },
            { "worldView",          GpuProgramParameters::ACT_WORLDVIEW_MATRIX },
            { "invProj",            GpuProgramParameters::ACT_INVERSE_PROJECTION_MATRIX },
            { "invView",            GpuProgramParameters::ACT_INVERSE_VIEW_MATRIX },
            { "flip",               GpuProgramParameters::ACT_RENDER_TARGET_FLIPPING },
            { "lightDiffuseColor",  GpuProgramParameters::ACT_LIGHT_DIFFUSE_COLOUR },
            { "lightSpecularColor", GpuProgramParameters::ACT_LIGHT_SPECULAR_COLOUR },
            { "lightFalloff",       GpuProgramParameters::ACT_LIGHT_ATTENUATION },
            { "lightPos",           GpuProgramParameters::ACT_LIGHT_POSITION_VIEW_SPACE },
            { "lightDir",           GpuProgramParameters::ACT_LIGHT_DIRECTION_VIEW_SPACE },
            { "spotParams",         GpuProgramParameters::ACT_SPOTLIGHT_PARAMS },
            { "farClipDistance",    GpuProgramParameters::ACT_FAR_CLIP_DISTANCE },
            { "shadowViewProjMat",  GpuProgramParameters::ACT_TEXTURE_VIEWPROJ_MATRIX }
        };
        int numParams = sizeof(AUTO_PARAMS) / sizeof(AutoParamPair);
        
        for (int i=0; i<numParams; i++)
        {
            if (params->_findNamedConstantDefinition(AUTO_PARAMS[i].name))
            {
                params->setNamedAutoConstant(AUTO_PARAMS[i].name, AUTO_PARAMS[i].type);
            }
        }
    }
};

LightMaterialGenerator::LightMaterialGenerator()
{
	vsMask = 0x00000004;
	fsMask = 0x0000003F;
	matMask =	LightMaterialGenerator::MI_DIRECTIONAL | 
				LightMaterialGenerator::MI_SHADOW_CASTER;
	
	materialBaseName = "DeferredShading/LightMaterial/";
    if ((GpuProgramManager::getSingleton().isSyntaxSupported("glsl") || GpuProgramManager::getSingleton().isSyntaxSupported("glsles")) &&
        !(GpuProgramManager::getSingleton().isSyntaxSupported("ps_2_x") ||GpuProgramManager::getSingleton().isSyntaxSupported("arbfp1")))
        mImpl = new LightMaterialGeneratorGLSL("DeferredShading/LightMaterial/");
    else
        mImpl = new LightMaterialGeneratorCG("DeferredShading/LightMaterial/");
}

LightMaterialGenerator::~LightMaterialGenerator()
{

}
