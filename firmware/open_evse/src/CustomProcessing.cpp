// 
// 
// 

#include "CustomProcessing.h"
#include "open_evse.h"

void CustomProcessingClass::init()
{
	m_pzem.setAddress(m_pzem_ip);

	m_temperatureSensorsReader.setResolution(m_insideThermometerAddr, 11);
}

void CustomProcessingClass::reset()
{
	m_prevProcessing = 0;
	m_prevReadTemperature = 0;
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
	if (!m_temperatureRequested && millis() - m_prevReadTemperature < READ_TEMPERATURE_INTERVAL)
	{
		return;
	}

	if (!m_temperatureRequested)
	{
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
	}
	else
	{
		if (millis() - m_prevReadTemperature < WAIT_TEMPERATURE_INTERVAL)
		{
			return;
		}

		m_temperature = m_temperatureSensorsReader.getTempC(m_insideThermometerAddr);
		if (m_temperature == DEVICE_DISCONNECTED)
		{
			m_temperature = 0;
		}

		m_temperatureRequested = false;
	}
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

CustomProcessingClass CustomProcessing;

