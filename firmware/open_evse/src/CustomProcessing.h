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


class CustomProcessingClass
{
	IPAddress m_pzem_ip;
	PZEM004T m_pzem;

	// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
	OneWire m_oneWire;

	// Pass our oneWire reference to Dallas Temperature. 
	DallasTemperature m_temperatureSensorsReader;

	// arrays to hold device address 
	DeviceAddress m_insideThermometerAddr;
	DeviceAddress m_outsideThermometerAddr;

	unsigned long m_startE;

	byte m_currentParam;

	bool m_readNext;

	unsigned long m_prevProcessing;
	unsigned long m_prevReadTemperature;
	unsigned long m_startWaitTemperature;
	bool m_temperatureRequested;

	bool m_temperatureShownInside;
	unsigned long m_prevTemperatureShow;

	unsigned long m_startReady;

	float readTemperature(DeviceAddress addr, float prevValue);

	float m_temperatureInside;
	float m_temperatureOutside;
	float m_temperatureInsideMax;
	float m_temperatureOutsideMax;

 public:
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
		if (m_temperatureInside > m_temperatureOutside)
			return m_temperatureInside;
		return m_temperatureOutside;
	}

	void getTemperatureToShow(char* buffer, byte* length);

	void checkBackLight();

	void readPZEM();

	void searchOneWire();

	void printOneWireAddress(byte addr[8]);

	void SetTemperatureInside(float temperature)
	{
		m_temperatureInside = temperature;
		if (temperature > m_temperatureInsideMax)
		{
			m_temperatureInsideMax = temperature;
		}
	}

	void SetTemperatureOutside(float temperature)
	{
		m_temperatureOutside = temperature;
		if (temperature > m_temperatureOutsideMax)
		{
			m_temperatureOutsideMax = temperature;
		}
	}

	float GetTemperatureInside() { return m_temperatureInside; }
	float GetTemperatureOutside() { return m_temperatureOutside; }
	float GetTemperatureInsideMax() { return m_temperatureInsideMax; }
	float GetTemperatureOutsideMax() { return m_temperatureOutsideMax; }
};

extern CustomProcessingClass CustomProcessing;

#endif

