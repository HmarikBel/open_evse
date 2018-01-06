// CustomProcessing.h

#ifndef _CUSTOMPROCESSING_h
#define _CUSTOMPROCESSING_h

#if defined(ARDUINO) && ARDUINO >= 100
	#include "Arduino.h"
#else
	#include "WProgram.h"
#endif

#include <PZEM004T.h>
//#include <Wire.h>
#include <OneWire.h>
#include <DallasTemperatureMy.h>

#define ENERGY_RX_PIN 5
#define ENERGY_TX_PIN 6

#define ONE_WIRE_BUS_PIN 12

#define PROCESS_INTERVAL 500

#define READ_TEMPERATURE_INTERVAL 5000
#define WAIT_TEMPERATURE_INTERVAL 400

const char g_psEnergy[] PROGMEM = "E:";
const char g_psEnergyEmpty[] PROGMEM = "-----";
const char g_psEnergyUnit[] PROGMEM = "Wh";
const char g_psPower[] PROGMEM = "P:";
const char g_psPowerUnit[] PROGMEM = "W";
const char g_psVoltage[] PROGMEM = "V:";
const char g_psVoltageUnit[] PROGMEM = "V";
const char g_psVoltageEmpty[] PROGMEM = "----";
const char g_psCurrent[] PROGMEM = "C:";
const char g_psCurrentUnit[] PROGMEM = "A";


class CustomProcessingClass
{
	IPAddress m_pzem_ip;
	PZEM004T m_pzem;

	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire m_oneWire;

	// Pass our oneWire reference to Dallas Temperature. 
	DallasTemperature m_temperatureSensorsReader;

	// arrays to hold device address 28FFB50361040068
	DeviceAddress m_insideThermometerAddr = { 0x28, 0xFF, 0xB5, 0x03, 0x61, 0x04, 0x00, 0x68 };

	unsigned long m_startE;

	byte m_currentParam;

	bool m_readNext;

	unsigned long m_prevProcessing;
	unsigned long m_prevReadTemperature;
	bool m_temperatureRequested;

	unsigned long m_startReady;

 public:
	long m_eTotal;
	long m_e;
	int m_p;
	float m_c;
	int m_v;

	float m_temperature;

	bool m_backLightIsOn;

	CustomProcessingClass():m_pzem_ip(192, 168, 1, 1), m_pzem(ENERGY_RX_PIN, ENERGY_TX_PIN),
		m_oneWire(ONE_WIRE_BUS_PIN), m_temperatureSensorsReader(&m_oneWire)
	{
		reset();

		m_pzem.setReadTimeout(70);
	}

	void init();

	void reset();

	void process();

	void readTemperature();

	void checkBackLight();

	void readPZEM();
};

extern CustomProcessingClass CustomProcessing;

#endif

