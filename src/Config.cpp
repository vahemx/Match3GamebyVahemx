#include "Config.hpp"
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include <exception>
#include <sstream>

constexpr int ROWS_NUM_DEFAULT = 7;
constexpr int COLS_NUM_DEFAULT = 6;
constexpr int ROWS_COLS_MAX_NUM = 8;
constexpr int ROWS_COLS_MIN_NUM = 4;
constexpr int GEM_COLORS_NUM_DEFAULT = 5;
constexpr int GEM_COLORS_NUM_MAX = 5;
constexpr int GEM_COLORS_NUM_MIN = 3;
constexpr int OBJECTIVES_COUNT_DEFAULT = 3;
constexpr int OBJECTIVES_COUNT_MAX = 3;
constexpr int MOVE_COUNT_DEFAULT = 17;
constexpr int RED_COLOR_OBJECTIVE_COUNT = 11;
constexpr int BLUE_COLOR_OBJECTIVE_COUNT = 9;
constexpr int ORANGE_COLOR_OBJECTIVE_COUNT = 20;



Config::Config()
{
	m_gemColorsNames.push_back("red");
	m_gemColorsNames.push_back("blue");
	m_gemColorsNames.push_back("orange");
	m_gemColorsNames.push_back("green");
	m_gemColorsNames.push_back("violet");

	initConfig();
}

void Config::initConfig()
{
	try
	{
		std::ifstream jsonFile("config.json");
		if (!jsonFile.is_open())
		{
			initDefaultConfig();
		}
		else
		{
			std::stringstream ss;
			ss << jsonFile.rdbuf();

			std::string err;
			Json jsonRoot = Json::parse(ss.str(), err);
			if (jsonRoot.is_null()) {
				throw std::exception("Failed to load json data");
			}

			if (!jsonRoot["moves"].is_null() && jsonRoot["moves"].is_number()
				&& !jsonRoot["rows"].is_null() && jsonRoot["rows"].is_number()
				&& !jsonRoot["columns"].is_null() && jsonRoot["columns"].is_number()
				&& !jsonRoot["colors_count"].is_null() && jsonRoot["colors_count"].is_number()
				&& !jsonRoot["objectives"].is_null() && jsonRoot["objectives"].is_object()
				)
			{
				m_moveCount = jsonRoot["moves"].int_value();
				m_rowsCount = jsonRoot["rows"].int_value();
				m_columnsCount = jsonRoot["columns"].int_value();
				m_gemColorsCount = jsonRoot["colors_count"].int_value();
				std::ostringstream stringStream;

				if (m_gemColorsCount < GEM_COLORS_NUM_MIN || m_gemColorsCount > GEM_COLORS_NUM_MAX)
				{
					stringStream << "Invalid configuration of Gem Colors number: "
						<< "Min - " << GEM_COLORS_NUM_MIN
						<< " Max - " << GEM_COLORS_NUM_MAX
						<< std::endl;
					std::string msg = stringStream.str();
					throw std::exception(msg.c_str());
				}

				if (m_rowsCount < ROWS_COLS_MIN_NUM || m_columnsCount < ROWS_COLS_MIN_NUM || m_rowsCount> ROWS_COLS_MAX_NUM || m_columnsCount > ROWS_COLS_MAX_NUM)
				{
					stringStream << "Invalid configuration of Rows and Columns count: "
						<< "Min - " << ROWS_COLS_MIN_NUM
						<< " Max - " << ROWS_COLS_MAX_NUM
						<< std::endl;
					std::string msg = stringStream.str();
					throw std::exception(msg.c_str());
				}



				Json::object objectives = jsonRoot["objectives"].object_items();
				int objectivesSize = objectives.size();
				if (objectivesSize > OBJECTIVES_COUNT_MAX || objectivesSize < 1)
				{
					stringStream << "Invalid configuration of objectives count: "
						<< "Min - " << 1
						<< " Max - " << OBJECTIVES_COUNT_MAX
						<< std::endl;
					std::string msg = stringStream.str();
					throw std::exception(msg.c_str());
				}
				for (auto &item : objectives)
				{
					std::string colorName = item.first;
					if (std::find(m_gemColorsNames.begin(), m_gemColorsNames.end(), colorName) == m_gemColorsNames.end())
					{
						stringStream << "Invalid color name in objectives: "
							<< "Available names: " << "red, blue, orange, green, violet"
							<< std::endl;
						std::string msg = stringStream.str();
						throw std::exception(msg.c_str());
					}
					if (!objectives[colorName].is_null() && objectives[colorName].is_number())
					{
						if (m_gemObjectivesMap.count(colorName))
						{
							stringStream << "Invalid input: multiple input of same Gem Color in objectives."
								<< std::endl;
							std::string msg = stringStream.str();
							throw std::exception(msg.c_str());
						}
						int value = objectives[colorName].int_value();
						m_gemObjectivesMap[colorName] = value;
					}
					else
					{
						stringStream << "Invalid input of objectives: "
							<< std::endl;
						std::string msg = stringStream.str();
						throw std::exception(msg.c_str());
					}
				}
			}
			else
			{
				throw std::exception("Invalid json configuration file");
			}
		}
	}
	catch (std::exception e)
	{
		std::cout << "Exception occured: " << e.what() << std::endl;
		throw;
	}
	


}

void Config::initDefaultConfig()
{
	m_moveCount = MOVE_COUNT_DEFAULT;
	m_rowsCount = ROWS_NUM_DEFAULT;
	m_columnsCount = COLS_NUM_DEFAULT;
	m_gemColorsCount = GEM_COLORS_NUM_DEFAULT;
	m_gemObjectivesMap["red"] = RED_COLOR_OBJECTIVE_COUNT;
	m_gemObjectivesMap["blue"] = BLUE_COLOR_OBJECTIVE_COUNT;
	m_gemObjectivesMap["orange"] = ORANGE_COLOR_OBJECTIVE_COUNT;

	createDefaultConfigFile();
}

void Config::createDefaultConfigFile()
{
	try
	{
		std::ofstream jsonOutFile("config.json");
		if (jsonOutFile.is_open())
		{
			m_jsonObject = Json::object{
				{ "moves", MOVE_COUNT_DEFAULT },
				{ "rows", ROWS_NUM_DEFAULT },
				{ "columns", COLS_NUM_DEFAULT },
				{ "colors_count", GEM_COLORS_NUM_DEFAULT },
				{ "objectives", Json::object { { "red", RED_COLOR_OBJECTIVE_COUNT }, { "blue", BLUE_COLOR_OBJECTIVE_COUNT }, { "orange", ORANGE_COLOR_OBJECTIVE_COUNT } } },
			};

			std::string json_str = m_jsonObject.dump();
			jsonOutFile << json_str << std::endl;
		}
		else
		{
			throw std::exception("Failed to create \"config.json\" file");
		}
	}
	catch (std::exception e)
	{
		std::cout << "Exception occured: " << e.what() << std::endl;
		throw;
	}
}