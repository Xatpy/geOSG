#include "XML_LoadScene.h"

#include <osg/Light>
#include <osgDB/ReadFile>
#include <osg/ShapeDrawable>

#include "Model.h"

#include "Util.h"

XML_LoadScene* XML_LoadScene::GetInstance()
{
	static XML_LoadScene vInstance;
	return &vInstance; 
}

//Función para pintar por consola el xml
void dump_to_stdout( TiXmlNode* pParent, unsigned int indent = 0 )
{
	if ( !pParent ) return;

	TiXmlNode* pChild;
	TiXmlText* pText;
	int t = pParent->Type();
	//printf( "%s", getIndent(indent));
	int num;

	switch ( t )
	{
	case TiXmlNode::NodeType::TINYXML_DOCUMENT:
		printf( "Document" );
		break;

	case TiXmlNode::NodeType::TINYXML_ELEMENT:
		printf( "Element [%s]", pParent->Value() );
		/*num=dump_attribs_to_stdout(pParent->ToElement(), indent+1);
		switch(num)
		{
		case 0:  printf( " (No attributes)"); break;
		case 1:  printf( "%s1 attribute", getIndentAlt(indent)); break;
		default: printf( "%s%d attributes", getIndentAlt(indent), num); break;
		}
		break;*/

	case TiXmlNode::NodeType::TINYXML_COMMENT:
		printf( "Comment: [%s]", pParent->Value());
		break;

	case TiXmlNode::NodeType::TINYXML_UNKNOWN:
		printf( "Unknown" );
		break;

	case TiXmlNode::NodeType::TINYXML_TEXT:
		pText = pParent->ToText();
		printf( "Text: [%s]", pText->Value() );
		break;

	case TiXmlNode::NodeType::TINYXML_DECLARATION:
		printf( "Declaration" );
		break;
	default:
		break;
	}
	printf( "\n" );
	for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
	{
		dump_to_stdout( pChild, indent+1 );
	}
}

void XML_LoadScene::ReadPAT(TiXmlNode* pParent, osg::Group*& gr)
{
	gr = (new osg::PositionAttitudeTransform);

	if (pParent->ToElement())
	{
		if (pParent->ToElement()->Attribute("name"))
		{
			gr->setName(pParent->ToElement()->Attribute("name"));
		}

		osg::Vec3 dir;
		std::string strAux = "";//clear aux
		if ( pParent->ToElement()->Attribute("position") ) 
		{
			strAux = pParent->ToElement()->Attribute("position");
		}
		if (!strAux.empty())
		{
			dir = Util::ParseStringToVec3(strAux.c_str());			
		}

		gr->asTransform()->asPositionAttitudeTransform()->setPosition(dir);
	}
}

osg::Node* XML_LoadScene::ReadModel(TiXmlNode* pParent)
{
	std::string nodoNameTr = pParent->ToElement()->Attribute("file");		
	bool bDebug = false;
	if ( pParent->ToElement()->Attribute("debug") ) 
	{
		std::string strDebug = pParent->ToElement()->Attribute("debug");
		bDebug = (strDebug == "true" ? true : false);					
	}
	Model* md = new Model(bDebug);
	md->LoadModel(nodoNameTr);
	md->GetNode()->setName("Modelo: " + nodoNameTr);			

	//md->GetNode()->getOrCreateStateSet()->setDataVariance(osg::Object::DYNAMIC); 
	md->GetNode()->getOrCreateStateSet()->setMode( GL_LIGHT0, osg::StateAttribute::OFF );
	md->GetNode()->getOrCreateStateSet()->setMode( GL_LIGHT1, osg::StateAttribute::OFF );
	
	return md->GetNode();
}

osg::LightSource* XML_LoadScene::ReadLight( TiXmlNode* pParent )
{
	osg::LightSource* ls = new osg::LightSource;
	ls->setName("CustomLight");
	osg::Light* light = ls->getLight();

	osg::Vec4 v4pos;

	std::string strAux = "";
	//Setear posicin
	if ( pParent->ToElement()->Attribute("position") ) 
	{
		strAux = pParent->ToElement()->Attribute("position");
	}
	if (!strAux.empty())
	{
		v4pos = Util::ParseStringToVec4(strAux);
		light->setPosition(v4pos);
	}		

	//Setear num
	strAux = "";//clear aux
	if ( pParent->ToElement()->Attribute("num") ) 
	{
		strAux = pParent->ToElement()->Attribute("num");
	}
	if (!strAux.empty())
	{
		int num = atoi(strAux.c_str());
		light->setLightNum(num);
	}

	//---Set direction
	strAux = "";//clear aux
	osg::Vec3 dir;
	if ( pParent->ToElement()->Attribute("direction") ) 
	{
		strAux = pParent->ToElement()->Attribute("direction");
	}
	if (!strAux.empty())
	{
		dir = Util::ParseStringToVec3(strAux.c_str());
		light->setDirection(dir);
	}

	//---SetDiffuse
	strAux = "";//clear aux
	osg::Vec4 diff;
	if ( pParent->ToElement()->Attribute("diffuse") ) 
	{
		strAux = pParent->ToElement()->Attribute("diffuse");
	}
	if (!strAux.empty())
	{
		diff = Util::ParseStringToVec4(strAux.c_str());
		light->setDiffuse(diff);
	}

	//---SetAmbient
	strAux = "";//clear aux
	if ( pParent->ToElement()->Attribute("ambient") ) 
	{
		strAux = pParent->ToElement()->Attribute("ambient");
	}
	if (!strAux.empty())
	{
		osg::Vec4 amb = Util::ParseStringToVec4(strAux.c_str());
		light->setAmbient(amb);
	}			

	//---SetSpecular
	strAux = "";//clear aux
	if ( pParent->ToElement()->Attribute("specular") ) 
	{
		strAux = pParent->ToElement()->Attribute("specular");
	}
	if (!strAux.empty())
	{
		osg::Vec4 spec = Util::ParseStringToVec4(strAux.c_str());
		light->setSpecular(spec);
	}						

	//Compruebo si hay que pintar la pelota de debug para ver dónde está la bola
	bool bDebugBall = false;
	if ( pParent->ToElement()->Attribute("debug") ) 
	{
		strAux = pParent->ToElement()->Attribute("debug");
	}
	if ( (strAux == "true") || (strAux == "1") )
	{
		bDebugBall = true;		
	}	

	if (bDebugBall)
	{
		osg::ref_ptr<osg::Geode> helper = new osg::Geode;
		osg::ref_ptr<osg::ShapeDrawable> shapeDrw = new osg::ShapeDrawable;
		shapeDrw.get()->setShape( new osg::Sphere(osg::Vec3(v4pos.x(), v4pos.y(), v4pos.z()), 2.5f) );
		shapeDrw->setColor( diff );
		helper->addDrawable(shapeDrw);
		ls->addChild(helper);

		//Arrow debug with the light direction
		osg::Vec3 target = dir * 4.0f;	
		target += osg::Vec3(v4pos.x(), v4pos.y(), v4pos.z());
		ls->addChild( CreateLine(osg::Vec3(v4pos.x(), v4pos.y(), v4pos.z()), target, osg::Vec4(1,0,0,1)) );
	}
	return ls;
}

void XML_LoadScene::ExploreNodeTree( TiXmlNode* pParent, stNode* tree, osg::Group*& osgTree, unsigned int indent)
{
	if ( !pParent ) 
		return;

	std::string type = pParent->Value();
	if ( pParent->Type() == TiXmlNode::NodeType::TINYXML_ELEMENT)
	{
		if (_bDebug)
			std::cout << "Tipo = " << type << " y el nivel es " << indent << std::endl;

		stNode* nd = new stNode;
		nd->type = type;	
		tree->children.push_back(nd);

		osg::Group* gr = 0;
		if (type == "Group")
		{
			gr = new osg::Group;
			gr->setName("Group");
			osgTree->addChild(gr);
		}
		else if (type == "PAT")
		{
			ReadPAT(pParent, gr);
			osgTree->addChild(gr);
		}
		else if (type == "Node")
		{
			if (pParent->ToElement()->Attribute("file"))
			{
				std::string nodoName = pParent->ToElement()->Attribute("file");
				osg::Node* nd = osgDB::readNodeFile(nodoName);
				nd->setName("Node");
				
				nd->getOrCreateStateSet()->setMode( GL_LIGHT0, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT1, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT2, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT3, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT4, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT5, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT6, osg::StateAttribute::OFF );
				nd->getOrCreateStateSet()->setMode( GL_LIGHT7, osg::StateAttribute::OFF );				

				osgTree->addChild(nd);
			}			
			else
			{
				std::cout << "Error loading node. No file attribute" << std::endl;
			}
		}
		else if (type == "Model")
		{	
			if (pParent->ToElement()->Attribute("file"))
			{
				osgTree->addChild( ReadModel(pParent) );
			}
			else
			{
				std::cout << "No name for model." << std::endl;
			}
		}
		else if (type == "Light")
		{
			osgTree->addChild(ReadLight(pParent));
		}
		else if (type == "Scene")
		{
			osgTree = new osg::Group;
			gr = new osg::Group;
			osgTree->addChild(gr);
		}

		TiXmlNode* pChild;
		for ( pChild = pParent->FirstChild(); pChild != 0; pChild = pChild->NextSibling()) 
		{
			ExploreNodeTree( pChild, nd, gr, indent+1 );
		}
	}
}

bool XML_LoadScene::ReadXMLScene(const char* sFile)
{
	std::cout << "-Trying to read " << sFile << "...: " << sFile << std::endl;

	TiXmlElement *pGeneral;
	TiXmlDocument doc(sFile);

	if (doc.LoadFile())
	{
		//Get global element
		pGeneral = doc.FirstChildElement("Scene");

		if (pGeneral)
		{
			//check if debug messages load xml by console
			if (pGeneral->Attribute("debug"))
			{
				std::string deb = pGeneral->Attribute("debug");
				if (deb == "true")
				{
					_bDebug = true;
				}
			}
			
			_tree = new stNode;
			_osgTree = 0;
			ExploreNodeTree(pGeneral, _tree, _osgTree);

			doc.Clear();
			isRead=true;
			
			std::cout << "Complete XMLSetupReader!" << std::endl;

			return true;
		}
		else
		{
			std::cout << "File error. No ModelSimplifier node found." << std::endl;
			return false;
		}
	}
	std::cout << "Error reading XML" << doc.ErrorDesc() << std::endl;
	return false;
}

osg::Geode* XML_LoadScene::CreateLine(const osg::Vec3& vSource, const osg::Vec3& vTarget, const osg::Vec4& color)
{
	osg::Vec3Array* vertices = new osg::Vec3Array(2);
	(*vertices)[0].set(vSource.x(), vSource.y(), vSource.z());
	(*vertices)[1].set(vTarget.x(), vTarget.y(), vTarget.z());

	osg::Geometry* linesGeom = new osg::Geometry();
	linesGeom->setVertexArray(vertices);

	// set the colors as before, plus using the above
	osg::Vec4Array* colors = new osg::Vec4Array;
	colors->push_back(color);
	linesGeom->setColorArray(colors, osg::Array::BIND_OVERALL);

	// set the normal in the same way color.
	osg::Vec3Array* normals = new osg::Vec3Array;
	normals->push_back(osg::Vec3(0.0f,-1.0f,0.0f));
	linesGeom->setNormalArray(normals, osg::Array::BIND_OVERALL);

	// This time we simply use primitive, and hardwire the number of coords to use
	// since we know up front,
	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(linesGeom);

	return geode.release();
}