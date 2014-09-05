geOSG
=====

DEMO: bin/geOSG_Demo.rar

# geOSG

## Description: 
geOSG is a 'toolkit' for Open Scene Graph that allows you to load models with shaders easily. 
You can use the next shaders:
- Phong Lighting
- Parallax Mapping (advance normal map technique)
- Parallax Mapping with Specular Map
- Parallax Mapping with Specular and Opacity Map

## Instructions
Execute: bin\Debug\geOSG.exe
Press key:
- 1: No shader
- 2: Phong.
- 3: Parallax 1 light (no specular)
- 4: Parallax 1 light (with specular)
- 5: Parallax 2 Lights (no specular)
- 6: Parallax 2 Lights (with specular)
- 7: Parallax Opacity 2 Lights (with specular)
- ENTER: Toggle on/off light movement

## How does it works: 
The program detects the maps of the model and apply its shader automatically following the next rules:

If the model has only diffuse, the shader is: Phong
If the model has diffuse AND normal AND height AND OPTIONALLY specular, the shader is: ParallaxTotal
If the model has diffuse AND normal AND height AND OPTIONALLY specular AND opacity, the shader is: ParallaxTotalOpacity

Summary table:
                                    Phong          ParallaxTotal             ParallaxTotalOpacity            
DIFFUSE                 	 X                       X                                X
NORMAL                         -                       X                                X
HEIGHT                         -                       X                                X
SPECULAR                       -                       X(*)                             X
OPACITY                        -                       -                                X

You have to set up your model's textures with some specific names:
- Diffuse texture: The name of the texture has to contains the literal '_DIFFUSE'. Example: teapot_DIFFUSE.jpg 
- Normal texture:The name of the texture has to contains the literal '_NORMAL'. Example: teapot_NORMAL.jpg 
- Height map texture: The name of the texture has to contains the literal '_HEIGHT'. Example: teapot_HEIGHT.jpg 
- Specular texture: The name of the texture has to contains the literal '_SPECULAR'. Example: teapot_SPECULAR.jpg 
- Opacity texture: The name of the texture has to contains the literal '_OPACITY'. Example: teapot_OPACITY.jpg 

All the textures have to have the same file format. Format allowed: .jpg, .png, .tga, .dds .

When you are creating your model with 3ds Max, you have to assign the material with these channels and maps. And, when you exports the model in osg format, you have to make sure that these map options are checked. 



# XMLoadScene 
XMLoadScene is an utility that loads an OSG scene directly from a XML file. So, you don't have to code anything and recompile the program to configure your scene. You can do it directly in the XML and run the program with the new changes.


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
