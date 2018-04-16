// 
// 
// 

#include "CustomProcessing.h"
#include "open_evse.h"

void CustomProcessingClass::init()
{
#ifdef MY_PORTABLE
	m_insideThermometerAddr[0] = 0x28;
	m_insideThermometerAddr[1] = 0xFF;
	m_insideThermometerAddr[2] = 0x08;
	m_insideThermometerAddr[3] = 0xD9;
	m_insideThermometerAddr[4] = 0x61;
	m_insideThermometerAddr[5] = 0x17;
	m_insideThermometerAddr[6] = 0x04;
	m_insideThermometerAddr[7] = 0x09;
	m_outsideThermometerAddr[0] = 0x28;
	m_outsideThermometerAddr[1] = 0xFF;
	m_outsideThermometerAddr[2] = 0x71;
	m_outsideThermometerAddr[3] = 0xAD;
	m_outsideThermometerAddr[4] = 0x61;
	m_outsideThermometerAddr[5] = 0x17;
	m_outsideThermometerAddr[6] = 0x04;
	m_outsideThermometerAddr[7] = 0x31;
#else
	m_insideThermometerAddr[0] = 0x28;
	m_insideThermometerAddr[1] = 0xFF;
	m_insideThermometerAddr[2] = 0xB5;
	m_insideThermometerAddr[3] = 0x03;
	m_insideThermometerAddr[4] = 0x61;
	m_insideThermometerAddr[5] = 0x04;
	m_insideThermometerAddr[6] = 0x00;
	m_insideThermometerAddr[7] = 0x68;
	m_outsideThermometerAddr[0] = 0x00;
	m_outsideThermometerAddr[1] = 0xFF;
	m_outsideThermometerAddr[2] = 0x71;
	m_outsideThermometerAddr[3] = 0xAD;
	m_outsideThermometerAddr[4] = 0x61;
	m_outsideThermometerAddr[5] = 0x17;
	m_outsideThermometerAddr[6] = 0x04;
	m_outsideThermometerAddr[7] = 0x31;
#endif // MY_PORTABLE

	m_pzem.setAddress(m_pzem_ip);

	m_temperatureSensorsReader.setResolution(m_insideThermometerAddr, 11);

	m_prevProcessing = 0;
	m_prevReadTemperature = 0;
	m_prevTemperatureShow = 0;

	reset();

//	searchOneWire();
//	printOneWireAddress(m_insideThermometerAddr);
//	printOneWireAddress(m_outsideThermometerAddr);
}

void CustomProcessingClass::reset()
{
	m_temperatureRequested = false;
	m_currentParam = 0;
	m_e = 0;
	m_p = 0;
	m_c = 0;
	m_v = 0;
	m_startE = 0;
	m_eTotal = 0;
	m_readNext = true;

	m_backLightIsOn = true;
	m_startReady = 0;
}

void CustomProcessingClass::process()
{
	if (millis() - m_prevProcessing < PROCESS_INTERVAL)
	{
		return;
	}

	if (millis() - m_prevProcessing < PROCESS_INTERVAL * 2)
	{
		m_prevProcessing += PROCESS_INTERVAL;
	}
	else
	{
		m_prevProcessing = millis();
	}

	readTemperature();

	checkBackLight();

	readPZEM();
}

void CustomProcessingClass::readPZEM()
{
	float readedValue;
	switch (m_currentParam)
	{
		case 0: 
		{
			if (m_readNext)
			{
				m_pzem.requestEnergy(m_pzem_ip);
			}

			readedValue = m_pzem.readEnergy(m_pzem_ip);

			m_readNext = readedValue != PZEM_NOT_READY_VALUE;
			
			if (readedValue != PZEM_NOT_READY_VALUE)
			{
				m_eTotal = (long)readedValue;
				if (m_eTotal > 0)
				{
					if (m_startE == 0)
					{
						m_startE = m_eTotal;
					}
					else
					{
						m_e = m_eTotal - m_startE;
					}
				}
				else
				{
					m_e = m_eTotal;
				}

				if (LCD_MAX_CHARS_PER_LINE == 20)
				{
					g_OBD.LcdPrint_P(0, 2, g_psEnergy);
				}
				if (m_e >= 0)
				{
					sprintf(g_sTmp, "%5ld", m_e);
					g_OBD.LcdPrint(LCD_MAX_CHARS_PER_LINE == 20 ? 2 : 0, 2, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 2 : 0, 2, g_psEnergyEmpty);
				}
				g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 7 : 5, 2, g_psEnergyUnit);
			}
			break;
		}
		case 1: 
			if (m_readNext)
			{
				m_pzem.requestPower(m_pzem_ip);
			}

			readedValue = m_pzem.readPower(m_pzem_ip);

			m_readNext = readedValue != PZEM_NOT_READY_VALUE;

			if (readedValue != PZEM_NOT_READY_VALUE)
			{
				m_p = (int)readedValue;

				if (LCD_MAX_CHARS_PER_LINE == 20)
				{
					g_OBD.LcdPrint_P(0, 3, g_psPower);
				}
				if (m_p >= 0)
				{
					sprintf(g_sTmp, "%5d", m_p);
					g_OBD.LcdPrint(LCD_MAX_CHARS_PER_LINE == 20 ? 2 : 0, 3, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 2 : 0, 3, g_psEnergyEmpty);
				}
				g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 7 : 5, 3, g_psPowerUnit);
			}
			break;
		case 2: 
			if (m_readNext)
			{
				m_pzem.requestVoltage(m_pzem_ip);
			}

			readedValue = m_pzem.readVoltage(m_pzem_ip);

			m_readNext = readedValue != PZEM_NOT_READY_VALUE;

			if (readedValue != PZEM_NOT_READY_VALUE)
			{
				m_v = (int)readedValue;
				if (m_v > 150)
				{
#ifdef ARCADIY
					m_v = m_v - 4;
#endif // ARCADIY
					m_v = m_v + 1;
				}

				if (LCD_MAX_CHARS_PER_LINE == 20)
				{
					g_OBD.LcdPrint_P(12, 2, g_psVoltage);
				}
				if (m_v >= 0)
				{
					sprintf(g_sTmp, "%4d", m_v);
					g_OBD.LcdPrint(LCD_MAX_CHARS_PER_LINE == 20 ? 15 : 11, 2, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 15 : 11, 2, g_psVoltageEmpty);
				}
				g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 19 : 15, 2, g_psVoltageUnit);
			}
			break;
		case 3: 
			if (m_readNext)
			{
				m_pzem.requestCurrent(m_pzem_ip);
			}

			readedValue = m_pzem.readCurrent(m_pzem_ip);

			m_readNext = readedValue != PZEM_NOT_READY_VALUE;

			if (readedValue != PZEM_NOT_READY_VALUE)
			{
				m_c = readedValue;

				if (LCD_MAX_CHARS_PER_LINE == 20)
				{
					g_OBD.LcdPrint_P(12, 3, g_psCurrent);
				}
				if (m_c >= 0)
				{
					sprintf(g_sTmp, "%2d.%01d", (int)m_c, (int)((m_c - (int)m_c) * 100));
					g_OBD.LcdPrint(LCD_MAX_CHARS_PER_LINE == 20 ? 15 : 11, 3, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 15 : 11, 3, g_psVoltageEmpty);
				}
				g_OBD.LcdPrint_P(LCD_MAX_CHARS_PER_LINE == 20 ? 19 : 15, 3, g_psCurrentUnit);
			}
			break;
	}

	if (m_readNext)
	{
		m_currentParam++;
	}

	if (m_currentParam > 3)
	{
		m_currentParam = 0;
	}
}

void CustomProcessingClass::readTemperature()
{
	if (!m_temperatureRequested)
	{
		if (millis() - m_prevReadTemperature < READ_TEMPERATURE_INTERVAL)
		{
			return;
		}

		if (millis() - m_prevReadTemperature < READ_TEMPERATURE_INTERVAL * 2)
		{
			m_prevReadTemperature += READ_TEMPERATURE_INTERVAL;
		}
		else
		{
			m_prevReadTemperature = millis();
		}

		m_temperatureSensorsReader.requestTemperatures();
		m_temperatureRequested = true;
		m_startWaitTemperature = millis();

		Serial.println("Request");
	}
	else
	{
		if (millis() - m_startWaitTemperature < WAIT_TEMPERATURE_INTERVAL)
		{
			return;
		}

		m_temperatureInside = m_temperatureSensorsReader.getTempC(m_insideThermometerAddr);
		if (m_temperatureInside == DEVICE_DISCONNECTED)
		{
			m_temperatureInside = 0;
		}

		m_temperatureOutside = m_temperatureSensorsReader.getTempC(m_outsideThermometerAddr);
		if (m_temperatureOutside == DEVICE_DISCONNECTED)
		{
			m_temperatureOutside = 0;
		}

		m_temperatureRequested = false;

		Serial.println("Read");

	}
}

void CustomProcessingClass::getTemperatureToShow(char* buffer, byte* length)
{
	if (millis() - m_prevTemperatureShow >= CHANGE_ON_UI_TEMPERATURE_INTERVAL)
	{
		if (millis() - m_prevTemperatureShow < CHANGE_ON_UI_TEMPERATURE_INTERVAL * 2)
		{
			m_prevTemperatureShow += CHANGE_ON_UI_TEMPERATURE_INTERVAL;
		}
		else
		{
			m_prevTemperatureShow = millis();
		}

		m_temperatureShownInside = !m_temperatureShownInside;
	}

	const char *tempfmt = "%2d.%1dC";

#ifndef MY_PORTABLE
//	if (m_outsideThermometerAddr[0] == 0)
//	{
		sprintf(buffer, "%2d.%1dC", (int)m_temperatureInside, ((int)(m_temperatureInside * 10)) % 10);
//	}
//	else
//	{
#else
		float value = m_temperatureShownInside ? m_temperatureInside : m_temperatureOutside;

		sprintf(buffer, "%s %2d.%1dC", m_temperatureShownInside ? "I" : "O", (int)value, ((int)(value * 10)) % 10);
	//}
#endif

	//Serial.println(buffer);
	//*length = m_outsideThermometerAddr[0] == 0 ? 5 : 7;


#ifdef MY_PORTABLE
		*length = 7;
#else
		*length = 5;
#endif
}

void CustomProcessingClass::checkBackLight()
{
	if (EVSE_STATE_A == g_EvseController.GetState())
	{
		if (0 == m_startReady)
		{
			m_startReady = millis();
		}
		else
		{
			if (m_backLightIsOn && millis() - m_startReady > 300000)
			{
				m_backLightIsOn = false;
				g_OBD.noBackLight();
			}
		}
	}
	else
	{
		m_startReady = 0;
		if (!m_backLightIsOn)
		{
			m_backLightIsOn = true;
			g_OBD.backLight();
		}
	}
}

void CustomProcessingClass::searchOneWire()
{
	//if (insideThermometerAddr[0] == 0)
	{
		char buffer[15];
		byte addr[8];
		bool devicesExists = false;
		while (m_oneWire.search(addr))
		{
			printOneWireAddress(addr);
		}
		if (!devicesExists)
		{
			String log = F("No oneWire addresses.");
//			Data.addLog(log);
			Serial.println(log);
		}
	}
}

void CustomProcessingClass::printOneWireAddress(byte addr[8])
{
	char buffer[5];
	String log = F("OneWire address: ");
	for (uint8_t i = 0; i < 8; i++)
	{
		sprintf(buffer, "%02X", addr[i]);
		log += buffer;
	}
	//			Data.addLog(log);
	Serial.println(log);
}

CustomProcessingClass CustomProcessing;

