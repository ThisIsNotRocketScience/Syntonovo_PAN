#include <stdio.h>
#include "FinalPanEnums.h"
#include "PanPreset.h"

#include "gui.h"

#include "ModSourceScreen.h"
#include "HomeScreen.h"
#include "VCF2Structure.h"
#include "ParameterModal.h"
#include "NewModulationModal.h"


#include "ExtraScreenDefinitions.h"

#include "ArpeggiatorScreen.h"
#include "KeyZoneScreen.h"



void cmd_AddCalibrationByte(unsigned char cmd)
{
	SystemScreen* SS = (SystemScreen * )gGui.Screens[SCREEN_SYSTEM];
	SS->calibrationcount++;
	int Osc = cmd >> 4;
	int State = cmd & 0xf;
	if (cmd == 0)
	{
		for (int i = 0; i < 8; i++)
		{
			SS->OscillatorReady[Osc] = false;
			SS->OscillatorOctave[Osc] = -1;
			SS->OscillatorError[Osc] = false;
			SS->CalibrationReady = false;
		}
	}
	if (Osc == 0xf)
	{
		SS->CalibrationReady = true;
	}
	else
	{
		if (State == 0xe)
		{
			SS->OscillatorReady[Osc] = true;
			SS->OscillatorError[Osc] = false;
		}
		else
		{
			if (State == 0xf)
			{
				SS->OscillatorError[Osc] = true;
			}
			else
			{
				SS->OscillatorError[Osc] = false;
				SS->OscillatorOctave[Osc] = State;
			}
			SS->OscillatorReady[Osc] = false;

		}
		SS->CalibrationReady = false;
	}

}


SystemScreen::SystemScreen() : _screensetup_t(SCREEN_SYSTEM)
{
	calibrationcount = 0;
	for (int i = 0; i < 8; i++)
	{

		OscillatorError[i] = false;

		OscillatorOctave[i] = -1;
		OscillatorReady[i] = false;
		CalibrationReady = true;
	}
}

void SystemScreen::Render(bool active, float DT)
{
	_screensetup_t::Render(active, DT);
	if (calibrationcount > 0)
	{
		for (int i = 0; i < 8; i++)
		{
			ImGui::SetCursorPos(ImVec2(300, i * 30+200));
			if (OscillatorReady[i])
			{
				ImGui::Text("oscillator %d: ready",i);
			}
			else
			{
				if (OscillatorError[i])
				{
					ImGui::Text("oscillator %d: ERROR! (last oct is %d)",i, OscillatorOctave[i]);
				}
				else
				{
					if (OscillatorOctave[i] < 0)
					{
						ImGui::Text("oscillator %d: waiting..", i);

					}
					else
					{
						ImGui::Text("oscillator %d: oct %d!", i, OscillatorOctave[i]);

					}

				}
			}
		}

		ImGui::SetCursorPos(ImVec2(300, 9 * 30 + 200));

		if (CalibrationReady)
		{
			ImGui::Text("Calibration ready!");
		}
		else
		{
			ImGui::Text("Calibration in progress!");
		}
	}
}



void Gui::BuildScreens()
{
	for (int i = 0; i < __SCREENS_COUNT; i++) Screens[i] = 0;


	Screens[SCREEN_SYSTEM] = new SystemScreen();
	Screens[SCREEN_PRESETNAME] = new PresetScreen();
	auto BL = new BankSelectScreen();
	BL->Side = Left;
	auto BR = new BankSelectScreen();
	BR->Side = Right;
	Screens[SCREEN_SELECTBANKL] = BL;
	Screens[SCREEN_SELECTBANKR] = BR;

	auto SaveScreen = new SavePresetScreen();
	Screens[SCREEN_PRESETSAVE] = SaveScreen;

	Screens[SCREEN_ARP] = new ArpeggiatorScreen();
	Screens[SCREEN_KEYZONES] = new KeyZoneScreen();

	Screens[SCREEN_TEST] = new ImageScreen(gGuiResources.TestBG);
	Screens[SCREEN_TEST]->AddText(10, 30, "10,30 - tadaa", align_left, font_small);
	Screens[SCREEN_TEST]->AddText(30, 60, "10,60 - tadaa", align_left, font_small);
	for (int i = 0; i < 100; i += 10)
	{
		Screens[SCREEN_TEST]->AddText(50 + 4 * i, 600 - i, "Hmm", align_left, font_small);
	}

	Screens[SCREEN_LOGO] = new ImageScreen(gGuiResources.LogoScreen);
	BL->LedButtonsThatOpenThisScreen.push_back(ledbutton_BankLeft);
	BR->LedButtonsThatOpenThisScreen.push_back(ledbutton_BankRight);

	theParameterBindingModal = new ParameterModal();
	TheNewModulationModal = new NewModulationModal();

	Screens[SCREEN_X] = AddModSourceScreen(SCREEN_X, Source_x, "Keyboard X");
	Screens[SCREEN_X]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BX);
	Screens[SCREEN_Y] = AddModSourceScreen(SCREEN_Y, Source_y, "Keyboard Y");
	Screens[SCREEN_Y]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BY);
	Screens[SCREEN_Z] = AddModSourceScreen(SCREEN_Z, Source_z, "Keyboard Z" );
	Screens[SCREEN_Z]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BZ);
	// TODO ??? what is touch ook al weer????
	Screens[SCREEN_TOUCH] = AddModSourceScreen(SCREEN_TOUCH, Source_zprime,"Keyboard Toucher" );
	Screens[SCREEN_TOUCH]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BTouch);
	Screens[SCREEN_LMOD] = AddModSourceScreen(SCREEN_LMOD, Source_left_mod, "Left Mod Pad");
	Screens[SCREEN_LMOD]->LedButtonsThatOpenThisScreen.push_back(ledbutton_LMod);
	Screens[SCREEN_RMOD] = AddModSourceScreen(SCREEN_RMOD, Source_right_mod, "Right Mod Pad");
	Screens[SCREEN_RMOD]->LedButtonsThatOpenThisScreen.push_back(ledbutton_RMod);
	Screens[SCREEN_LSUS] = AddModSourceScreen(SCREEN_LSUS, Source_left_sus,"Left Sustain" );
	Screens[SCREEN_LSUS]->LedButtonsThatOpenThisScreen.push_back(ledbutton_LSus);
	Screens[SCREEN_RSUS] = AddModSourceScreen(SCREEN_RSUS, Source_right_sus,"Right Sustain");
	Screens[SCREEN_RSUS]->LedButtonsThatOpenThisScreen.push_back(ledbutton_RSus);
	Screens[SCREEN_LUNA] = AddModSourceScreen(SCREEN_LUNA, Source_left_unac, "Left Una Corda");
	Screens[SCREEN_LUNA]->LedButtonsThatOpenThisScreen.push_back(ledbutton_LUna);
	Screens[SCREEN_RUNA] = AddModSourceScreen(SCREEN_RUNA, Source_right_unac,"Right Una Corda");
	Screens[SCREEN_RUNA]->LedButtonsThatOpenThisScreen.push_back(ledbutton_RUna);

	Screens[SCREEN_VELOCITY] = AddModSourceScreen(SCREEN_VELOCITY, Source_vel, "Keyboard Velocity" );
	Screens[SCREEN_VELOCITY]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BVelocity);
	Screens[SCREEN_KEYBOARD] = AddModSourceScreen(SCREEN_KEYBOARD, Source_note,"Keyboard Tracking" );
	Screens[SCREEN_KEYBOARD]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BCV);
	Screens[SCREEN_LFO] = AddModSourceScreen(SCREEN_LFO, Source_LFO,"LFO" );
	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BLFO);

	Screens[SCREEN_ENVELOPE] = AddModSourceScreen(SCREEN_ENVELOPE, Source_Envelope, "Envelope");
	Screens[SCREEN_ENVELOPE]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BEnv);
	Screens[SCREEN_VCF2_structure] = new VCF2StructureScreen();

	Screens[SCREEN_HOME] = new HomeScreen();

	for (int i = 0; i < __SCREENS_COUNT; i++)
	{
		if (Screens[i] == 0) Screens[i] = new _screensetup_t(i);
	}

	Screens[SCREEN_HOME]->SetTitle("Home");

	Screens[SCREEN_HOME]->EncodersThatOpenThisScreen.push_back(encoder_SketchLeft);

	Screens[SCREEN_HOME]->EnableButton(LB2, "System ", MenuEntry_Page, SCREEN_SYSTEM);
	Screens[SCREEN_HOME]->EnableButton(LB3, "MIDI", MenuEntry_Page, SCREEN_MIDI);
	Screens[SCREEN_HOME]->EnableButton(LB4, "VCF1 Mix", MenuEntry_Page, SCREEN_VCF1MIX);
	Screens[SCREEN_HOME]->EnableButton(LB5, "VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_HOME]->EnableButton(LB6, "Cleanfeed Mix", MenuEntry_Page, SCREEN_CLEANMIX);
	Screens[SCREEN_HOME]->EnableButton(LB7, "Keyzones", MenuEntry_Page, SCREEN_KEYZONES);
	Screens[SCREEN_HOME]->EnableButton(LB8, "Calibration", MenuEntry_Page, SCREEN_CALIBRATION);


	Screens[SCREEN_HOME]->EnableButton(RB2, "Save preset", MenuEntry_Page, SCREEN_PRESETSAVE);
	Screens[SCREEN_HOME]->EnableButton(RB3, "Rename Preset", MenuEntry_Page, SCREEN_PRESETNAME);
	Screens[SCREEN_HOME]->EnableButton(RB4, "Delete/revert Preset", MenuEntry_Page, SCREEN_PRESETNAME);

	//	Screens[SCREEN_HOME]->EnableButton(12, "Colors", MenuEntry_Page, SCREEN_COLORS);
//	Screens[SCREEN_HOME]->EnableButton(5, "Reference Lines", MenuEntry_Action, MenuAction_EnableReferenceLines);
//	Screens[SCREEN_HOME]->EnableButton(6, "Test-image", MenuEntry_Action, MenuAction_EnableTestImage);


	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BHome);


	Screens[SCREEN_LOADING]->SetTitle("Loading...");

	Screens[SCREEN_EFFECTS]->ControlsInOrder.push_back(new EffectList());

	Screens[SCREEN_EFFECTS]->EnableButton(2, "Previous", MenuEntry_Action, MenuAction_FX_Prev);
	Screens[SCREEN_EFFECTS]->EnableButton(9, "Next", MenuEntry_Action, MenuAction_FX_Next);

	Screens[SCREEN_VCF2a]->SetTitle("VCF2 A");
	Screens[SCREEN_VCF2a]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_VCF2b]->SetTitle("VCF2 B");
	Screens[SCREEN_VCF2b]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_VCF2c]->SetTitle("VCF2 C");
	Screens[SCREEN_VCF2c]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);
	Screens[SCREEN_VCF2d]->SetTitle("VCF2 D");
	Screens[SCREEN_VCF2d]->EnableButton(12, "to VCF2 Mix", MenuEntry_Page, SCREEN_VCF2MIX);

	Screens[SCREEN_VCF2_structure]->SetTitle("VCF2 routing");
	Screens[SCREEN_VCF1]->EnableButton(12, "to VCF1 Mix", MenuEntry_Page, SCREEN_VCF1MIX);


	Screens[SCREEN_VCO4]->SetTitle("VCO 5");
	Screens[SCREEN_VCO5]->SetTitle("VCO 6");
	Screens[SCREEN_VCO6]->SetTitle("VCO 7");
	Screens[SCREEN_VCO7]->SetTitle("VCO 8");
	Screens[SCREEN_VCO4]->EncodersThatOpenThisScreen.push_back(encoder_VCO4);
	Screens[SCREEN_VCO5]->EncodersThatOpenThisScreen.push_back(encoder_VCO5);
	Screens[SCREEN_VCO6]->EncodersThatOpenThisScreen.push_back(encoder_VCO6);
	Screens[SCREEN_VCO7]->EncodersThatOpenThisScreen.push_back(encoder_VCO7);


	Screens[SCREEN_SYSTEM]->SetTitle("System Settings");
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Recalibrate Oscillators", MenuEntry_Action, MenuAction_CalibrateOscillators);
	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Recalibrate Pads", MenuEntry_Action, MenuAction_CalibratePads);
	Screens[SCREEN_SYSTEM]->EnableButton(7, "Done", MenuEntry_Action, MenuAction_Home);

	
	//	Screens[SCREEN_SYSTEM]->EnableAvailableButton("Polymode bit 0", MenuEntry_Toggle, Switch_POLYMODE1);
	//Screens[SCREEN_SYSTEM]->EnableAvailableButton("Polymode bit 1", MenuEntry_Toggle, Switch_POLYMODE2);
//	Screens[SCREEN_SYSTEM]->EnableAvailableButton("KeyPrio bit 0", MenuEntry_Toggle, Switch_KEYPRIO1);
	//Screens[SCREEN_SYSTEM]->EnableAvailableButton("KeyPrio bit 1", MenuEntry_Toggle, Switch_KEYPRIO2);

	//Screens[SCREEN_SYSTEM]->EnableAvailableButton("Headphone enable", MenuEntry_Toggle, Switch_SELVCF1MOST);

	//Screens[SCREEN_SYSTEM]->EnableButton(7, "Done", Menu);



	Screens[SCREEN_PRESETNAME]->SetTitle("Edit Name/Category");

	Screens[SCREEN_COLORS]->SetTitle("Colors");

	Screens[SCREEN_PORTAMENTO]->SetTitle("Portamento");

	Screens[SCREEN_COLORS]->AddLedControl("Low", GetEncoderX(1) - 20, 300, Led_Low);
	Screens[SCREEN_COLORS]->AddLedControl("High", GetEncoderX(5) - 20, 300, Led_High);
	Screens[SCREEN_COLORS]->AddLedControl("Active", GetEncoderX(9) - 20, 300, Led_Active);
	Screens[SCREEN_COLORS]->EnableButton(7, "Done", MenuEntry_Action, MenuAction_Home);




	auto Lines1 = new EncoderLineDisplay();
	Lines1->fromX = GetEncoderX(1);
	Lines1->fromY = 350;
	Lines1->toEncoders.push_back(0);
	Lines1->toEncoders.push_back(1);
	Lines1->toEncoders.push_back(2);
	Lines1->AddTo(Screens[SCREEN_COLORS]);

	auto Lines2 = new EncoderLineDisplay();
	Lines2->fromX = GetEncoderX(5);
	Lines2->fromY = 350;
	Lines2->toEncoders.push_back(4);
	Lines2->toEncoders.push_back(5);
	Lines2->toEncoders.push_back(6);
	Lines2->AddTo(Screens[SCREEN_COLORS]);

	auto Lines3 = new EncoderLineDisplay();
	Lines3->fromX = GetEncoderX(9);
	Lines3->fromY = 350;
	Lines3->toEncoders.push_back(8);
	Lines3->toEncoders.push_back(9);
	Lines3->toEncoders.push_back(10);

	Lines3->AddTo(Screens[SCREEN_COLORS]);


	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Low: Hue", MenuEntry_LedValue, Led_Low_Hue, -1);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Low: Sat", MenuEntry_LedValue, Led_Low_Sat, -1);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Low: Bright", MenuEntry_LedValue, Led_Low_Bright, -1);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("BlinkSpeed", MenuEntry_LedValue, Led_BlinkSpeed, -1);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("High: Hue", MenuEntry_LedValue, Led_High_Hue, -1);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("High: Sat", MenuEntry_LedValue, Led_High_Sat, -1);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("High: Bright", MenuEntry_LedValue, Led_High_Bright, -1);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Brightness", MenuEntry_LedValue, Led_Brightness, -1);

	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Active: Hue", MenuEntry_LedValue, Led_Active_Hue, -1);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Active: Sat", MenuEntry_LedValue, Led_Active_Sat, -1);
	Screens[SCREEN_COLORS]->EnableAvailableEncoder("Active: Bright", MenuEntry_LedValue, Led_Active_Bright, -1);


	Screens[SCREEN_ENVELOPE]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BEnv);

	Screens[SCREEN_LFO]->LedButtonsThatOpenThisScreen.push_back(ledbutton_BLFO);

	Screens[SCREEN_ARP]->SetTitle("Arpeggiator");
	Screens[SCREEN_ARP]->LedButtonsThatOpenThisScreen.push_back(ledbutton_ArpEdit);

	Screens[SCREEN_VCO1]->AddVCONextPrev();
	Screens[SCREEN_VCO2]->AddVCONextPrev();
	Screens[SCREEN_VCO3]->AddVCONextPrev();
	Screens[SCREEN_VCO4]->AddVCONextPrev();
	Screens[SCREEN_VCO5]->AddVCONextPrev();
	Screens[SCREEN_VCO6]->AddVCONextPrev();
	Screens[SCREEN_VCO7]->AddVCONextPrev();
	Screens[SCREEN_VCO8]->AddVCONextPrev();

	Screens[SCREEN_VCF2a]->AddVCF2NextPrev();
	Screens[SCREEN_VCF2b]->AddVCF2NextPrev();
	Screens[SCREEN_VCF2c]->AddVCF2NextPrev();
	Screens[SCREEN_VCF2d]->AddVCF2NextPrev();


	Screens[SCREEN_VCO1]->SetTitle("VCO1");
	Screens[SCREEN_VCO1]->EncodersThatOpenThisScreen.push_back(encoder_VCO1);

	Screens[SCREEN_VCO2]->SetTitle("VCO2");
	Screens[SCREEN_VCO2]->EncodersThatOpenThisScreen.push_back(encoder_VCO2);

	Screens[SCREEN_VCO3]->SetTitle("VCO3");
	Screens[SCREEN_VCO3]->EncodersThatOpenThisScreen.push_back(encoder_VCO3);

	Screens[SCREEN_VCF1]->SetTitle("VCF1");
	Screens[SCREEN_VCF1]->EncodersThatOpenThisScreen.push_back(encoder_VCF1Freq);

	Screens[SCREEN_VCF2a]->SetTitle("VCF2: A");
	Screens[SCREEN_VCF2a]->EncodersThatOpenThisScreen.push_back(encoder_VCF2a);
	Screens[SCREEN_VCF2a]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);

	Screens[SCREEN_VCF2b]->SetTitle("VCF2: B");
	Screens[SCREEN_VCF2b]->EncodersThatOpenThisScreen.push_back(encoder_VCF2b);
	Screens[SCREEN_VCF2b]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);

	Screens[SCREEN_VCF2c]->SetTitle("VCF2: C");
	Screens[SCREEN_VCF2c]->EncodersThatOpenThisScreen.push_back(encoder_VCF2c);
	Screens[SCREEN_VCF2c]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);

	Screens[SCREEN_VCF2d]->SetTitle("VCF2: D");
	Screens[SCREEN_VCF2d]->EncodersThatOpenThisScreen.push_back(encoder_VCF2d);
	Screens[SCREEN_VCF2d]->EnableAvailableButton("Routing", MenuEntry_Action, MenuAction_OpenVCF2Structure);


	Screens[SCREEN_VCF1MIX]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);
	Screens[SCREEN_VCF2MIX]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);
	Screens[SCREEN_CLEANMIX]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);
	Screens[SCREEN_MASTER]->EnableAvailableButton("Effects", MenuEntry_Action, MenuAction_OpenEffects);

	Screens[SCREEN_EFFECTS]->EnableAvailableButton("Done", MenuEntry_Action, MenuAction_CloseParentModal);




	Screens[SCREEN_TEST]->SetTitle("Test scherm");
	//Screens[SCREEN_TEST]->EnableButton(2, "Button1!");
	//Screens[SCREEN_TEST]->EnableButton(5, "Button2!");
	//Screens[SCREEN_TEST]->EnableButton(11, "Button3!");



	_screensetup_t* current = Screens[SCREEN_TEST];
	int lastbutton = 0;
	int lastencoder = 0;
#define MENU(a,b,c) current = Screens[SCREEN_##a];current->SetTitle(c);lastbutton = 0;lastencoder = 0;
#define ENTRY(a,b,c)  lastencoder = current->EnableAvailableEncoder( a,b,c, lastencoder, true);
#define SKIPENTRY   lastencoder = current->SkipAvailableEncoder(lastencoder+1);
#define CUSTOMENTRY(name, style, target) lastbutton = current->EnableAvailableButton(name ,style, target);
	//ENTRY("Spectrum Mod", MenuEntry_Value, Output_VCF2_CROSSMOD) 
#include "PanUiMap.h"
#undef MENU
#undef ENTRY
#undef SKIPENTRY



	for (int i = 0; i < __SCREENS_COUNT; i++)
	{
		Screens[i]->SetFirstEnabledControlActive();
		Screens[i]->SetupEncoderSet(0);
	}
}

