#include "skse/PluginAPI.h"		// super
#include "skse/skse_version.h"	// What version of SKSE is running?
#include <shlobj.h>				// CSIDL_MYCODUMENTS

#include <fstream>
#include <iostream>
#include <string>

#include <shlwapi.h> // PathRemoveFileSpec
#pragma comment( lib, "shlwapi.lib" )



#include "MyPlugin.h"

static PluginHandle					g_pluginHandle = kPluginHandle_Invalid;
static SKSEPapyrusInterface         * g_papyrus = NULL;

extern "C"	{

	bool SKSEPlugin_Query(const SKSEInterface * skse, PluginInfo * info)	{	// Called by SKSE to learn about this plugin and check that it's safe to load it
		gLog.OpenRelative(CSIDL_MYDOCUMENTS, "\\My Games\\Skyrim\\SKSE\\IH_SKSEScript.log");
		gLog.SetPrintLevel(IDebugLog::kLevel_Error);
		gLog.SetLogLevel(IDebugLog::kLevel_DebugMessage);

		_MESSAGE("IH_SKSEScript");

		// populate info structure
		info->infoVersion =	PluginInfo::kInfoVersion;
		info->name =		"IH_SKSEScript";
		info->version =		1;

		// store plugin handle so we can identify ourselves later
		g_pluginHandle = skse->GetPluginHandle();

		if(skse->isEditor)
		{
			_MESSAGE("loaded in editor, marking as incompatible");

			return false;
		}
		else if(skse->runtimeVersion != RUNTIME_VERSION_1_9_32_0)
		{
			_MESSAGE("unsupported runtime version %08X", skse->runtimeVersion);

			return false;
		}

		// ### do not do anything else in this callback
		// ### only fill out PluginInfo and return true/false

		// supported runtime version
		return true;
	}

	bool SKSEPlugin_Load(const SKSEInterface * skse)	{	// Called by SKSE to load this plugin
		_MESSAGE("IH_SKSEScript loaded");


		char buffer[MAX_PATH]; //or wchar_t * buffer;
		GetModuleFileName(NULL, buffer, MAX_PATH);
		PathRemoveFileSpec(buffer);
		std::string path = buffer;
		path += "\\Data\\skse\\plugins\\";
		path += "IH_arrows.txt";

		_MESSAGE("location: %s", path.c_str());
		char line[500];
		std::fstream fileStream(path.c_str(), std::fstream::in);
		if (fileStream.fail()) _MESSAGE("NOPE %s", strerror(errno));
		while (!fileStream.eof()) {
			fileStream.getline(line, 500);
			_MESSAGE("line: %s", line);
			IH_SKSEScript::addArrowName(line);
		}
		g_papyrus = (SKSEPapyrusInterface *)skse->QueryInterface(kInterface_Papyrus);

		//Check if the function registration was a success...
		bool btest = g_papyrus->Register(IH_SKSEScript::RegisterFuncs);

		if (btest) {
			_MESSAGE("Register Succeeded");
		}

		return true;
	}

};