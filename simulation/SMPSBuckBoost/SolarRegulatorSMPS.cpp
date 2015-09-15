//          SMPS Buck Boost Simulator
//
// This program executes a set of design equations for various SMPS architectures
// and solutions to aid in design for efficiency and reliability.
//
/***************************************************************************
 *   Copyright (C) 2007 by Ken Sarkies                                     *
 *   ksarkies@trinity.asn.au                                               *
 *                                                                         *
 *   This file is part of SMPSBuckBoost.                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   The program is distributed in the hope that it will be useful,        *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You may obtain a copy of the GNU General Public License by writing to *
 *   the Free Software Foundation, Inc.,                                   *
 *   51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA.             *
 ***************************************************************************/

#include <iostream>                                 // Base stream classes
#include <fstream>                                  // File I/O
#include <cstring>
#include <cstdlib>
#include <cmath>
using namespace std;

int main(int argc, char ** argv)
{
    double absVoltage = 15; // Absorption voltage for Diamec batteries
// Target output voltage and current
	double voltage = absVoltage;
	double absCurrent = 2.88;  // Current limit for Diamec 7.2AH battery
	double slowCurrent = 0.1;
	double current = absCurrent;
	double R = 0.1;		    // Battery internal resistance ohms

	double buckmax = 14;
	double boostmax = 8;

	double cycle = 16;	// length of cycle us
	double dt = cycle/100;	// Time step us
	double L = 300;		// Inductor uH
	double C = 22000;		// Filter capacitor uF
    double vBatt = 13;  // battery terminal voltage
	int buckCycle = 4;	// Length of s1 pulse
	int delay = 1;		// Delay to start of s2 pulse
	int boostCycle = 0;	// Length of s2 pulse

	bool s1 = true;		// high side switch state
	bool s2 = false;	// low side switch state
	double t = 0;		// time within cycle

	double Vi = 12;     // Input voltage
	double Vd1 = 0;     // Buck diode voltage
	double Id1 = 0;     // Buck diode current
	double Is2 = 0;     // Boost switch current
	double Ii = 0;      // Input current
	double Il = 0;      // Inductor current
	double Vs2 = 0;     // Boost switch voltage
	double Id2 = 0;     // Boost diode current
	double Ic = 0;      // Capacitor current
	double Ir = 0;      // Load current
	double Vc = 12;     // Capacitor voltage - initially the battery voltage

	double VcAvg = 12;
	double IrAvg = 0;
	double cVcAvg = 0;
	double cIrAvg = 0;
	double c = 256;		// Filter coefficient

	std::cout << "Cycle Time," << cycle << ",Time Increment," << dt << ",Buck Duty Cycle," << buckCycle << ",Boost Pulse Delay," << delay << ",Boost Duty Cycle," << boostCycle;
	std::cout << std::endl;
	std::cout << "Buck Duty Cycle," << "Boost Duty Cycle," << "Battery Current" << "," << "Battery Voltage" << "," << "Avg Battery Current" << "," << "Avg Battery Voltage";
	std::cout << std::endl;

// START OF THE SIMULATION LOOP
	for (unsigned long i = 0; i < 60000; i++)
	{

// AVERAGING OF CONTROL VARIABLES
		VcAvg = cVcAvg/c;
		cVcAvg += Vc - VcAvg;
		IrAvg = cIrAvg/c;
		cIrAvg += Ir - IrAvg;

// SMPS MODEL
// The feedback algorithm defines the duty cycle lengths
// This algorithm matches that of the SMPS
// The step time is 0.16us, so this is done every 26/0.16=160us.
		if ((VcAvg < voltage) && (Ir < current))
		{
			if (i > 1000)
			{
				if (buckCycle < buckmax) buckCycle++;
				else if ((boostCycle < boostmax) && ((i % 2048) == 0)) boostCycle++;
			}
		}
		else
		{
			if (boostCycle > 0) boostCycle = 0;
			else if (buckCycle > 4) buckCycle = 0;
		}

// SIMULATION OF H-BRIDGE SMPS BUCK-BOOST CIRCUIT
// The time starts with all variables at zero and s1 turns on with s2 off. After delay
// "delay" s2 turns on. At the end of duty cycle buckCycle s1 turns off. At the end of
// d+boostCycle s2 turns off. The program must be able to switch these in any specified
// order.
// As time progresses set switch states. S1 defines the cycle start.
// Handle case where S2 straddles the end of the cycle.
		for (unsigned long j = 0; j < 160; j++)
		{
			if (t > 0) s1 = true;
			if (t > buckCycle) s1 = false;
			if (delay+boostCycle < cycle)
			{
				if (t > delay) s2 = true;
				if (t > delay+boostCycle) s2 = false;
			}
			else
			{
				if (t > delay+boostCycle-cycle) s2 = false;
				if (t > delay) s2 = true;
			}

// Case of S1 on and S2 off
// As S2 is off, if inductor current flows then diode 2 conducts and Vs2 equals Vc
// Current builds in inductor and flows to capacitor and load
			if (s1 && !s2)
			{
				Vd1 = Vi;
				Id1 = 0;
				Is2 = 0;
				Vs2 = Vc;
				Il += (Vd1 - Vs2)*dt/L;
				Id2 = Il - Is2;
				Ii = Il + Id1;
			}
// Case of S1 on and S2 on
// Inductor current flows back to the source and the capacitor discharges
			else if (s1 && s2)
			{
				Vd1 = Vi;
				Id1 = 0;
				Id2 = 0;
				Vs2 = 0;
				Il += (Vd1 - Vs2)*dt/L;
				Is2 = Il - Id2;
				Ii = Il + Id1;
			}
// Case of S1 off and S2 on
// As S1 is off, if inductor current flows, diode 1 must conduct and so forces Vd1 to zero
// Source is isolated and current flows through the inductor and diode 1
			else if (!s1 && s2)
			{
				Ii = 0;
				Id2 = 0;
				Vs2 = 0;
				Vd1 = 0;
				Il += (Vd1 - Vs2)*dt/L;
				Id1 = Ii - Il;
				Is2 = Il - Id2;
			}
// Case of S1 off and S2 off
// As S1 is off, if inductor current flows, diode 1 must conduct and so forces Vd1 to zero
// As S2 is off, if inductor current flows then diode 2 conducts and Vs2 equals Vc
// Source is isolated and inductor current flows into the capacitor.
			else if (!s1 && !s2)
			{
				Ii = 0;
				Is2 = 0;
				Vd1 = 0;
				Vs2 = Vc;
				Il += (Vd1 - Vs2)*dt/L;
				Id1 = Ii - Il;
				Id2 = Il - Is2;
			}
			Ic = Id2 - Ir;
			Vc += Ic*dt/C;

// BATTERY MODEL
// When the output voltage exceeds the battery terminal voltage the SMPS starts to
// deliver current to the battery and the voltage is clamped.
			if (Vc > vBatt) Ir = (Vc-vBatt)/R;
			else Ir = 0;

// Finish off simulation cycle
			if (t > cycle) t = 0;		// Start next cycle
			else t += dt;
		}

		std::cout << buckCycle << ","  << boostCycle << "," << Ir << "," << Vc << "," << IrAvg << "," << VcAvg;
		std::cout << std::endl;
	}
}
