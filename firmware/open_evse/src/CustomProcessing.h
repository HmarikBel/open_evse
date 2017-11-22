// CustomProcessing.h

#ifndef _CUSTOMPROCESSING_h
#define _CUSTOMPROCESSING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <PZEM004T.h>

#define ENERGY_RX_PIN 5
#define ENERGY_TX_PIN 6
#define PROCESS_INTERVAL 500

const char g_psEnergy[] PROGMEM = "E:";
const char g_psEnergyEmpty[] PROGMEM = "-----";
const char g_psEnergyUnit[] PROGMEM = "Wh";
const char g_psPower[] PROGMEM = "P:";
const char g_psPowerUnit[] PROGMEM = "W";
const char g_psVoltage[] PROGMEM = "V:";
const char g_psVoltageUnit[] PROGMEM = "V";
const char g_psVoltageEmpty[] PROGMEM = "----";
const char g_psCurrent[] PROGMEM = "C:00.0";
const char g_psCurrentUnit[] PROGMEM = "A";

class CustomProcessingClass
{
	IPAddress m_pzem_ip;
	PZEM004T m_pzem;

	int m_startE;

	byte m_currentParam;

	bool m_readNext;

	unsigned long m_prevProcessing;

 public:
	int m_e;
	int m_p;
	float m_c;
	int m_v;

	CustomProcessingClass():m_pzem_ip(192, 168, 1, 1), m_pzem(ENERGY_RX_PIN, ENERGY_TX_PIN)
	{
		reset();

		m_pzem.setReadTimeout(60);
	}

	void init();

	void reset();

	void process();
};

extern CustomProcessingClass CustomProcessing;

#endif

