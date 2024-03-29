#pragma once

#ifndef ARPSETTINGS_H
#define ARPSETTINGS_H

#ifndef __max
#define __max(x,y) (((x)>(y))?(x):(y))
#endif

#ifndef __min
#define __min(x,y) (((x)<(y))?(x):(y))
#endif

#ifdef WIN32
#define PACKED
#else
#define PACKED  __attribute__((packed))
#endif

typedef enum ClockSource_t : char
{
	Clock1,
	Clock2,
	Clock3,
	__ClockSource_Count
} PACKED ClockSource_t;

typedef enum ArpeggioMode_t : char
{
	Arp_Up,
	Arp_Down,
	Arp_UpDown,
	Arp_DownUp,


	Arp_InOrder,
	Arp_ReverseOrder,

	Arp_Random,

	__ArpeggioMode_Count
} PACKED ArpeggioMode_t;

typedef enum ArpeggioOctaves_t : char
{
	Oct_None,
	Oct_One,
	Oct_Two,
	Oct_Three,
	Oct_Four,
	__ArpeggioOctaves_Count
} PACKED ArpeggioOctaves_t;

typedef enum ArpeggioOctaveInterleaving_t : char
{
	Interleave_Pattern,
	Interleave_Note,

	__ArpeggioOctavesInterleaving_Count
} PACKED ArpeggioOctaveInterleaving_t;

#define MAXBOTTOM 32
#define MAXTOP 8
class SpeedMode_t
{
public:
	SpeedMode_t()
	{
		Top = 4;
		Bottom = 4;
	}

	void Delta(int delta) 
	{

		int B = Bottom;
		int T = Top;
		B += delta>0?1:-1;
		if (B > MAXBOTTOM) {
			if (T < MAXTOP)
			{
				T++;
				B = 1;
			}
			else
			{
				B = MAXBOTTOM;
			}
		}
		else
		{
			if (B < 1)
			{
				if (T > 1)
				{
					T--;
					B = MAXBOTTOM;
				}
				else
				{
					B = 1;
				}
			}
		}
		Bottom = B;
		Top = T;
	}
	uint8_t Top;
	uint8_t Bottom;
	
	void DeltaBottom(int delta)
	{
		int B = Bottom + delta;
		if (B < 1) { B = 1; }
		else
		{
			if (B > MAXBOTTOM) B = MAXBOTTOM;
		}
		Bottom = B;
	}

	void DeltaTop(int delta)
	{
		int T = Top + delta;
		if (T < 1) { T = 1; }
		else
		{
			if (T > MAXTOP) T = MAXTOP;
		}
		Top = T;
	}
} PACKED;


typedef enum VelocityMode_t : char
{
	VelMode_MaxVelocity,
	VelMode_LastVelocity,
	VelMode_NoteVelocity,
	__VelocityMode_Count
} PACKED VelocityMode_t;

typedef enum TimeSource_t : char
{
	TimeSource_Internal,
	TimeSource_USB,
	TimeSource_DIN,
	__TimeSource_Count
} PACKED TimeSource_t;

class ClockChannelSettings_t
{
public:
	ClockChannelSettings_t()
	{
		Source = TimeSource_Internal;
		Division.Bottom = 4;
		Division.Top = 4;
	}

	TimeSource_t Source;
	SpeedMode_t Division;
} PACKED;

class ClockSettings_t
{
public:
	ClockSettings_t()
	{
		BPM = 120;
		Running = false;
	}
	ClockChannelSettings_t Channel[3];
	TimeSource_t ResetSource;

	int BPM;
	bool Running;
} PACKED;


class ArpSettings_t
{
public:
	ArpSettings_t()
	{
		Mode = ArpeggioMode_t::Arp_UpDown;
		Transpose = 0;
		Speed.Top = 4;
		Speed.Bottom = 4;
		Octaves = Oct_None;
		//Octaves = Oct_One;
		VelocityMode = VelMode_NoteVelocity;
		ResetTimeOnFirstKey = true;
		KillNotesAfterLastNoteOff = true;
		Interleave = Interleave_Pattern;
		Rhythm = 255;
		TimingSource = TimeSource_Internal;
	}

	TimeSource_t TimingSource;
	uint8_t Rhythm;
	ArpeggioMode_t Mode;
	SpeedMode_t Speed;
	VelocityMode_t VelocityMode;
	ArpeggioOctaveInterleaving_t Interleave;
	ArpeggioOctaves_t Octaves;

	int Transpose;
	bool ResetTimeOnFirstKey;
	bool KillNotesAfterLastNoteOff;
	bool Hold;
} PACKED;

#endif
