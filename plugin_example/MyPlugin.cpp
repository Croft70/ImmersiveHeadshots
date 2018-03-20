#include "MyPlugin.h"


#include <sstream>
#include <stdio.h>
#include <string.h>
#include <chrono>
#include <vector>
#include <typeinfo>

#include "String.h"

#include "skse/GameAPI.h"
#include "skse/GameForms.h"
#include "skse/GameRTTI.h"

#include "skse/GameObjects.h"
#include "skse/GameThreads.h"


#include "skse/NiNodes.h"
#include "skse/NiGeometry.h"



using namespace std::chrono;


template <typename T>
class NiTArray;

namespace IH_SKSEScript {

	int damageMultiplier = 2;
	int baseDamages[3] = {50,50,50};
	int weaponDamage = 1;
	int ignoreHelmet = 1;
	int helmetValue = 0;
	float headshotDistance = 10; // tested by repeatedly shooting faendal
	float headOffset = 15; // tested by repeatedly shooting faendal
	std::vector<std::string> arrowNames;
	std::vector<NiAVObject*> arrows;
	NiAVObject * headNode = NULL;

	UInt32 testHeadShot(StaticFunctionTag *base, TESObjectREFR* obj, TESObjectWEAP* weapon, UInt32 hasHelmet) {
		
		long startTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		float returnValue = -1.0;

		helmetValue = hasHelmet;
		if (ignoreHelmet) helmetValue = 0;

		int headshots = 0;

		if (obj) {
			NiNode * baseNode = obj->GetNiNode();
			if (baseNode) {
				headshots = checkForHeadshots(baseNode);
				if (headshots > 0) {
					returnValue = baseDamages[helmetValue]; 
				}
			}
		}

		long endTime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
		_MESSAGE("Time: %d - %d, = %d TEST", endTime ,startTime, endTime - startTime);

		return returnValue;
	}


	int checkForHeadshots(NiNode* baseNode) {
		BSFixedString headName = "NPC Head [Head]";
		headNode = baseNode->GetObjectByName(&headName.data);

		BSFixedString uselessName = "NPC R Clavicle [RClv]"; // change to parent name
		NiAVObject * uselessNode = baseNode->GetObjectByName(&uselessName.data);

		findArrows(baseNode, 0, uselessNode);
		int headshotCount = 0;
		for (int x = 0; x < arrows.size(); x++) {
			if (isHeadShot(arrows[x])) {
				headshotCount++;
			}
		}

		arrows.clear();
		return headshotCount;
	}

	NiAVObject * ResolveNode(TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson)
	{
		if (!obj) return NULL;

		NiAVObject	* result = obj->GetNiNode();

		// special-case for the player, switch between first/third-person
		PlayerCharacter * player = DYNAMIC_CAST(obj, TESObjectREFR, PlayerCharacter);
		if (player && player->loadedState)
			result = firstPerson ? player->firstPersonSkeleton : player->loadedState->node;

		// name lookup
		if (obj && nodeName.data[0] && result)
			result = result->GetObjectByName(&nodeName.data);

		return result;
	}

	NiAVObject* hasArrowInNode(NiAVObject	* node) {

		for (auto s : arrowNames) {
			NiAVObject* temp;
			BSFixedString arrowName = s.c_str();
			temp = node->GetObjectByName(&arrowName.data);
			if (temp) return temp;
		}

		return NULL;
	}

	void findArrows(NiNode	* node,int c, NiAVObject * uselessNode)
	{
		// TODO: This should find all arrows in the body just in case multiple arrows hit at the same time, to avoid any issues.

		while (true) {
			NiAVObject* arrow = hasArrowInNode(node);
			if (arrow) {
				arrow->m_name = uselessNode->m_name;
				arrows.push_back(arrow);
			}
			else break;
		}

	}
	
	bool isHeadShot(NiAVObject* arrow) {
		
		
		std::string s = "NPC Head [Head]";
		std::string s2 = "NPC Spine2 [Spn2]";
		std::string s3 = "NPC Spine1 [Spn1]";

		std::string parentName = arrow->m_parent->m_name;
		_MESSAGE("arrow found, parent: %s", arrow->m_parent->m_name);

		if (s == parentName) {
			//_MESSAGE("hs");
			return true;
		}
		else if(parentName == s2 || parentName == s3){ // check by distance
			_MESSAGE("z: %f, zRot2: %f, new Z: %f", arrow->m_worldTransform.pos.z, headNode->m_localTransform.rot.data[2][0], headNode->m_worldTransform.pos.z + (headNode->m_localTransform.rot.data[2][0] * headOffset));

			// Bump the center of the head from below the neck towards the head, I am not sure if it works since its pain in the ass to test
			
			float z = (headNode->m_worldTransform.pos.z + (headNode->m_localTransform.rot.data[2][0] * headOffset) )- arrow->m_worldTransform.pos.z;

			float dist = sqrt(pow(z, 2));

			_MESSAGE("distance: %f", dist);

			if (dist < headshotDistance) return true;

			
		}

		return false;
	}

	bool HasNode(StaticFunctionTag* base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson)
	{
		NiAVObject	* object = ResolveNode(obj, nodeName, firstPerson);

		return object != NULL;
	}

	float GetNodePositionX(StaticFunctionTag* base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson)
	{
		NiAVObject	* object = ResolveNode(obj, nodeName, firstPerson);

		return object ? object->m_worldTransform.pos.x : 0;
	}

	float GetNodePositionY(StaticFunctionTag* base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson)
	{
		NiAVObject	* object = ResolveNode(obj, nodeName, firstPerson);

		return object ? object->m_worldTransform.pos.y : 0;
	}

	float GetNodePositionZ(StaticFunctionTag* base, TESObjectREFR * obj, BSFixedString nodeName, bool firstPerson)
	{
		NiAVObject	* object = ResolveNode(obj, nodeName, firstPerson);

		return object ? object->m_worldTransform.pos.z : 0;
	}

	float testHeadShotForPlayer(StaticFunctionTag *base, TESObjectREFR* obj, float cameraState, UInt32 hasHelmet) {
		
		float returnValue = -1.0;
		int headshots = 0;
		

		helmetValue = hasHelmet;
		if (ignoreHelmet) helmetValue = 0;

		if (obj) {

			NiNode * baseNode;

			if (cameraState > 0) {

				// third Person
				baseNode = obj->GetNiNode();
			}
			else {
				// first Person
				NiAVObject * result;
				PlayerCharacter * player = DYNAMIC_CAST(obj, TESObjectREFR, PlayerCharacter);
				if (player && player->loadedState) {
					baseNode = player->loadedState->node;
				}
			}
			if (baseNode) {
				headshots = checkForHeadshots(baseNode);
				if (headshots > 0) {
					returnValue = baseDamages[helmetValue]; // CHANGE ME, WEAPON DAMAGE + ARROW + SKILL
				}
			}

		}


		

		return returnValue;
	}

	int getDamage(TESObjectWEAP* weapon) {
		
		_MESSAGE("weapon damage: %d", weapon->damage.GetAttackDamage());
		return weapon->damage.GetAttackDamage();
	}

	float changeDamageMultiplier(StaticFunctionTag *base, float var) {
		damageMultiplier = var - 1;
		if (damageMultiplier > 8) damageMultiplier = 100;
		return 0.0;
	}

	void updateDamages(StaticFunctionTag *base, float d1, float d2, float d3) {
		baseDamages[0] = d1;
		baseDamages[1] = d2;
		baseDamages[2] = d3;
	}

	void changeIgnoreHelmet(StaticFunctionTag *base, UInt32 var) {
		
		ignoreHelmet = var;
	}

	void addArrowName(std::string name) {

		arrowNames.push_back(name);

	}

	bool RegisterFuncs(VMClassRegistry* registry) {
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, UInt32, TESObjectREFR*, TESObjectWEAP*, UInt32>("testHeadShot", "IH_SKSEScript", IH_SKSEScript::testHeadShot, registry));
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, float, TESObjectREFR*,float, UInt32>("testHeadShotForPlayer", "IH_SKSEScript", IH_SKSEScript::testHeadShotForPlayer, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag,float,float>("changeDamageMultiplier", "IH_SKSEScript", IH_SKSEScript::changeDamageMultiplier, registry));
		registry->RegisterFunction(
			new NativeFunction1 <StaticFunctionTag, void, UInt32>("changeIgnoreHelmet", "IH_SKSEScript", IH_SKSEScript::changeIgnoreHelmet, registry));
		registry->RegisterFunction(
			new NativeFunction3 <StaticFunctionTag, void, float,float,float>("updateDamages", "IH_SKSEScript", IH_SKSEScript::updateDamages, registry));

		
		return true;
	}
} 
