#include "skse/PapyrusNativeFunctions.h"

class TESObjectWEAP;
#include "skse/GameTypes.h"


#include "skse/GameReferences.h"




namespace IH_SKSEScript
{
	UInt32 testHeadShot(StaticFunctionTag *base, TESObjectREFR* obj, TESObjectWEAP* weapon, UInt32 hasHelmet);

	bool HasNode(StaticFunctionTag * base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson);
	
	float GetNodePositionX(StaticFunctionTag * base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson);

	float GetNodePositionY(StaticFunctionTag * base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson);

	float GetNodePositionZ(StaticFunctionTag * base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson);

	NiAVObject* hasArrowInNode(NiAVObject	* node);

	int checkForHeadshots(NiNode* baseNode);

	void findArrows(NiNode* node, int c, NiAVObject * uselessNode);

	bool isHeadShot(NiAVObject* arrow);

	int getDamage(TESObjectWEAP* weapon);

	float testHeadShotForPlayer(StaticFunctionTag *base, TESObjectREFR* obj, float cameraState, UInt32 hasHelmet);

	bool RegisterFuncs(VMClassRegistry* registry);

	void updateDamages(StaticFunctionTag *base, float d1, float d2, float d3);

	void addArrowName(std::string name);






}
