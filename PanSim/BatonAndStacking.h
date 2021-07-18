#pragma once

#ifndef BATONANDSTACKING
#define BATONANDSTACKING

#ifndef __min
#define __min(a,b) ((a<b)?(a):(b))
#endif

#ifndef __max
#define __max(a,b) ((a>b)?(a):(b))
#endif

#include <math.h>

#define MAXBATONSTAGE 4


class Baton
{
public:
	float Outputs[MAXBATONSTAGE];

	void Set(int idx, float input)
	{
		if (idx > 0) return;
		float out = input * (MAXBATONSTAGE+1);
		for (int i = 0; i < MAXBATONSTAGE; i++)
		{
			Outputs[i] = __max(0, __min(1, triangle( out-i)));
		}
	}
	float triangle(float in)
	{
		return in > 1 ? 2 - in : in;
	}
	
	float Get(unsigned int idx)
	{
		if (idx < MAXBATONSTAGE) return Outputs[idx];
		return 0;
	}
};

class Stack
{
public:
	float Outputs[MAXBATONSTAGE];

	void Set(int idx, float input)
	{
		if (idx > 0) return;
		float out = input * (MAXBATONSTAGE + 1);
		for (int i = 0; i < MAXBATONSTAGE; i++)
		{
			Outputs[i] = __max(0, __min(1, (out - i)));
		}
	}

	float Get(unsigned int idx)
	{
		if (idx < MAXBATONSTAGE) return Outputs[idx];
		return 0;
	}
};

#endif