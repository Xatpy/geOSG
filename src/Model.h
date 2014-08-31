#pragma once

#ifndef MODEL_H
#define MODEL_H

#include <string>

#include <osg/Texture2D>
#include <osg/TextureCubeMap>
#include <osg/Material>

#include <osgViewer/Viewer>

enum eNodeType { OTHER, GROUP, GEODE, TRANSFORM};

//TODO: Convertir a class y poder hacer herencia.
struct stGeom
{
	std::string								m_vName;
	osg::Texture2D*							m_tDiffuse;
	osg::Texture2D*							m_tNormal;
	osg::Texture2D*							m_tSpecular;
	osg::Texture2D*							m_tOpacity;
	osg::Texture2D*							m_tHeight;
	osg::Texture2D*							m_tDUDV;
	osg::Texture2D*							m_tSkyMap;
	osg::Drawable*							m_pDrawable;
	osg::Uniform*							m_pUniform;

	stGeom () : m_vName(""), m_tDiffuse(0), m_tOpacity(0), m_tSpecular(0), m_tNormal(0), m_tHeight(0), m_tDUDV(0), m_tSkyMap(0), m_pDrawable(0), m_pUniform(0) {}
};

class Model
{
public:
	void									LoadModel													(std::string vFile);

	osg::Node*								GetNode														()													{ return m_node; }

	std::vector<stGeom*>					GetGeoms													()													{ return m_vGeoms; }

	void									Update														();

	std::string								GetShader													(stGeom geom);
	void										CheckShading											();
	void									ApplyShader												(std::string vNameShader, int iContDrawable);

	bool									GetShadowed												()																										{ return m_bShadowed; }
	void										SetShadowed													(bool bValue)																							{ m_bShadowed = bValue; }

	void										SetNumLights												(int iValue)																							{ m_iNumLights = iValue; }

	void										SetUniformVec4										(osg::Vec4 vec);
	void										SetUniformInt											(int iValue);
	osg::Material*							GetMaterial												(osg::StateSet* stateset);

	void										SetFogDensity											(float fInc);

public:
											Model												(bool bDegud = false);
											~Model												();
private:
	void									ExploreNodeDepth									();
	void									ExploreNodeDepth									(osg::Node* gr);
	void									ExploreGeode										(osg::Geode* ge);
	void									ExploreDrawable										(osg::Drawable* dr);
	void									ExploreTexture										(std::string vNameTexture, osg::Texture2D* tex, stGeom *geom);

	eNodeType								GetNodeType											(osg::Node *gr);

	void									ShaderTest											(osg::StateSet* stateset, int iContDrawable);
	void									ShaderTestFog										(osg::StateSet* stateset, int iContDrawable);
	void									ShaderPerPixel										(osg::StateSet* stateset, int iContDrawable);
	void									ShaderParallaxTotal									(osg::StateSet* stateset, osg::Program* programObject, int iContDrawable);
	void									ShaderParallaxShadow								(osg::StateSet* stateset, osg::Program* programObject, int iContDrawable);

	void									ShaderWater											(osg::StateSet* stateset, osg::Program* programObject, int iContDrawable);
	void									ShaderMovement										(osg::StateSet* stateset);

	osg::Texture2D*							CreateTexture										(const std::string& filename );

	void									SetTangent											(osg::Geometry *tmpGeo, osg::Program* programObject, int iNormalMapTexUnit = 0);
	void									SetInitialFog										(osg::StateSet* stateset);
	void									SetTransparency										(osg::StateSet *stateset);

	void									SaveModelPath										(std::string vFile);


private:
	bool									m_bDebug;

	osg::Node*								m_node;
	std::vector<stGeom*>					m_vGeoms;

	bool									m_bShadowed;	
	int										m_iNumLights;

	std::string								m_vPathModel;
};
#endif
