#pragma once

#ifndef ARPEGGIATOR_H
#define ARPEGGIATOR_H

#include "../../Raspberry/ArpSettings.h"

#ifndef __max
#define __max(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef __min
#define __min(x,y) (((x)<(y))?(x):(y))
#endif

class Synth
{
public:
	virtual void NoteOn(int note, int vel) {}
	virtual void NoteOff(int note, int vel) {}
	virtual void Panic() {};
	virtual void Start() {};
	virtual void Stop() { Panic(); };
};

typedef struct StepData_t
{
	int16_t Length;
	int8_t Velocity;
	int8_t Note;
	int Order;
} StepData_t;

class ArpeggioState : public Synth
{
public:
	ArpeggioState()
	{
		Target = 0;
		NotesOn = 0;
		Panic();
	}

	ArpSettings_t *Settings;
	Synth *Target;

	void Init(ArpSettings_t *settings, Synth *target)
	{
		Settings = settings;
		Target = target;
	}


	virtual void NoteOn(int note, int vel)
	{
		
		if (NotesOn == 0)
		{
			RhythmIndex = 0;
			NoteIndex = 0xffff;
			if (Settings->ResetTimeOnFirstKey) TimerSteps = 0;
		}
		if (NoteActive[note] == 0)
		{
			NotesOn++;
			#ifdef WIN32 
			printf("note++ %d\n", NotesOn);
			#endif			
		}
		NoteActive[note]++;
		NoteVelocity[note] = vel;
		NoteOrder[note] = NotesOn;
		
		Rebuild();
	}

	virtual void NoteOff(int note, int vel)
	{

		if (NoteActive[note] > 0)
		{
			NoteActive[note]--;
			
			if (NoteActive[note] == 0)
			{
				NotesOn--;
#ifdef WIN32 
				printf("note-- %d\n", NotesOn);
				#endif
				for (int i = 0; i < 128; i++)
				{
					if (NoteOrder[i] > 0) NoteOrder[i]--;
				}
			}
		}

		if (NotesOn == 0 && Settings->KillNotesAfterLastNoteOff && LastNote != -1)
		{
			Target->NoteOff(LastNote, 0);
			LastNote = -1;
			Stop();
		}
		Rebuild();
	}
	
	static int CompNote(const void *v1, const void *v2)
	{
		const StepData_t *p1 = (StepData_t *)v1;
		const StepData_t *p2 = (StepData_t *)v2;
		if (p1->Order < p2->Order)
			return -1;
		else if (p1->Order > p2->Order)
			return +1;
		else
			return 0;
	}

	void Rebuild()
	{
		
		int idx = 0;
		bool Sort = false;
		bool SortAsc = true;
		bool Reverse = false;

		switch (Settings->Mode)
		{
		case ArpeggioMode_t::Arp_Down:
			Reverse = true;
		case ArpeggioMode_t::Arp_Up:
		{
			for (int i = 0; i < 128; i++)
			{
				if (NoteActive[i] > 0)
				{
					Pattern[idx].Note = i;
					Pattern[idx].Velocity = NoteVelocity[i];
					Pattern[idx].Length = 127;
					idx++;
				}
			}
		}
		break;

		case ArpeggioMode_t::Arp_DownUp:
		{
			for (int i = 127; i >= 0; i--)
			{
				if (NoteActive[i] > 0)
				{
					Pattern[idx].Note = i;
					Pattern[idx].Velocity = NoteVelocity[i];
					idx++;
				}
			}idx--;
			if (idx < 0) idx = 0;
			for (int i = 0; i < 128; i++)
			{
				if (NoteActive[i] > 0)
				{
					Pattern[idx].Note = i;
					Pattern[idx].Velocity = NoteVelocity[i];
					idx++;
				}
			}
			idx--;
		}
		break;

		case ArpeggioMode_t::Arp_UpDown:
		{
			for (int i = 0; i < 128; i++)
			{
				if (NoteActive[i] > 0)
				{
					Pattern[idx].Note = i;
					Pattern[idx].Velocity = NoteVelocity[i];
					idx++;
				}
			}
			idx--;
			for (int i = 127; i >= 0; i--)
			{
				if (NoteActive[i] > 0)
				{
					Pattern[idx].Note = i;
					Pattern[idx].Velocity = NoteVelocity[i];
					idx++;
				}
			}idx--;
			if (idx < 0) idx = 0;
		}
		break;
			case ArpeggioMode_t::Arp_Random:
				for (int i = 0; i < 128; i++)
				{
					if (NoteActive[i] > 0)
					{
						Pattern[idx].Note = i;
						Pattern[idx].Velocity = NoteVelocity[i];
						Pattern[idx].Length = 127;
						Pattern[idx].Order = rand();
						idx++;
					}
				}
				Sort = true;
				break;
			case ArpeggioMode_t::Arp_ReverseOrder:
				Reverse = true;
			case ArpeggioMode_t::Arp_InOrder:
				Sort = true;
				for (int i = 0; i < 128; i++)
				{
					if (NoteActive[i] > 0)
					{
						Pattern[idx].Note = i;
						Pattern[idx].Velocity = NoteVelocity[i];
						Pattern[idx].Length = 127;
						Pattern[idx].Order = NoteOrder[i];
						idx++;
					}
				}
				break;

		}
		if (Sort)
		{
			qsort(Pattern, idx, sizeof(StepData_t), CompNote);
		}
		for (int i = 0; i < idx; i++)
		{
			Pattern[i].Note += Settings->Transpose;
			Pattern[i].Length = 127;
		}
		switch (Settings->Octaves)
		{
		case ArpeggioOctaves_t::Oct_One: idx = RepeatOct(idx, 1); break;
		case ArpeggioOctaves_t::Oct_Two: idx = RepeatOct(idx, 2); break;
		case ArpeggioOctaves_t::Oct_Three: idx = RepeatOct(idx, 3); break;
		case ArpeggioOctaves_t::Oct_Four: idx = RepeatOct(idx, 4); break;
		}

		if (Reverse)
		{
#define SWAP(a, b, size)                                                      \
  do                                                                              \
    {                                                                              \
      size_t __size = (size);                                                      \
      char *__a = (a), *__b = (b);                                              \
      do                                                                      \
        {                                                                      \
          char __tmp = *__a;                                                      \
          *__a++ = *__b;                                                      \
          *__b++ = __tmp;                                                      \
        } while (--__size > 0);                                                      \
    } while (0)



			for (int i = 0; i < idx / 2; i++)
			{
				int other = (idx - 1 - i);
				char *A = (char*)&Pattern[i];
				char *B = (char*)&Pattern[other];
				SWAP(A, B, sizeof(StepData_t));

			}
		}

#ifdef WIN32 
		PrintPattern();
		printf("%d notes on\n", NotesOn);
		printf("(generated %d steps out of %d %d notesOn)\n", idx, NotesForMode(Settings->Mode, Settings->Octaves), NotesOn);
#endif
	}
#ifdef WIN32 

	void PrintPattern(int len = -1)
	{
		printf("pattern: ");
		if (len == -1) len = NotesForMode(Settings->Mode, Settings->Octaves);

		if (len == 0) 
		{
			printf("empty.");
		}
		else
		{
			for (int i = 0; i < len; i++)
			{
				printf("(%s%d) ", NoteChar(Pattern[i].Note), Pattern[i].Note / 12);
			}
		}
		printf("\n");
	}
#endif

	const char *NoteChar(int n)
	{
		switch ((n+3) % 12)
		{
		case 0:return "c-";
		case 1:return "c#";
		case 2:return "d-";
		case 3:return "d#";
		case 4:return "e-";
		case 5:return "f-";
		case 6:return "f#";
		case 7:return "g-";
		case 8:return "g#";
		case 9:return "a-";
		case 10:return "a#";
		case 11:return "b-";
		}

		return " ";
	}


	int RepeatOct(int len, int octs)
	{
		if (Settings->Interleave == Interleave_Note)
		{
			for (int i = len - 1; i >= 0; i--)
			{
				int BaseMelody = Pattern[i].Note;
				int BaseLength = Pattern[i].Length;
				int BaseVelocity = Pattern[i].Velocity;

				for (int j = octs; j >= 0; j--)
				{
					int idx = (i * octs) + j;
					Pattern[idx].Note = BaseMelody + j * 12;
					Pattern[idx].Velocity = BaseVelocity;
					Pattern[idx].Length = BaseLength;
				}
			}
		}
		else
		{
			for (int i = 1; i <= octs; i++)
			{
				for (int j = 0; j < len; j++)
				{
					Pattern[len * i + j].Note = Pattern[j].Note + i * 12;
					Pattern[len * i + j].Velocity = Pattern[j].Velocity;
					Pattern[len * i + j].Length = Pattern[j].Length;
				}
			}
		}
		return len * (octs + 1);
	}

	virtual void Panic()
	{
		for (int i = 0; i < 128; i++)
		{
			NoteActive[i] = 0;
			NoteOrder[i] = 0;

		}
		NotesOn = 0;
		NoteIndex = 0xffff;
		NoteTicks = 0;
		LastNote = -1;
		if (Target) Target->Panic();
	}

	int8_t NoteActive[128];
	int16_t NoteOrder[128];
	int8_t NoteVelocity[128];

	int NotesOn;
	int NoteIndex;
	int TimerSteps;
	int NoteTicks;
	int RhythmIndex;
	int LastNote;
	StepData_t Pattern[128 * 5];
	
	virtual void Start()
	{
		TimerSteps = 0;
	};

	void TimerTick()
	{
		TimerSteps--;

		if (TimerSteps <= 0)
		{

			ArpeggioStep(Settings, Target);
			TimerSteps = TimerStepsForSpeedMode(Settings->Speed);
		}
		else
		{
			if (NoteTicks > 0)
			{
				NoteTicks--;
				if (NoteTicks == 0 && LastNote != -1)
				{
					Target->NoteOff(LastNote, 0);
					LastNote = -1;
				}
			}
		}
	}

	int TimerStepsForSpeedMode(SpeedMode_t Speed)
	{
		/*

96 ppqn table
Whole   384    Dotted Whole    576   Whole Triplet   256
Half    192    Dotted Half     288   Half  Triplet   128
Quarter  96    Dotted Quarter  144   Quarter Triplet  64
Eighth   48    Dotted Eighth    72   Eighth Triplet   32
16th     24    Dotted 16th      36   16th Triplet     16
32nd     12    Dotted 32nd      18   32nd Triplet      8
64th      6    Dotted 64th       9   64th Triplet      4
128th     3    Dotted 128th     --   128th Triplet     2

*/

		return (96 / __max(1, __min(96,Speed.Bottom)));
	}

	int NotesForMode(ArpeggioMode_t Mode, ArpeggioOctaves_t Octs)
	{
		int notes = NotesOn;

		switch (Mode)
		{
		case Arp_Up:
		case Arp_Down:
		case Arp_Random:
		case Arp_InOrder:
		case Arp_ReverseOrder:
			break;
		case Arp_UpDown:
		case Arp_DownUp:
		{
			if (notes >= 2) notes = ((NotesOn - 2) * 2) + 2; else notes = NotesOn;
		}


		}
		switch (Octs)
		{
		case ArpeggioOctaves_t::Oct_One: notes *= 2; break;
		case ArpeggioOctaves_t::Oct_Two:notes *= 3; break;
		case ArpeggioOctaves_t::Oct_Three:notes *= 4; break;
		case ArpeggioOctaves_t::Oct_Four:notes *= 5; break;

		}

		return notes;
	}

	void ArpeggioStep(ArpSettings_t *Settings, Synth *Target)
	{
		if (NotesOn == 0) return;
		if ((Settings->Rhythm & (1 << RhythmIndex)) > 0)
		{
			NoteIndex++;
			int NFM = NotesForMode(Settings->Mode, Settings->Octaves);
			if (NFM <= 0)
			{
				if (LastNote != -1)
				{
					Target->NoteOff(LastNote, 0);
					LastNote = -1;
				}
				return;
			}
			if (NoteIndex >= NFM) NoteIndex = 0;

			if (Pattern[NoteIndex].Velocity > 0)
			{
				if (LastNote != -1)
				{
					Target->NoteOff(LastNote, 0);
					LastNote = -1;
				}
				Target->NoteOn(Pattern[NoteIndex].Note, Pattern[NoteIndex].Velocity);
				LastNote = Pattern[NoteIndex].Note;
				NoteTicks = __max(1, ((Pattern[NoteIndex].Length * TimerStepsForSpeedMode(Settings->Speed)) >> 8));
			}
		}
		RhythmIndex = (RhythmIndex + 1) % 8;

	}
};


#endif
