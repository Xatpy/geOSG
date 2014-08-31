#include "Model.h"

#include <osgDB/ReadFile>

#include <osgUtil/TangentSpaceGenerator>
#include <osgUtil\SceneView>

#include <osgViewer\Renderer>

#include <osg/State>
#include <osg/Fog>

//shader movimiento
class MovementCallback : public osg::Uniform::Callback
{
public:
	MovementCallback()	 {};
	virtual void operator()( osg::Uniform* uniform, osg::NodeVisitor* nv )
	{
		const osg::FrameStamp* fs = nv->getFrameStamp();
		if ( !fs ) return;

		int paso = fs->getFrameNumber();
		uniform->set( paso );
	}
};

Model::Model(bool bDebug)	: m_bShadowed(false), m_iNumLights(3), m_vPathModel("")
{
	m_bDebug = bDebug;	
}

Model::~Model()
{
}

void Model::SaveModelPath(std::string vFile)
{
	size_t mark = 0;
	if (vFile.find_last_of('/') != std::string::npos)
	{
		mark = vFile.find_last_of('/');
	}
	else if (vFile.find_last_of("\\") != std::string::npos)
		mark = vFile.find_last_of("\\");

	if (mark > 0)
	{
		m_vPathModel = vFile.substr(0, mark);
		m_vPathModel += "/";
	}
}

void Model::LoadModel(std::string vFile)
{
	m_node = osgDB::readNodeFile(vFile.c_str()) ;		

	if (m_node)
	{
		SaveModelPath(vFile);

		ExploreNodeDepth(m_node);

		//Check shading and if so, apply
		CheckShading();
	}
	else
	{
		std::cout << "Could not load " << vFile << std::endl;
	}
}

void Model::ExploreTexture(std::string vNameTexture, osg::Texture2D* tex, stGeom *geom)
{
	tex->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
	tex->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);
	tex->setFilter(osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR);
	tex->setFilter(osg::Texture::MAG_FILTER, osg::Texture::LINEAR);   

	if (vNameTexture.find("DIFFUSE") != std::string::npos)
	{
		geom->m_tDiffuse = tex;
	}
	else if (vNameTexture.find("NORMAL") != std::string::npos)
	{
		geom->m_tNormal = tex;
	}
	else if (vNameTexture.find("SPECULAR") != std::string::npos)
	{
		geom->m_tSpecular = tex;
	}
	else if (vNameTexture.find("OPACITY") != std::string::npos)
	{
		geom->m_tOpacity = tex;
	}
	else if (vNameTexture.find("HEIGHT") != std::string::npos)
	{
		geom->m_tHeight = tex;
	}
	else if (vNameTexture.find("DUDV") != std::string::npos)
	{
		geom->m_tDUDV = tex;
	}
	else if (m_bDebug) //texture name doen't match for shading
	{		
		std::cout << "Texture " << vNameTexture << " not 'saved' for shading. " << std::endl;
	}
}

//Explora el drawable y cargara sus texturas para los shaders en función de los nombres de las texturas.
void Model::ExploreDrawable(osg::Drawable* dr)
{
	//Extract state set from drawable
	if (dr)
	{
		osg::StateSet *st = dr->getStateSet();

		//Save pointer to that drawable
		//m_vDrawables.push_back(dr);
		if (st)
		{
			//Get texture list of that stateset
			osg::StateSet::TextureAttributeList listText = st->getTextureAttributeList();
			if (listText.size() > 0)
			{
				stGeom* geom = new stGeom;
				geom->m_pDrawable = dr;
				geom->m_vName = dr->getName();

				for (int i = 0; i < listText.size(); ++i)
				{
					osg::Texture2D* tex = dynamic_cast<osg::Texture2D*>(st->getTextureAttribute(i, osg::StateAttribute::TEXTURE));
					if ( (tex) && (tex->getImage()) )
					{
						std::string nameTexture = tex->getImage()->getFileName();

						//Convert to uppercase to check naming
						std::transform(nameTexture.begin(), nameTexture.end(),nameTexture.begin(), ::toupper);
						if (m_bDebug)
						{
							std::cout << "nameTexture " << nameTexture << std::endl;
						}

						ExploreTexture(nameTexture, tex, geom);
					}//if (tex)
				}//for listText

				//Save the geometry
				m_vGeoms.push_back(geom);
			} // if listText.size > 0
		}//if (st)
	}
}

void Model::ExploreGeode(osg::Geode* ge)
{
	if (!ge)
		return;	
	
	//Go through drawables
	osg::Geode::DrawableList list;
	list = ge->getDrawableList();
	for (int i = 0; i<list.size(); ++i)
	{
		//std::cout << list[i]->getName() << std::endl;
		ExploreDrawable(list[i]);
	}	
}

void Model::ExploreNodeDepth(osg::Node* gr)
{		
	/*if (m_bDebug)
	{
		std::cout <<"Node name = " << m_node->getName() << std::endl;
	}*/

	if (!gr)
		return;

	eNodeType type = GetNodeType(gr);
	if (type == eNodeType::GROUP)
	{
		for (int i = 0; i < gr->asGroup()->getNumChildren(); ++i)
		{
			ExploreNodeDepth(gr->asGroup()->getChild(i));
			//Exit recursivity
		}
	}
	else if (type == eNodeType::GEODE)
	{
		//if (m_bDebug)
			//std::cout << "Type = Geode" << std::endl;
		ExploreGeode(gr->asGeode());
	}
	else if (type == eNodeType::TRANSFORM)
	{
		//if (m_bDebug)
			//std::cout << "Type = Transform" << std::endl;
	}
	return;
}

//Return true if it is a group
eNodeType Model::GetNodeType(osg::Node* node)
{
	//Compruebo si el nodo es un drawable
	osg::Node* nd = node->asGeode();
	if (nd)
	{
		/*if (m_bDebug)
		{
			std::cout << "It is a geode" << std::endl;
		}*/	
		return eNodeType::GEODE;
	}	
	else
	{
		nd = node->asGroup();
		if (nd)
		{
			/*if (m_bDebug)
			{
				std::cout << "It is a group" << std::endl;
			}*/		
			return eNodeType::GROUP;
		}
		else
		{
			nd = node->asTransform();
			if (nd)
			{
				/*if (m_bDebug)
				{
					std::cout << "It is a transform" << std::endl;
				}*/	
				return eNodeType::TRANSFORM;
			}
		}
	}

	return eNodeType::OTHER; //es un geode o un transform
}

//Checks shader type depending on textures
std::string	Model::GetShader(stGeom geom)
{
	std::string ret("");

	//If shader, there is diffuse texture at least
	if (geom.m_tDiffuse)
	{
		if ( (!geom.m_tNormal) && (!geom.m_tSpecular) && (!geom.m_tOpacity) && (!geom.m_tHeight) && (!geom.m_tDUDV) )
			//ret = "PerPixelLighting";
			ret = "PhongPointLight";
		else if ( (geom.m_tNormal) && (geom.m_tHeight) && (!geom.m_tDUDV) && (!geom.m_tOpacity))
		{
			ret = (m_iNumLights !=  1) ? "ParallaxTotal" : "ParallaxTotal_1Light";									
		}
		else if ( (geom.m_tNormal) && (geom.m_tHeight) && (!geom.m_tDUDV) && (geom.m_tOpacity))
			ret = "ParallaxTotalOpacity";
		else if ( (geom.m_tNormal) && (geom.m_tDUDV) )
			ret = "Water";
		/*
		else if ( (geom.m_tNormal) && (!geom.m_tSpecular) && (!geom.m_tOpacity) && (!geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "Normal";
		else if ( (geom.m_tNormal) && (geom.m_tSpecular) && (!geom.m_tOpacity) && (!geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "NormalSpecular";
		else if ( (geom.m_tNormal) && (geom.m_tSpecular) && (geom.m_tOpacity) && (!geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "NormalSpecularOpacity";
		else if ( (geom.m_tNormal) && (geom.m_tSpecular) && (geom.m_tOpacity) && (!geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "NormalSpecularOpacity";
		else if ( (geom.m_tNormal) && (!geom.m_tSpecular) && (!geom.m_tOpacity) && (geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "Parallax";
		else if ( (geom.m_tNormal) && (geom.m_tSpecular) && (!geom.m_tOpacity) && (geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "ParallaxSpecular";
		else if ( (geom.m_tNormal) && (geom.m_tSpecular) && (geom.m_tOpacity) && (geom.m_tHeight) && (!geom.m_tDUDV))
			ret = "ParallaxOpacity";

		/*else if ( (!geom.m_tNormal) && (geom.m_tSpecular) && (!geom.m_tOpacity) )
			ret = "Specular";
		else if ( (!geom.m_tNormal) && (!geom.m_tSpecular) && (geom.m_tOpacity) )
			ret = "Opacity";
		else if ( (!geom.m_tNormal) && (geom.m_tSpecular) && (!geom.m_tOpacity) )
			ret = "SpecularOpacity";*/
	}	

	return ret;
}

void Model::CheckShading()
{
	//Go through all geometries and check if there are shaders 
	for (int i = 0; i < m_vGeoms.size(); ++i)
	{
		std::string vShaderName = GetShader(*m_vGeoms[i]);	
		
		if (m_bShadowed)
		{
			vShaderName = "ParallaxShadow";
		}		

		//vShaderName = "ParallaxTotal_1Light";

		if (!vShaderName.empty())
		{
			if (m_bDebug)
			{
				std::cout << "Shader applied: " << vShaderName << " - Node: " << m_vGeoms[i]->m_vName << std::endl;
			}
			ApplyShader(vShaderName, i);
		}		
	}
}

void Model::ApplyShader(std::string vNameShader, int iContDrawable)
{
	//Load shader file
	osg::StateSet* stateset = new osg::StateSet;
	osg::Program *programObject = new osg::Program;
	programObject->setName(vNameShader.c_str());	
	osg::Shader *vertexObject = new osg::Shader (osg::Shader::VERTEX);
	osg::Shader *fragmentObject = new osg::Shader (osg::Shader::FRAGMENT);	
	programObject->addShader(vertexObject);
	programObject->addShader(fragmentObject);
	std::string vertShader = m_vPathModel + "shaders/";	
	std::string fragShader = m_vPathModel + "shaders/";
	vertShader += vNameShader;
	fragShader += vNameShader;
	vertShader += ".vert";
	fragShader += ".frag";
	vertexObject->loadShaderSourceFromFile(vertShader);
	fragmentObject->loadShaderSourceFromFile(fragShader);	
	//Apply shader
	stateset->setAttributeAndModes(programObject, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);			

	//Double face off
	stateset->setMode( GL_CULL_FACE, osg::StateAttribute::ON ); 

	//Shader configuration
	if ( (vNameShader == "PerPixelLighting") || (vNameShader == "PhongPointLight") )
	{
		ShaderPerPixel(stateset, iContDrawable);
	}
	else if ( (vNameShader == "ParallaxTotal") || (vNameShader == "ParallaxTotalOpacity") || (vNameShader == "ParallaxTotal_1Light") )
	{
		ShaderParallaxTotal(stateset, programObject, iContDrawable);
	}
	else if (vNameShader == "ParallaxShadow")
	{
		ShaderParallaxShadow(stateset, programObject, iContDrawable);
	}
	else if (vNameShader == "Water")
	{
		ShaderWater(stateset, programObject, iContDrawable);
	}
	else if (vNameShader == "movimiento")
	{
		ShaderMovement(stateset);
	}
	else if (vNameShader == "testShader")
	{
		ShaderTest(stateset, iContDrawable);
	}	
	else if (vNameShader == "testFog")
	{
		ShaderTestFog(stateset, iContDrawable);
	}	
		
	m_vGeoms[iContDrawable]->m_pDrawable->setStateSet(stateset);			
}

//----------------------- Shading ---------------------------
void Model::ShaderPerPixel(osg::StateSet* stateset, int iContDrawable)
{	
	stateset->setTextureAttribute(0, m_vGeoms[iContDrawable]->m_tDiffuse);
	stateset->addUniform( new osg::Uniform("myTexture", 0) );	
}

void Model::ShaderTest(osg::StateSet* stateset, int iContDrawable)
{	
	osg::Vec4 vColor;
	switch (iContDrawable)
	{
		case 0:
			vColor = osg::Vec4(1,0,0,1);
		break;
		case 1:
			vColor = osg::Vec4(0,1,0,1);
		break;
		case 2:
			vColor = osg::Vec4(0,0,1,1);
		break;
		case 3:
			vColor = osg::Vec4(1,1,0,1);
		break;
		case 4:
			vColor = osg::Vec4(1,0,1,1);
		break;
		case 5:
			vColor = osg::Vec4(0,1,1,1);
		break;
		default:
			vColor =osg:: Vec4(1,1,1,1);
		break;
	}

	m_vGeoms[iContDrawable]->m_pUniform = new osg::Uniform("vColor", vColor);
	stateset->addUniform(m_vGeoms[iContDrawable]->m_pUniform);
}

void Model::ShaderTestFog(osg::StateSet* stateset, int iContDrawable)
{	
	osg::Vec4 vColor;
	switch (iContDrawable)
	{
	case 0:
		vColor = osg::Vec4(1,0,0,1);
		break;
	case 1:
		vColor = osg::Vec4(0,1,0,1);
		break;
	case 2:
		vColor = osg::Vec4(0,0,1,1);
		break;
	case 3:
		vColor = osg::Vec4(1,1,0,1);
		break;
	case 4:
		vColor = osg::Vec4(1,0,1,1);
		break;
	case 5:
		vColor = osg::Vec4(0,1,1,1);
		break;
	default:
		vColor =osg:: Vec4(1,1,1,1);
		break;
	}
	//stateset->addUniform( new osg::Uniform("vColor", vColor) );	

	osg::ref_ptr<osg::Fog> fog = new osg::Fog();  //The fog object
	//fog->setMode(osg::Fog::EXP);		  	//Fog type 			
	fog->setColor(osg::Vec4(0.5,.5,.5,.5));	//Fog color	
	//fog->setStart(10.);				//Start position of the fog - distance from the camera to the fog
	//fog->setEnd(200.); // maximum distance - where the fog terminates.
	fog->setDensity(0.007f);
	fog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH );
	//osg::ref_ptr< osg::StateSet> fogState (new osg::StateSet);

	stateset->setAttributeAndModes(fog.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_FOG,osg::StateAttribute::ON);
	

	//m_Uniform = new osg::Uniform("vColor", vColor) ;	
	m_vGeoms[iContDrawable]->m_pUniform = new osg::Uniform("vColor", vColor);
	stateset->addUniform(m_vGeoms[iContDrawable]->m_pUniform);
}

void Model::ShaderParallaxTotal(osg::StateSet* stateset, osg::Program* programObject, int iContDrawable)
{
	bool bHasOpacity = m_vGeoms[iContDrawable]->m_tOpacity;

	stateset->setTextureAttribute(0, m_vGeoms[iContDrawable]->m_tDiffuse);
	stateset->setTextureAttribute(1, m_vGeoms[iContDrawable]->m_tNormal);	
	stateset->setTextureAttribute(2, m_vGeoms[iContDrawable]->m_tHeight);
	stateset->setTextureAttribute(3, m_vGeoms[iContDrawable]->m_tSpecular);
	if (bHasOpacity)
		stateset->setTextureAttribute(4, m_vGeoms[iContDrawable]->m_tOpacity);	
	/*else
	{
		//Create the texture image
		//MIRAR LO DEL OPACITY!!! A VER QUÉ PASA CON EL RG Y CON LA TRANSPARENCIA
		osg::ref_ptr<osg::Image> image = new osg::Image();
		image->allocateImage(1,1,1,   // 2D texture is 1 pixel deep
			GL_RGBA,
			GL_UNSIGNED_BYTE);
		image->setInternalTextureFormat(GL_RGBA8);
		// Create the texture object and set the image
		osg::ref_ptr<osg::Texture2D> texture2D = new osg::Texture2D();
		texture2D->setImage(image);

		stateset->setTextureAttribute(4, texture2D);	
	}*/

	stateset->addUniform( new osg::Uniform("colorMap", 0) );
	stateset->addUniform( new osg::Uniform("normalMap", 1) ); 
	stateset->addUniform( new osg::Uniform("heightMap", 2) ); 
	stateset->addUniform( new osg::Uniform("specularMap", 3) ); 
	if (bHasOpacity)
		stateset->addUniform( new osg::Uniform("opacityMap", 4) ); 

	osg::Geometry *tmpGeo = dynamic_cast<osg::Geometry*>(m_vGeoms[iContDrawable]->m_pDrawable);
	tmpGeo->setTexCoordArray(0, tmpGeo->getTexCoordArray(0));

	osg::Material *material = GetMaterial(tmpGeo->getOrCreateStateSet());
	if (!material)
	{
		osg::Material *material = new osg::Material();
		material->setAmbient(osg::Material::FRONT, osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
		material->setDiffuse(osg::Material::FRONT, osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
		material->setSpecular(osg::Material::FRONT, osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
		material->setShininess(osg::Material::FRONT, 4.0f);
		//material->setColorMode(osg::Material::ColorMode::AMBIENT_AND_DIFFUSE);
	}
	else if (material->getShininess(osg::Material::FRONT) < 1.0f)
	{
		material->setShininess(osg::Material::FRONT, 4.0f); //shininess for lighting, if not is black
	}

	stateset->setAttribute(material, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);

	SetTangent(tmpGeo, programObject);

	SetInitialFog(stateset);

	if (bHasOpacity)
	{
		SetTransparency(stateset); //transparency for opacity
	}
	
	//Uniform light
	m_vGeoms[iContDrawable]->m_pUniform = new osg::Uniform("iTypeLight", 1);
	stateset->addUniform(m_vGeoms[iContDrawable]->m_pUniform);
}

void Model::ShaderParallaxShadow(osg::StateSet* stateset, osg::Program* programObject, int iContDrawable)
{	
	stateset->addUniform( new osg::Uniform("colorMap", 0) );	
	stateset->addUniform( new osg::Uniform("normalMap", 1) );	
	stateset->addUniform( new osg::Uniform("heightMap", 2) );

	stateset->setTextureAttribute(0,  m_vGeoms[iContDrawable]->m_tDiffuse ); 
	stateset->setTextureAttribute(1,  m_vGeoms[iContDrawable]->m_tNormal ); 
	stateset->setTextureAttribute(2,  m_vGeoms[iContDrawable]->m_tHeight ); 

	osg::Material *material = new osg::Material();
	material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
	material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.9f, 0.9f, 0.9f, 1.0f));
	material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.2f, 0.2f, 0.2f, 1.0f));
	material->setShininess(osg::Material::FRONT_AND_BACK, 4.0f);
	stateset->setAttribute(material, osg::StateAttribute::ON);	

	osg::Geometry *tmpGeo = dynamic_cast<osg::Geometry*>(m_vGeoms[iContDrawable]->m_pDrawable);
	SetTangent(tmpGeo, programObject);
}

void Model::ShaderWater(osg::StateSet* stateset, osg::Program* programObject, int iContDrawable)
{
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON ); 

	stateset->setTextureAttribute(0, m_vGeoms[iContDrawable]->m_tDiffuse );
	stateset->setTextureAttribute(1, m_vGeoms[iContDrawable]->m_tNormal);
	stateset->setTextureAttribute(2, m_vGeoms[iContDrawable]->m_tDUDV);	
	stateset->setTextureAttribute(3, m_vGeoms[iContDrawable]->m_tSkyMap);

	stateset->addUniform( new osg::Uniform("waterTex", 0) );
	stateset->addUniform( new osg::Uniform("normalTex", 1) );
	stateset->addUniform( new osg::Uniform("DUDVTex", 2) );
	stateset->addUniform( new osg::Uniform("skyMapTex", 3) );	

	osg::Geometry *tmpGeo = dynamic_cast<osg::Geometry*>(m_vGeoms[iContDrawable]->m_pDrawable);
	osgUtil::TangentSpaceGenerator* tsg = new osgUtil::TangentSpaceGenerator();	
	tsg->generate(tmpGeo);	
	osg::Vec4Array* a_tangent = tsg->getTangentArray ();				
	tmpGeo->setVertexAttribArray (6, a_tangent);
	tmpGeo->setVertexAttribBinding (6, osg::Geometry::BIND_PER_VERTEX);
	programObject->addBindAttribLocation ("rm_Tangent", 6);
	osg::Vec4Array* a_binormal = tsg->getBinormalArray();
	tmpGeo->setVertexAttribArray(7, a_binormal);
	tmpGeo->setVertexAttribBinding(7, osg::Geometry::BIND_PER_VERTEX);		
	programObject->addBindAttribLocation ("rm_Binormal", 7);	

	SetInitialFog(stateset);
}

void Model::ShaderMovement(osg::StateSet* stateset)
{	
	osg::ref_ptr<osg::Uniform> moveUniform = new osg::Uniform( "TIME_FROM_INIT",  1);
	moveUniform->setUpdateCallback( new MovementCallback );
	stateset->addUniform( moveUniform.get() );

	osg::ref_ptr<osg::Texture2D> diffuseTexture = new osg::Texture2D;
	osg::ref_ptr<osg::Texture2D> normalTexture = new osg::Texture2D;

	//Hack setting texture!
	diffuseTexture->setImage(osgDB::readImageFile("images/interior4.tga"));	
	normalTexture->setImage(osgDB::readImageFile("images/0000.jpg"));
		
	stateset->setTextureAttribute(0, diffuseTexture);
	stateset->setTextureAttribute(1, normalTexture);

	stateset->addUniform( new osg::Uniform("tex", 0) );
	stateset->addUniform( new osg::Uniform("tex2", 1) );

	//stateset->setDataVariance(osg::Object::DYNAMIC); // ???
}

//-----------------------Internal methods---------------------------
osg::Texture2D* Model::CreateTexture( const std::string& filename )
{
	osg::ref_ptr<osg::Texture2D> texture = new osg::Texture2D;
	texture->setImage( osgDB::readImageFile(filename) );
	texture->setWrap( osg::Texture2D::WRAP_S, osg::Texture2D::REPEAT );
	texture->setWrap( osg::Texture2D::WRAP_T, osg::Texture2D::REPEAT );
	texture->setFilter( osg::Texture::MIN_FILTER, osg::Texture::LINEAR_MIPMAP_LINEAR );
	texture->setFilter( osg::Texture::MAG_FILTER, osg::Texture::LINEAR );
	return texture.release();
}

void Model::SetInitialFog(osg::StateSet *stateset)
{
	osg::ref_ptr<osg::Fog> fog = new osg::Fog();  //The fog object
	fog->setColor(osg::Vec4(0.5,.5,.5,.5));	//Fog color	
	fog->setFogCoordinateSource(osg::Fog::FRAGMENT_DEPTH );
	//fog->setDensity(0.007f);
	fog->setDensity(0.0f);

	//fog->setMode(osg::Fog::EXP);		  	//Fog type 			
	//fog->setStart(10.);									//Start position of the fog - distance from the camera to the fog //Fog linear
	//fog->setEnd(200.);									// maximum distance - where the fog terminates.

	stateset->setAttributeAndModes(fog.get(),osg::StateAttribute::ON);
	stateset->setMode(GL_FOG,osg::StateAttribute::ON);
}

void Model::SetTransparency(osg::StateSet *stateset)
{
	stateset->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
	stateset->setMode( GL_BLEND, osg::StateAttribute::ON ); 
}

void Model::SetTangent(osg::Geometry *tmpGeo, osg::Program* programObject, int iNormalMapTexUnit)
{
	osgUtil::TangentSpaceGenerator* tsg = new osgUtil::TangentSpaceGenerator();
	tsg->generate(tmpGeo, iNormalMapTexUnit);
	osg::Vec4Array* a_tangent = tsg->getTangentArray();

	tmpGeo->setVertexAttribArray (6, a_tangent);
	tmpGeo->setVertexAttribBinding (6, osg::Geometry::BIND_PER_VERTEX);
	programObject->addBindAttribLocation ("Tangent", 6);
}

osg::Material* Model::GetMaterial(osg::StateSet* stateset)
{
	osg::StateAttribute::Type tp = osg::StateAttribute::Type::MATERIAL;
	const osg::StateSet::RefAttributePair* rap =	stateset->getAttributePair(tp);		
	if (rap)
		return ( dynamic_cast<osg::Material*>(rap->first.get()) );
	else
		return NULL;
}

//-----------------------External setters-------------------------
void Model::SetFogDensity(float fInc)
{
	osg::StateAttribute::Type tp = osg::StateAttribute::Type::FOG;
	//Go through every drawables
	for (int i = 0; i < m_vGeoms.size(); ++i)
	{
		osg::StateSet* ss = m_vGeoms[i]->m_pDrawable->getOrCreateStateSet();
		if (ss)
		{
			const osg::StateSet::RefAttributePair* rap =	ss->getAttributePair(tp);		
			if (rap)
			{
					osg::Fog* fg = dynamic_cast<osg::Fog*>(rap->first.get()); 				 
					float fDensity = fg->getDensity() + fInc;
					if (fDensity > 0.0f)
						fg->setDensity(fg->getDensity() + fInc);
					else
						fg->setDensity(0.0f);
				std::cout << "Fog density = " << fg->getDensity() << std::endl;
			}
			else
			{
				std::cout << "No fog. Apply initial fog:" << std::endl;
				SetInitialFog(ss);
			}
		}
	}
}

//-----------------------Set Uniforms-------------------------------
void Model::SetUniformVec4(osg::Vec4 color)
{ 
	for (int i = 0; i < m_vGeoms.size(); ++i)
		m_vGeoms[i]->m_pUniform->set(color); 
}

void Model::SetUniformInt(int iValue)
{
	for (int i = 0; i < m_vGeoms.size(); ++i)
	{
		if (m_vGeoms[i]->m_pUniform)
			m_vGeoms[i]->m_pUniform->set(iValue); 
	}
}

//----------------------------UPDATE--------------------------------
void Model::Update()
{
	//TO-DO
}