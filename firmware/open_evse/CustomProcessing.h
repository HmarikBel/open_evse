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
#define WAIT_TEMPERATURE_INTERVAL 500
#define CHANGE_ON_UI_TEMPERATURE_INTERVAL 3000

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

#ifdef MY_PORTABLE
#define SENSORS_COUNT 2
#else
#define SENSORS_COUNT 3
#endif MY_PORTABLE

class CustomProcessingClass
{
	IPAddress m_pzem_ip;
	PZEM004T m_pzem;

	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire m_oneWire;

	// Pass our oneWire reference to Dallas Temperature. 
	DallasTemperature m_temperatureSensorsReader;

	// arrays to hold device address 
	DeviceAddress m_insideThermometerAddr[SENSORS_COUNT];

#ifdef MY_PORTABLE
	DeviceAddress m_outsideThermometerAddr;
#endif MY_PORTABLE

	unsigned long m_startE;

	byte m_currentParam;

	bool m_readNext;

	unsigned long m_prevProcessing;
	unsigned long m_prevReadTemperature;
	unsigned long m_startWaitTemperature;
	bool m_temperatureRequested;

	byte m_temperatureToShown;
	unsigned long m_prevTemperatureShow;

	unsigned long m_startReady;

	float readTemperature(DeviceAddress addr, float prevValue);



 public:

	float m_temperature[3];
	float m_temperatureMax[3];

	long m_eTotal;
	long m_e;
	int m_p;
	float m_c;
	int m_v;



	bool m_backLightIsOn;

	CustomProcessingClass():m_pzem_ip(192, 168, 1, 1), m_pzem(ENERGY_RX_PIN, ENERGY_TX_PIN),
		m_oneWire(ONE_WIRE_BUS_PIN), m_temperatureSensorsReader(&m_oneWire)
	{
		m_pzem.setReadTimeout(70);
	}

	void init();

	void reset();

	void process();

	void readTemperature();

	float getHigherTemperature() 
	{
		float maxTemperature = 0;
		for (float i : m_temperature)
		{
			if (i > maxTemperature)
				maxTemperature = i;
		}

		return maxTemperature;
	}

	void getTemperatureToShow(char* buffer);

	void checkBackLight();

	void readPZEM();

	void searchOneWire();

	void printOneWireAddress(byte addr[8]);

	void SetTemperature(byte index, float temperature)
	{
		m_temperature[index] = temperature;
		if (temperature > m_temperatureMax[index])
		{
			m_temperatureMax[index] = temperature;
		}
	}
};

extern CustomProcessingClass CustomProcessing;

#endif

