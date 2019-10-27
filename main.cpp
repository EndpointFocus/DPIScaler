#include "DpiHelper.h"
#include <string>
#include <regex>
#include <map>
#include <iostream>

const char* HelpMenuText =
	"\n"
	"Usage: DPIScaler[-GetDPIValue | -SetDPIValue | -GetAdapterID][-DisplayAdapter][-DPIValue][-Help]\n"
	"\n"
	"Options:\n"
	"    -GetDPIValue      Gets the DPI scale value of the selected monitor\n"
	"    -SetDPIValue      Sets the DPI scale value of the selected monitor\n"
	"    -GetAdapterID     Gets a list of display adapters\n"
	"    -DisplayAdapter   Specify the display adapter (0 is usually the primary display)\n"
	"    -DPIValue         Set the desired DPI scale value\n"
	"    -Help             Shows the help menu\n"
	"\n"
	"\n"
	"Example: DPIScaler.exe -SetDPIValue -DisplayAdapter=0 -DPIValue=125\n"
	"\n"
	"\n"
	"Created by Liam Matthews.\n"
;

struct DisplayData
{
	LUID m_adapterId;
	int m_targetID;
	int m_sourceID;
	int currentDPI, minDPI, maxDPI, recommendedDPI;

	DisplayData()
	{
		m_adapterId = {};
		m_targetID = m_sourceID = -1;
		currentDPI = minDPI = maxDPI = recommendedDPI = -1;
	}
};
std::map<int, DisplayData> m_displayDataCache;

bool Refresh(bool displaydata)
{
	m_displayDataCache.clear();

	std::vector<DISPLAYCONFIG_PATH_INFO> pathsV;
	std::vector<DISPLAYCONFIG_MODE_INFO> modesV;
	int flags = QDC_ONLY_ACTIVE_PATHS;
	if (false == DpiHelper::GetPathsAndModes(pathsV, modesV, flags))
	{
		// fail
	}
	else
	{
		//succc
	}

	int iIndex = 0;
	for (const auto& path : pathsV)
	{
		//get display name
		auto adapterLUID = path.targetInfo.adapterId;
		auto targetID = path.targetInfo.id;
		auto sourceID = path.sourceInfo.id;

		DISPLAYCONFIG_TARGET_DEVICE_NAME deviceName;
		deviceName.header.size = sizeof(deviceName);
		deviceName.header.type = DISPLAYCONFIG_DEVICE_INFO_GET_TARGET_NAME;
		deviceName.header.adapterId = adapterLUID;
		deviceName.header.id = targetID;
		if (ERROR_SUCCESS != DisplayConfigGetDeviceInfo(&deviceName.header))
		{
			// fail
		}
		else
		{
			std::wstring nameString = std::to_wstring(iIndex) + std::wstring(L". ") + deviceName.monitorFriendlyDeviceName;
			if (DISPLAYCONFIG_OUTPUT_TECHNOLOGY_INTERNAL == deviceName.outputTechnology)
			{
				nameString += L"(internal display)";
			}
			DisplayData dd = {};
			dd.m_adapterId = adapterLUID;
			dd.m_sourceID = sourceID;
			dd.m_targetID = targetID;

			m_displayDataCache[iIndex] = dd;

			if (displaydata) { std::wcout << nameString; }
			iIndex++;
		}
	}
	return true;
}

int main(int argc, char* argv[])
{
	try
	{
		unsigned int ret = 0;

		std::string FullArgs;
		std::string CurrentArg;
		std::string NextArg;
		std::string Arg1;
		std::string Arg2;
		int adapterNum = -1;
		int newDPI = 0;

		bool GetDPIValue = false;
		bool SetDPIValue = false;
		bool GetAdapterID = false;
		bool DisplayAdapter = false;
		bool DPIValue = false;
		bool Help = false;

		auto test1 = DpiHelper::GetPathsAndModes;
		int EndArgs = argc - 2;

		for (int i = 1; i < argc; i++)
		{
			CurrentArg.clear();
			NextArg.clear();
			FullArgs += argv[i];
			std::string CurrentArg = argv[i];
			if (i <= EndArgs) { NextArg = argv[i + 1]; }
			std::transform(CurrentArg.begin(), CurrentArg.end(), CurrentArg.begin(),
				[](unsigned char c) { return std::tolower(c); });

			if (CurrentArg.find("-getdpivalue") == 0 || CurrentArg.find("/getdpivalue") == 0)
			{
				GetDPIValue = true;
			}
			else if (CurrentArg.find("-setdpivalue") == 0 || CurrentArg.find("/setdpivalue") == 0)
			{
				SetDPIValue = true;
			}
			else if (CurrentArg.find("-getadapterid") == 0 || CurrentArg.find("/getadapterid") == 0)
			{
				GetAdapterID = true;
			}

			if (CurrentArg.find("-displayadapter") == 0 || CurrentArg.find("/displayadapter") == 0)
			{
				DisplayAdapter = true;
				Arg1 = argv[i];
				std::size_t found = CurrentArg.find('=');
				if (found != std::string::npos)
				{ 
					adapterNum = std::stoi(Arg1.substr(Arg1.find("=") + 1)); 
				}
				else
				{
					adapterNum = std::stoi(NextArg);
				}
			}
			if (CurrentArg.find("-dpivalue") == 0 || CurrentArg.find("/dpivalue") == 0)
			{
				DPIValue = true;
				Arg2 = argv[i];
				std::size_t found = CurrentArg.find('=');
				if (found != std::string::npos)
				{ 
					newDPI = std::stoi(Arg2.substr(Arg2.find("=") + 1)); 
				}
				else
				{
					newDPI = std::stoi(NextArg);
				}
			}
			if (CurrentArg.find("-help") == 0 || CurrentArg.find("/help") == 0 || CurrentArg.find("-?") == 0 || CurrentArg.find("/?") == 0)
			{
				Help = true;
			}
		}

		if (Help)
		{
			std::cout << HelpMenuText;
		}
		else
		{
			if (!GetDPIValue && !SetDPIValue && GetAdapterID && !DisplayAdapter && !DPIValue)
			{
				Refresh(true);
			}
			else if (GetDPIValue && !SetDPIValue && !GetAdapterID && DisplayAdapter && !DPIValue)
			{
				Refresh(false);
				auto data = DpiHelper::GetDPIScalingInfo(m_displayDataCache[adapterNum].m_adapterId, m_displayDataCache[adapterNum].m_sourceID);
				std::cout << data.current;
			}
			else if (!GetDPIValue && SetDPIValue && !GetAdapterID && DisplayAdapter && DPIValue)
			{
				Refresh(false);
				if (DpiHelper::SetDPIScaling(m_displayDataCache[adapterNum].m_adapterId, m_displayDataCache[adapterNum].m_sourceID, newDPI))
				{
					// good
					ret = 0;
				}
				else
				{
					// bad
					ret = 1;
				}
			}
			else
			{
				std::cout << HelpMenuText;
			}
		}
		return ret;
	}
	catch (...)
	{
		std::cout << "An error has occured when attempting to run the application\n\n";
		std::cout << HelpMenuText;
	}
}