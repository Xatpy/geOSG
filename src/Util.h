#pragma once

#ifndef UTIL_H
#define UTIL_H

#include <string>
#include <vector>

#include <osg/Vec3>
#include <osg/Vec4>
#include <osg/Geode>
#include <osg/Image>
#include <osg/TextureCubeMap>

namespace Util
{
	void							RemoveCharactersFromString							(std::string &str, char charToRemove);

	std::vector<int>	FindOccurrencesAndPosOfAString					(std::string str);

	void							PrintValue															(osg::Vec3 v3, std::string vMessageBefore = "");
	void							PrintValue															(osg::Vec4 v4, std::string vMessageBefore = "");

	bool							checkIfFileExists												(std::string wFile);
	bool							checkIfDirectoryExists									(const std::string& dirName_in);

	/***************** OSG UTILITIES *****************/

	osg::Vec3					ParseStringToVec3												(std::string str);
	osg::Vec4					ParseStringToVec4												(std::string str);

	osg::Geode*				CreateBox																(osg::Vec3 vPosition, float fWidth, osg::Vec4 vColor);
	osg::Geode*				CreateBox																(osg::Vec3 vPosition, float fLengthX, float fLengthY, float fLengthZ, osg::Vec4 vColor);

	bool							CheckIfCubeMapCorrect										(osg::TextureCubeMap* cubeMap);
}

#endif