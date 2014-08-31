#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/AnimationPath>
#include <osg/MatrixTransform>
#include <osgGA/TrackballManipulator>
#include <osgDB/ReadFile>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include "Model.h"

#include "XML_LoadScene.h"

//-----------------	GLOBAL ------------------
osgViewer::Viewer viewer;
osg::Switch* swGeneral = new osg::Switch;

osg::PositionAttitudeTransform* pat_light_white;
osg::PositionAttitudeTransform* pat_light_red;
osg::PositionAttitudeTransform* pat_light_star;

osg::AnimationPathCallback* apcb;
static bool _bMovementLights = false;
//---------------------------------------------

void ChangeNode(int iNode)
{
	//Activanting current node
	if (iNode < swGeneral->getNumChildren())
	{
		for (int i = 0; i < swGeneral->getNumChildren(); ++i)
		{
			if (i == iNode)
			{
				swGeneral->setValue(i, true);
			}
			else
			{
				swGeneral->setValue(i, false);
			}
		}		
	}	

	//Activating lights
	switch (iNode)
	{
		case 0: //NoShader
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x0);
			pat_light_star->setNodeMask(0x0);
			std::cout << "**No shader" << std::endl;
		break;
		case 1: //PhongLight
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x0);
			pat_light_star->setNodeMask(0x0);
			std::cout << "**Phong Point Light" << std::endl;
		break;
		case 2: //Parallax1Light (no specular)
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x0);
			pat_light_star->setNodeMask(0x0);
			std::cout << "**Parallax Total 1 Light (no specular)" << std::endl;
		break;
		case 3: //Parallax1Light (with specular)
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x0);
			pat_light_star->setNodeMask(0x0);
			std::cout << "**Parallax Total 1 Light (with specular)" << std::endl;
			break;
		case 4: //Parallax2Lights (No specular)
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x1);
			pat_light_star->setNodeMask(0x1);
			std::cout << "**Parallax Total 2 Lights (no specular)" << std::endl;
		break;
		case 5: //Parallax2Lights (with specular)
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x1);
			pat_light_star->setNodeMask(0x1);
			std::cout << "**Parallax Total 2 Lights (with specular)" << std::endl;
		break;		
		case 6: //Parallax2Lights (with specular and opacity)
			pat_light_white->setNodeMask(0x1);
			pat_light_red->setNodeMask(0x1);
			pat_light_star->setNodeMask(0x1);
			std::cout << "**Parallax Total 2 Lights (with specular and opacity)" << std::endl;
		break;		
	};
}

class KeyboardEventHandler : public osgGA::GUIEventHandler
{
public:
	virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter&)
	{
		switch(ea.getEventType())
		{
			case(osgGA::GUIEventAdapter::KEYDOWN):
			{
				switch(ea.getKey())
				{
				case osgGA::GUIEventAdapter::KEY_1:	//No shader
					ChangeNode(0);
				break;
				case osgGA::GUIEventAdapter::KEY_2:	//Phong point light
					ChangeNode(1);
				break;
				case osgGA::GUIEventAdapter::KEY_3:	//Parallax 1 light (no specular)
					ChangeNode(2);
				break;
				case osgGA::GUIEventAdapter::KEY_4: //Parallax 1 light (with specular)
					ChangeNode(3);
				break;
				case osgGA::GUIEventAdapter::KEY_5: //Parallax 2 lights (no specular)
					ChangeNode(4);
				break;
				case osgGA::GUIEventAdapter::KEY_6: //Parallax 2 lights (with specular)
					ChangeNode(5);
				break;
				case osgGA::GUIEventAdapter::KEY_7: //Parallax 2 lights (with specular and opacity)
					ChangeNode(6);
					break;
				case osgGA::GUIEventAdapter::KEY_Return:{					
					_bMovementLights = !_bMovementLights;
					apcb->setPause(_bMovementLights);
				}break;
				default:
					return false;
				} 
			}
			default:
			return false;
		}
	}
	virtual void accept(osgGA::GUIEventHandlerVisitor& v)   { v.visit(*this); };
};

osg::Geode* CreateLine(const osg::Vec3& vSource, const osg::Vec3& vTarget, const osg::Vec4& color)
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

	// This time we simply use primitive, and hardwire the number of coords to use since we know up front,
	linesGeom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES,0,2));

	osg::ref_ptr<osg::Geode> geode = new osg::Geode;
	geode->addDrawable(linesGeom);

	return geode.release();
}

osg::Group* CreateDirectionalLight( unsigned int lightNum, const osg::Vec3& position, const osg::Vec3& direction, const osg::Vec4& color)
{
	osg::Group* group = new osg::Group;

	pat_light_star = new osg::PositionAttitudeTransform();
	pat_light_star->setPosition(position);

	// create light source.
	osg::LightSource* lightsource = new osg::LightSource;
	osg::Light* m_MoonLight = lightsource->getLight();
	lightsource->setLocalStateSetModes(osg::StateAttribute::ON);

	m_MoonLight->setLightNum(lightNum);
	m_MoonLight->setPosition(osg::Vec4(position, 0.0f)); //4th component = 1 --> Spot light

	m_MoonLight->setDirection(direction);
	m_MoonLight->setDiffuse(osg::Vec4(color));		

	pat_light_star->addChild(lightsource);

	osg::ref_ptr<osg::Geode> helper = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> shapemarca1 = new osg::ShapeDrawable;
	shapemarca1.get()->setShape( new osg::Box(position, 2.5f) );
	shapemarca1->setColor( color );
	helper->addDrawable(shapemarca1);
	pat_light_star->addChild(helper);

	//Arrow debug with the light direction
	/*osg::Vec3 target = direction * 4.0f;	
	target += position;
	transf->addChild( CreateLine(position, target, osg::Vec4(1,0,0,1)) );*/

	group->addChild(pat_light_star);    
	return group;    
}

osg::PositionAttitudeTransform* Load_2_Lights()
{
	//----------------LIGHT 1------------------
	osg::Vec4 color = osg::Vec4(1,1,1,1);
	osg::Vec3 direction_1 = osg::Vec3(0,1,0);

	//Create Light
	osg::LightSource* lightsource_1 = new osg::LightSource;
	osg::Light* light_1 = lightsource_1->getLight();
	lightsource_1->setLocalStateSetModes(osg::StateAttribute::ON);
	light_1->setLightNum(4);
	light_1->setPosition(osg::Vec4(0,0,0,1)); //4th component = 1 --> Spot light
	light_1->setDirection(osg::Vec3(0,1,0));	
	light_1->setDiffuse(osg::Vec4(color));		
	light_1->setAmbient(osg::Vec4(0,0,0,1));  
	light_1->setSpecular(osg::Vec4(1,1,1,1));
	//light_1->setSpotCutoff( 25.0f );
	//light_1->setQuadraticAttenuation(0.002f);

	//Light helper
	osg::ref_ptr<osg::Geode> helper_1 = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> shapemarca1 = new osg::ShapeDrawable;
	shapemarca1.get()->setShape( new osg::Sphere(osg::Vec3(), 2.5f) );
	shapemarca1->setColor( color );
	helper_1->addDrawable(shapemarca1);
	helper_1->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	//Arrow debug with the light direction
	//osg::Vec3 arrow_1 = direction_1 * 4.0f;	

	//----------------LIGHT 2------------------
	osg::Vec4 color_2 = osg::Vec4(1,0,0,1);

	//Create Light
	osg::LightSource* lightsource_2 = new osg::LightSource;
	osg::Light* light_2 = lightsource_2->getLight();
	lightsource_2->setLocalStateSetModes(osg::StateAttribute::ON);
	light_2->setLightNum(5);
	light_2->setPosition(osg::Vec4(0,0,0,1)); //4th component = 1 --> Spot light
	light_2->setDirection(osg::Vec3(0,1,0));	
	light_2->setDiffuse(color_2);		
	light_2->setAmbient(osg::Vec4(0,0,0,1));  
	light_2->setSpecular(osg::Vec4(1,1,1,1));
	//light_2->setSpotCutoff( 25.0f );
	//light_2->setQuadraticAttenuation(0.002f);


	//Light helper
	osg::ref_ptr<osg::Geode> helper_2 = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> shapemarca_2 = new osg::ShapeDrawable;
	shapemarca_2.get()->setShape( new osg::Sphere(osg::Vec3(), 2.5f) );
	shapemarca_2->setColor( color_2 );
	helper_2->addDrawable(shapemarca_2);
	helper_2->getOrCreateStateSet()->setMode( GL_LIGHTING, osg::StateAttribute::OFF );

	////----------------Position attitude transform//----------------
	osg::Vec3 position_1 = osg::Vec3(10,0,0);
	pat_light_white = new osg::PositionAttitudeTransform;
	pat_light_white->setPosition(position_1);
	pat_light_white->addChild(helper_1);
	//pat_light_1->addChild(CreateLine(osg::Vec3(), arrow_1, osg::Vec4(1,0,0,1)));
	pat_light_white->addChild(lightsource_1);

	osg::Vec3 position_2 = osg::Vec3(-10,0,0);
	pat_light_red = new osg::PositionAttitudeTransform;
	pat_light_red->setPosition(position_2);
	pat_light_red->addChild(helper_2);
	pat_light_red->addChild(lightsource_2);

	osg::Vec3 positionGeneral = osg::Vec3(10,0,25);
	osg::PositionAttitudeTransform* patGeneral = new osg::PositionAttitudeTransform;
	patGeneral->setPosition(positionGeneral);
	patGeneral->addChild(pat_light_white);
	patGeneral->addChild(pat_light_red);	

	return patGeneral;
}

osg::AnimationPath* createAnimationPath( float radius, float time )
{
	osg::ref_ptr<osg::AnimationPath> path = new osg::AnimationPath;
	path->setLoopMode( osg::AnimationPath::LOOP );
	unsigned int numSamples = 32;
	float delta_yaw = 2.0f * osg::PI/((float)numSamples - 1.0f);
	float delta_time = time / (float)numSamples;
	for ( unsigned int i=0; i<numSamples; ++i )
	{
		float yaw = delta_yaw * (float)i;
		osg::Vec3 pos( sinf(yaw)*radius, cosf(yaw)*radius, 0.0f );
		osg::Quat rot( -yaw, osg::Z_AXIS );
		path->insert( delta_time * (float)i,	osg::AnimationPath::ControlPoint(pos, rot) );
	}
	return path.release(); 
}

//Spotlights move with this node
osg::MatrixTransform* LoadMovementNode()
{
	osg::ref_ptr<osg::MatrixTransform> node = new osg::MatrixTransform;
	node->addChild( Load_2_Lights() );
	apcb = new osg::AnimationPathCallback;
	apcb->setAnimationPath( createAnimationPath(50.0f, 10.0f) );
	_bMovementLights = false; //Method used: ->setPaused(bool);
	node->setUpdateCallback( apcb );

	return node.release();
}

osg::Node* LoadShaderModel(int iType, bool bUseShader = true)
{
	std::string vFile = "../Assets/";
	switch (iType)
	{
		case 0:{ //PhongPoint
			vFile += "TeapotDiffuse.osgt";
		}break;
		case 1: //Parallax1Light (no specular)
		case 3: //Parallax1Light (no specular)
		{
			vFile += "TeapotParallax.osgt";
		}break;		
		case 2: //Parallax2LightsTotal (with specular)
		case 4: //Parallax2LightsTotal (with specular)
		{
			vFile += "TeapotParallaxTotal.osgt";
		}break;
		case 5: { //Opacity
			vFile += "TeapotParallaxTotalOpacity.osgt";
		}
	}

	if (bUseShader)
	{
		Model* md = new Model(false);
		if ( (iType == 1) || (iType == 2) )	{ //1 light
			md->SetNumLights(1);
		}
		md->LoadModel(vFile);
		return md->GetNode();
	}
	else
	{
		return osgDB::readNodeFile(vFile);
	}
}

osg::Node* LoadSimpleModel()
{
	osg::ref_ptr<osg::Node> simpleModel = osgDB::readNodeFile("../Assets/TeapotDiffuse.osgt");
	simpleModel->getOrCreateStateSet()->setMode(GL_LIGHT0, osg::StateAttribute::OFF);
	//simpleModel->getOrCreateStateSet()->setMode(GL_LIGHT3, osg::StateAttribute::ON);
	simpleModel->getOrCreateStateSet()->setMode(GL_LIGHT4, osg::StateAttribute::ON);
	//simpleModel->getOrCreateStateSet()->setMode(GL_LIGHT5, osg::StateAttribute::ON);	
	return simpleModel.release();
}

void SetupViewer(osg::Group* root)
{
	viewer.setSceneData( root );
	viewer.setUpViewInWindow(300, 100, 800, 600);

	//viewer.getCamera()->getView()->setLightingMode(osg::View::LightingMode::NO_LIGHT);	
	viewer.getCamera()->getView()->getLight()->setDirection(osg::Vec3(0,1,0));
	viewer.getCamera()->setClearColor(osg::Vec4(0.1,0.1,0.1,1.0));

	viewer.addEventHandler( new osgViewer::StatsHandler);
	viewer.addEventHandler( new KeyboardEventHandler() );
}

void MainMenu()
{
	std::cout << "---------------------------------geOSG Demo---------------------------------" << std::endl;
	std::cout << "**** INSTRUCTIONS ****" << std::endl;
	std::cout << std::endl;
	std::cout << "Press key:" << std::endl;
	std::cout << "- 1: No shader " << std::endl;
	std::cout << "- 2: Phong. " << std::endl;
	std::cout << "- 3: Parallax 1 light (no specular) " << std::endl;
	std::cout << "- 4: Parallax 1 light (with specular) " << std::endl;
	std::cout << "- 5: Parallax 2 Lights (no specular)" << std::endl;
	std::cout << "- 6: Parallax 2 Lights (with specular) " << std::endl;
	std::cout << "- 7: Parallax Opacity 2 Lights (with specular) " << std::endl;
	std::cout << "- ENTER: Toggle on/off light movement" << std::endl;

	std::cout << std::endl;
}

int main( int argc, char** argv )
{	
	//- Main Group
	osg::ref_ptr<osg::Group> root = new osg::Group;

	//-------- Load system lights
	root->addChild(LoadMovementNode());
	root->addChild(CreateDirectionalLight(3, osg::Vec3(1,1,100), osg::Vec3(0, 0, -1), osg::Vec4(0.6, 0.8, 1, 0.5))); //azul

	//-------- Load Models
	swGeneral->addChild(LoadSimpleModel());		//No shader
	swGeneral->addChild(LoadShaderModel(0));	//PhongPointLight
	swGeneral->addChild(LoadShaderModel(1));	//Parallax 1 Light (no specular)
	swGeneral->addChild(LoadShaderModel(2));	//Parallax 1 Light (with specular)
	swGeneral->addChild(LoadShaderModel(3));	//Parallax 2 Lights (no specular)
	swGeneral->addChild(LoadShaderModel(4));	//Parallax 2 Lights (with specular)	
	swGeneral->addChild(LoadShaderModel(5));	//Parallax Opacity
	
	//Add switch to general root
	root->addChild(swGeneral);

	//XML_LoadScene::GetInstance()->ReadXMLScene("../Assets/scene.xml");

	//--->	Viewer
	SetupViewer(root.get());

	bool bFirstTime = true;

	MainMenu();

	//--->	Execute!
	//viewer.run();
	viewer.setCameraManipulator(new osgGA::TrackballManipulator);
	viewer.realize();
	while(!viewer.done())
	{
		viewer.frame();  	

		if (bFirstTime)
		{
			bFirstTime = false;
			ChangeNode(0);
		}
	}
	return 0;
}
