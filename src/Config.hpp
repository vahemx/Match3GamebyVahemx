#ifndef _CONFIG_HPP_
#define _CONFIG_HPP_

#include "json11.hpp"
#include <vector>
#include<string>
#include <map>

using namespace json11;

class Config
{
public:
	Config();
	int getMoveCount() { return m_moveCount; };
	int getRowsCount() { return m_rowsCount; };
	int getColumnsCount() { return m_columnsCount; };
	int getGemColorsCount() { return m_gemColorsCount; };
	std::map<std::string, int> getObjectivesMap() { return m_gemObjectivesMap; };


private:
	void initConfig();
	void initDefaultConfig();
	void createDefaultConfigFile();

	std::vector<std::string> m_gemColorsNames;
	Json m_jsonObject;
	int m_moveCount;
	int m_rowsCount;
	int m_columnsCount;
	int m_gemColorsCount;
	std::map<std::string, int> m_gemObjectivesMap;
};

#endif