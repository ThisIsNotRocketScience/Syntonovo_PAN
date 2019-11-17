#pragma once

#include "FinalPanEnums.h"
#include "PanPreset.h"
#include "gui.h"
#include "ModTargetModal.h"

class KeyZoneSelectorScreen: public _screensetup_t
{
public:
	int ActiveInstance;
	int MaxInstances;
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void TweakParameterValue(int param, int delta);

	KeyZoneSelectorScreen();

	virtual void Action(int a);

	virtual void EncoderPress(FinalEncoderEnum button);
	
	virtual void Deactivate();
	virtual void RenderContent(bool active, float DT);

	virtual void Render(bool active, float DT);
	virtual void Activate();

	void SetTarget(int ScreenID, int instance);
	void SetZone(int Z);
	key_mapping_t* currentmapping;
	int RevertZone;
};


class KeyZoneScreen : public _screensetup_t
{
public:
	int ActiveInstance;
	int MaxInstances;
	virtual uint16_t GetParameterValue(int param, int encoderset);
	virtual void TweakParameterValue(int param, int delta);
	
	KeyZoneScreen();
	virtual void Action(int a);

	virtual void EncoderPress(FinalEncoderEnum button);

	void SetActiveInstance(int id);

	virtual void Deactivate();
	virtual void RenderContent(bool active, float DT);
	virtual void RepeatGoto();

	virtual void Render(bool active, float DT);
	virtual void Activate();

	virtual bool GetToggle(int id);
	virtual void DoToggle(int id);

};

extern KeyZoneSelectorScreen KeyZoneModal;