#pragma once

#ifndef XML_MODEL_SIMPLIFIER_H
#define XML_MODEL_SIMPLIFIER_H

#include <iostream>
#include <vector>

#include "tinyxml.h"

#include "Model.h"

#include <osg/PositionAttitudeTransform>


/************************************************************************/
/* -----------------------------XML_LoadScene---------------------------------------------    	
	Load node hierarchy from XML file (root is an osg::Group). This file has the next features:	
	- XML root is a node called 'Scene'. If this has an attribute debug="true", it will print (console) when it is reading nodes.	
	- Node types allowed:
		# Group. Without attributes.
		# Node: Graphics nodes (example: ive). 
			· file = file name.
		# Model: Models with shaders. Attributes:
			· file="nameFile"; Model file (example: *.osgb, *.osgt)
			· debug="true"; Printing model load
		# Light: Scene lights
			· num: Light number (OpenGL)
			· position:"x,y,z,w". Lights coordinates. If the fourth component is 1, it is a spot light. If it is 0, it a directional light.
			· direction:"x,y,z". Vec3. Light direction.
			· diffuse:"x,y,z,w". Diffuse color (affects materials). Fourth compontent: alpha.			
			· ambient:"x,y,z,w". Ambient color (affects materials). Fourth compontent: alpha.	
			· specular:"x,y,z,w". Specular color (affects materials). Fourth compontent: alpha.	
		# PAT: Posittion, Attitude, Transform. Position node.
			·position="x,y,z". Vec3 position node.	
	- Vec3(x,y,z) y Vec4(x,y,z,w) read from string (with "") in the xml. Example: position="50,0,10"

	Example:
	<Scene debug="true" >
		<Group>
			<PAT position="50,0,0">
				<Model file="car.osgt" />
			</PAT>
			<PAT position="0,60,0.2">
				<Model debug="true" file="box.osgt" />
			</PAT>
			<PAT position="0,60,0">
				<Model debug="true" file="terreno_sin_agua.osgt" />
			</PAT>
			<PAT position="0,0,50">
				<Model debug="true" file="teapot_diffuse.osgt" />
				<Node file="cabeza.osg"/>
			</PAT>
		</Group>
		<Light num="1" position=" 50, 50, 100, 1 " direction="3,4,5" diffuse="1,1,1,1" ambient="0.1,0.1,0.1,1" specular="0.1,0.1,0.1,1" /> 	
	</Scene>				

/************************************************************************/

//Struct for checking tree creation
struct stNode
{	
	std::string				type;
	std::vector<stNode*>	children;
};

class XML_LoadScene
{
public:
	static XML_LoadScene*						GetInstance();

public:
	bool										ReadXMLScene									(const char* nameFile);

	bool										GetDebug										()										const { return _bDebug; }
		void										SetDebug												(bool val)									{ _bDebug = val; }

	osg::Group*									GetOSGTree										()													{ return _osgTree; }

private:
	void										ExploreNodeTree									( TiXmlNode* pParent, stNode* tree, osg::Group*& osgTree, unsigned int indent = 0);

	void										ReadPAT											(	TiXmlNode* pParent, osg::Group*& gr);	
	osg::Node*									ReadModel										(	TiXmlNode* pParent);
	osg::LightSource*							ReadLight										( TiXmlNode* pParent);

	osg::Geode*									CreateLine										(const osg::Vec3& vSource, const osg::Vec3& vTarget, const osg::Vec4& color);

	bool										isXMLRead										();

private://methods
												XML_LoadScene									(void)					: _bDebug(false) 				{	};
												~XML_LoadScene									(void)																								{	};

private: //data	
	bool										isRead;
	bool										_bDebug;
	stNode*										_tree;			
	osg::Group*									_osgTree;
};

#endif