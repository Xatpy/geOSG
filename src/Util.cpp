#include "Util.h"

#include <sstream>
#include <windows.h>
#include <algorithm>
#include <iostream>

#include <osg/ShapeDrawable>
#include <osgDB/ReadFile>



namespace Util
{
	/*int CountOccurrences(std::string strBase, std::string stdFound )
	{
		int count = 0;
		while (std::size_t contain = strBase.find(stdFound,stdFound)) != std::string::npos)
		{
			count++;
			i = contain + 1;
		}
		return count;
	}*/

	void RemoveCharactersFromString(std::string &str, char charToRemove)
	{
		str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
	}

	std::vector<int> FindOccurrencesAndPosOfAString(std::string str)
	{
		std::vector<int> vPositions;

		std::size_t start = 0;
		int occurrences = 0;
		std::string to_find_occurances_of = ",";
		while ((start = str.find(to_find_occurances_of, start)) != std::string::npos) 
		{
			++occurrences;
			vPositions.push_back(start);
			start += to_find_occurances_of.length(); // see the note
		}

		return vPositions;
	}

	osg::Vec3 ParseStringToVec3(std::string str)
	{
		osg::Vec3 d;

		//quito los espacios por si aca.
		Util::RemoveCharactersFromString(str,' ');

		char lookingFor = ',';
		int count = std::count(str.begin(), str.end(), lookingFor);

		//check 2 commas
		if (count == 2)
		{
			float x = atof(str.substr(0, str.find_first_of(',')).c_str());
			float y = atof(str.substr(str.find_first_of(',') + 1 , str.find_last_of(',')).c_str());
			float z = atof(str.substr(str.find_last_of(',') + 1 , str.size() - 1).c_str());

			d.set(osg::Vec3(x,y,z));
		}
		else
		{
			std::cout << "Error parsing StringToVec3. Invalid format (No 2 commas)" << std::endl;
		}

		return d;
	}

	osg::Vec4 ParseStringToVec4(std::string str)
	{
		osg::Vec4 d;

		//quito los espacios por si aca.
		Util::RemoveCharactersFromString(str,' ');

		std::vector<int> commaPositions = FindOccurrencesAndPosOfAString(str);

		//check 2 commas
		if (commaPositions.size() == 3)
		{
			float x = atof(str.substr(0, commaPositions[0]).c_str());
			float y = atof(str.substr(commaPositions[0] + 1 , commaPositions[1]).c_str());
			float z = atof(str.substr(commaPositions[1] + 1 , commaPositions[2]).c_str());
			float w = atof(str.substr(commaPositions[2] + 1 ,  str.size() - 1).c_str());

			d.set(x,y,z,w);
		}
		else
		{
			std::cout << "Error parsing StringToVec4. Invalid format (No 3 commas)" << std::endl;
		}

		return d;
	}

	void PrintValue (osg::Vec3 v3, std::string vMessageBefore)
	{		
		std::cout << vMessageBefore << v3.x() << " " << v3.y() << " "<< v3.z() <<std::endl;
	}

	void PrintValue (osg::Vec4 v4, std::string vMessageBefore)
	{
		std::cout << vMessageBefore << v4.x() << " " << v4.y() << " "<< v4.z() << " " << v4.w() << std::endl;
	}

	osg::Geode* CreateBox(osg::Vec3 vPosition, float fWidth, osg::Vec4 vColor)
	{
		osg::ref_ptr<osg::Geode> helper = new osg::Geode;
		osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
		shape.get()->setShape( new osg::Box(vPosition, fWidth) );
		shape->setColor( vColor );
		helper->addDrawable(shape);
		return helper.release();
	}

	osg::Geode* CreateBox(osg::Vec3 vPosition, float fLengthX, float fLengthY, float fLengthZ, osg::Vec4 vColor)
	{
		osg::ref_ptr<osg::Geode> helper = new osg::Geode;
		osg::ref_ptr<osg::ShapeDrawable> shape = new osg::ShapeDrawable;
		shape.get()->setShape( new osg::Box(vPosition, fLengthX, fLengthY, fLengthZ) );
		shape->setColor( vColor );
		helper->addDrawable(shape);
		return helper.release();
	}

	bool checkIfFileExists(std::string wFile)
	{
		if (FILE *file = fopen(wFile.c_str(), "r")) 
		{
			fclose(file);
			return true;
		}
		else 
		{
			return false;
		}
	}

	bool checkIfDirectoryExists (const std::string& dirName_in)
	{
		DWORD ftyp = GetFileAttributesA(dirName_in.c_str());
		if (ftyp == INVALID_FILE_ATTRIBUTES)
			return false;  //something is wrong with your path!

		if (ftyp & FILE_ATTRIBUTE_DIRECTORY)
			return true;   // this is a directory!

		return false;    // this is not a directory!
	}

	bool CheckIfCubeMapCorrect	(osg::TextureCubeMap* cubeMap)
	{
		int iNumImages = cubeMap->getNumImages();
		if (iNumImages == 0)
			return false; //No hay texturas cargadas
		//Compruebo que todas las texturas cargadas sean correctas
		bool ret = true;
		for (int i = 0; i < iNumImages; ++i)
		{
			if (!cubeMap->getImage(i))
			{
				//Hay una imagen no cargada, así que devuelvo falso.
				ret = false;
				break;
			}
		}
		return ret;
	}


}