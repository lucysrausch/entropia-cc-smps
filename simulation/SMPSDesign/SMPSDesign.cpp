//          SMPS Design Suite
//
// This program executes a set of design equations for various SMPS architectures
// and solutions to aid in design for efficiency and reliability.
//
/***************************************************************************
 *   Copyright (C) 2007 by Ken Sarkies                                     *
 *   ksarkies@trinity.asn.au                                               *
 *                                                                         *
 *   This file is part of SMPSDesign.                                      *
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

// This is for a Buck Regulator plus Schottky diode rectifier, high side PMOS

	bool chooseInductor = false;				// Either inductor or ripple current
	float inputVoltage = 12;					// Volts
	float outputVoltage = 6;					// Volts
	float frequency = 62.5;						// kHz
	float highSideSwitchResistance = 0.02;		// Ohms
	float outputCurrent = 5;					// Amps
	int nCap = 1;								// Number of Output Capacitors
	float outputCapacitance = 47*nCap;			// microFarads
	float capacitanceESR = 0.33/nCap;			// Ohms
	float diodeVoltage = 0.3;					// Volts

// Various Voltages

	float inductorVoltageOn = inputVoltage-outputVoltage-highSideSwitchResistance*outputCurrent;
	float inductorVoltageOff = outputVoltage-diodeVoltage;

// This depends also on switch and diode voltage drops

	float dutyCycle = inductorVoltageOff/(inductorVoltageOn+inductorVoltageOff);
	std::cout << "Duty Cycle " << dutyCycle;
	std::cout << std::endl;

// Determine the required inductance from the ripple current rating
// This also sets the continuous/discontinuous boundary

	float rippleCurrentRatio = 0.2;
	float inductance = inductorVoltageOn*dutyCycle/(rippleCurrentRatio*frequency*outputCurrent);	// milliHenry
	if (chooseInductor)
	{
		inductance = 0.2;
		rippleCurrentRatio =inductorVoltageOn*dutyCycle/(inductance*frequency*outputCurrent);
		std::cout << "Ripple Current Ratio " << rippleCurrentRatio;
	}
	else
	{
		if (inductance > 1)							// Set display scale
			std::cout << "Inductance " << inductance << " milliHenry";
		else
			std::cout << "Inductance " << inductance*1000 << " microHenry";
	}
	std::cout << std::endl;

// Determine the output ripple

	float rippleVoltage = outputCurrent*rippleCurrentRatio
						    *((dutyCycle*dutyCycle+(1-dutyCycle)*(1-dutyCycle))
							/(8*outputCapacitance*frequency)
							+capacitanceESR);
	std::cout << "Ripple Voltage " << rippleVoltage*1000 << " milliVolts";
	std::cout << std::endl;

// Losses

	float outputPower = outputCurrent*outputVoltage;
	float outputCapacitorLoss = outputCurrent*rippleCurrentRatio
								*outputCurrent*rippleCurrentRatio*capacitanceESR;
	float highSideSwitchLoss = outputCurrent*outputCurrent*dutyCycle
								*highSideSwitchResistance;
	float lowSideSwitchLoss = outputCurrent*(1-dutyCycle)*diodeVoltage;
	float totalLoss = outputCapacitorLoss+highSideSwitchLoss+lowSideSwitchLoss;
	std::cout << "Output Capacitor Loss " << 100*outputCapacitorLoss/outputPower << "%";
	std::cout << std::endl;
	std::cout << "High Side Switch Loss " << 100*highSideSwitchLoss/outputPower << "%";
	std::cout << std::endl;
	std::cout << "Low Side Switch Loss " << 100*lowSideSwitchLoss/outputPower << "%";
	std::cout << std::endl;
	std::cout << "Efficiency " << 100*(1-totalLoss/outputPower) << "%";
	std::cout << std::endl;
}
