#include <stdio.h>
#include <algorithm>
#include "ModTargetModal.h"

#define MODULATIONTARGETNAMELENGTH 100
typedef struct ModulationTargetOutputEntries_t
{
	char name[MODULATIONTARGETNAMELENGTH];
	int targetid;
	int categoryid;
} ModulationTargetOutputEntries_t;


typedef struct CategoryDesc_t
{
	int id;
	char *label;
} CategoryDesc_t;

const CategoryDesc_t Categories[]=
{
#define CATEGORY(enumname, desc) {Category_##enumname, desc},
#include "../interface/paramdef.h"
#undef CATEGORY
};

std::vector<ModulationTargetOutputEntries_t *> ModulationTargetList;
bool SortList(ModulationTargetOutputEntries_t *A, ModulationTargetOutputEntries_t *B)
{
	if (A->categoryid < B->categoryid) return true;
	return false;
};

bool modulationlistbuilt = false;


void BuildModulationTargetList()
{
	if (modulationlistbuilt == true) return;
	modulationlistbuilt = true;
	ModulationTargetOutputEntries_t *dummy = new ModulationTargetOutputEntries_t();
	dummy->targetid = -1;
	dummy->categoryid = Category_EMPTY;
	sprintf(dummy->name, "No target");

	ModulationTargetList.push_back(dummy);

#define OUTPUT(outname,codecport,codecpin, type,id, style,defaultvalue, label, categorylbl)if (Category_##categorylbl != Category_IGNORE) {ModulationTargetOutputEntries_t *t = new ModulationTargetOutputEntries_t();snprintf(t->name, MODULATIONTARGETNAMELENGTH,"%s", label);t->targetid = Output_##outname; t->categoryid = Category_##categorylbl ; ModulationTargetList.push_back(t);  }
#define OUTPUT_VIRT(outname,codecport,codecpin, type,id, style,defaultvalue, label, categorylbl)if (Category_##categorylbl != Category_IGNORE){ModulationTargetOutputEntries_t *t = new ModulationTargetOutputEntries_t();snprintf(t->name, MODULATIONTARGETNAMELENGTH,"%s", label);t->targetid = Output_##outname; t->categoryid = Category_##categorylbl ; ModulationTargetList.push_back(t);  }
#include "../interface/paramdef.h"
#undef OUTPUT
#undef OUTPUT_VIRT
	;
	
	std::sort (ModulationTargetList.begin(), ModulationTargetList.end(), [](ModulationTargetOutputEntries_t * a, ModulationTargetOutputEntries_t * b) {return a->categoryid < b->categoryid; });
};


int FindModulationListIndex(int c)
{
	for (int i = 0; i < ModulationTargetList.size(); i++)
	{

		if (ModulationTargetList[i]->targetid == c) return i;
	}
	return -1;
}
void TargetList::SetCurrent(int c)
{
	Current = c;
	CurrentIDX = FindModulationListIndex(c);
	if (CurrentIDX  > -1) SetPage(CurrentIDX);
}

void TargetList::SketchRightDelta(int delta)
{
	CurrentIDX = (CurrentIDX + delta + ModulationTargetList.size()) % ModulationTargetList.size();

	Current = ModulationTargetList[CurrentIDX]->targetid;
	((ModTargetModal*)Parent)->SetOutput(Current);


	while (CurrentIDX > PageEnd - 1)
	{
		PageEnd++;
		PageStart++;
	}
	while (CurrentIDX < PageStart)
	{
		PageStart--;
		PageEnd--;
	}
}

void ModTargetModal::SetOutput(int out)
{
	auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
	row->targets[TargetID].outputid = out;
}

void TargetList::Render(bool active, float dt)
{
	int LastCategory = -1;
	ImGui::PushFont(gGuiResources.TinyFont);
	float Line = ImGui::GetTextLineHeight();
	for (int i = 0; i < PageLength; i++)
	{
		
		int idx = PageStart + i;
		if (idx < ModulationTargetList.size() && idx >= 0)
		{
			auto c = ModulationTargetList[idx];
			if (c->categoryid != LastCategory)
			{
				LastCategory = c->categoryid;
				auto R = ImGui::CalcTextSize(Categories[c->categoryid].label);
				ImGui::SetCursorPos(ImVec2(400 - R.x - ParamMasterMargin, i * Line + 40));
				ImGui::TextColored((ImVec4)(ImColor)(c->targetid == Current? gGuiResources.Highlight: gGuiResources.Normal), (Categories[c->categoryid].label));
				

			}
			ImGui::SetCursorPos(ImVec2(400, i * Line + 40));
			ImGui::TextColored((ImVec4)(ImColor)(c->targetid == Current ? gGuiResources.Highlight : gGuiResources.Normal), c->name);
		}
	}
	ImGui::PopFont();
}


void ModTargetModal::SetupLeds()
{
	_screensetup_t::SetupLeds();
}

void ModTargetModal::Action(int action)
{
	switch (action)
	{
	case MenuAction_Ok:
	{
		Parent->Action(MenuAction_CloseModal);
	}
	break;
	case MenuAction_Cancel:
	{
		auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
		row->targets[TargetID].outputid = OriginalOutputID;
		//row->targets[TargetID].depth = OriginalModulation;
		row->targets[TargetID].sourceid = OriginalSourceID;
		Parent->Action(MenuAction_CloseModal);
	}
	break;
	case MenuAction_Remove:
	{
		auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
		row->targets[TargetID].outputid = -1;
		row->targets[TargetID].depth = 0;
		row->targets[TargetID].sourceid = 0;
		Parent->Action(MenuAction_CloseModal);
	}
	break;

	}
}



uint16_t ModTargetModal::GetParameterValue(int param, int encoderset)
{
	auto row = gCurrentPreset.GetModSourceRow(modType, Instance);
	return row->targets[param].depth;
}

void ModTargetModal::Activate()
{
	auto row = gCurrentPreset.GetModSourceRow(modType, Instance);	
	OriginalOutputID = row->targets[TargetID].outputid;
	OriginalSourceID = row->targets[TargetID].sourceid;
	OriginalModulation = row->targets[TargetID].depth;
	
	TheTargetList.SetCurrent(OriginalOutputID);
	TheTargetList.Activate();
	ActiveControl = ControlsInOrder.size() - 1;
	for (int i = 0; i < 11; i++)
	{
		encoders[0][i].enabled = false;
		encoders[0][i].style = MenuEntry_MidValue;
		encoders[0][i].target = 0;
		encoders[0][i].ledmode = ledmode_solid;
		encoders[0][i].SetTitle("modulation");
	}
	encoders[0][TargetID].enabled = true;
	
}

ModTargetModal::ModTargetModal()
{
	EnableButton(11, "OK", MenuEntry_Action, MenuAction_Ok);
	EnableButton(10, "Cancel", MenuEntry_Action, MenuAction_Cancel);
	EnableButton(9, "Remove Modulation", MenuEntry_Action, MenuAction_Remove);


	ControlsInOrder.push_back(&TheTargetList);
	TheTargetList.Parent = this;
}


void GetModulationName(char *t, int len, ModSource_t mod)
{
	switch (mod)
	{
	case Source_none:

	case Source_left_mod: snprintf(t, len, "Left pad"); break;
	case Source_right_mod:snprintf(t, len, "Right pad"); break;
	case Source_x: snprintf(t, len, "Keyboard X"); break;
	case Source_y: snprintf(t, len, "Keyboard Y"); break;
	case Source_z: snprintf(t, len, "Keyboard Z"); break;
	case Source_zprime: snprintf(t, len, "Keyboard Toucher"); break;
	case Source_note: snprintf(t, len, "Keyboard Note"); break;
	case Source_vel: snprintf(t, len, "Keyboard Velocity"); break;
	case Source_left_sus: snprintf(t, len, "Left sustain"); break;
	case Source_right_sus: snprintf(t, len, "Right sustain"); break;
	case Source_left_unac: snprintf(t, len, "Left Una Corda"); break;
	case Source_right_unac: snprintf(t, len, "Right Una Corda"); break;
	case Source_Envelope: snprintf(t, len, "Envelope"); break;
	case Source_LFO: snprintf(t, len, "LFO"); break;
	default:
		snprintf(t, len, "Unknown modulation source");
	}
}


void ModTargetModal::Render(bool active, float dt)
{
	_screensetup_t::Render(active, dt);


	char name[100];
	char txt[100];
	GetModulationName(name, 100, modType);

	if (ModulationSourceHasInstances(modType))
	{
		snprintf(txt, 100, "%s %d slot %d", name, this->Instance, this->TargetID);
	}
	else
	{
		snprintf(txt, 100, "%s slot %d", name, this->TargetID);

	}
	ImGui::PushFont(gGuiResources.BigFont);
	auto R = ImGui::CalcTextSize(txt);
	ImGui::SetCursorPos(ImVec2(1024 / 2 - R.x / 2, ParamMasterMargin));
	ImGui::Text(txt);
	ImGui::PopFont();


}
