// 
// 
// 

#include "CustomProcessing.h"
#include "open_evse.h"

void CustomProcessingClass::init()
{
	m_pzem.setAddress(m_pzem_ip);

	m_temperatureSensorsReader.setResolution(m_insideThermometerAddr, 12);
	m_temperatureSensorsReader.requestTemperatures();
}

void CustomProcessingClass::reset()
{
	m_prevProcessing = 0;
	m_currentParam = 0;
	m_e = 0;
	m_p = 0;
	m_c = 0;
	m_v = 0;
	m_startE = 0;
	m_eTotal = 0;
	m_readNext = true;
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

				g_OBD.LcdPrint_P(0, 2, g_psEnergy);
				if (m_e >= 0)
				{
					sprintf(g_sTmp, "%5ld", m_e);
					g_OBD.LcdPrint(2, 2, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(2, 2, g_psEnergyEmpty);
				}
				g_OBD.LcdPrint_P(7, 2, g_psEnergyUnit);
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

				g_OBD.LcdPrint_P(0, 3, g_psPower);
				if (m_p >= 0)
				{
					sprintf(g_sTmp, "%5d", m_p);
					g_OBD.LcdPrint(2, 3, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(2, 3, g_psEnergyEmpty);
				}
				g_OBD.LcdPrint_P(7, 3, g_psPowerUnit);
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

				g_OBD.LcdPrint_P(10, 2, g_psVoltage);
				if (m_v >= 0)
				{
					sprintf(g_sTmp, "%4d", m_v);
					g_OBD.LcdPrint(15, 2, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(15, 2, g_psVoltageEmpty);
				}
				g_OBD.LcdPrint_P(19, 2, g_psVoltageUnit);
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

				g_OBD.LcdPrint_P(10, 3, g_psCurrent);
				if (m_c >= 0)
				{
					sprintf(g_sTmp, "%2d", (int)m_c);
					g_OBD.LcdPrint(15, 3, g_sTmp);
					sprintf(g_sTmp, "%01d", (int)((m_c - (int)m_c) * 100));
					g_OBD.LcdPrint(18, 3, g_sTmp);
				}
				else
				{
					g_OBD.LcdPrint_P(15, 3, g_psVoltageEmpty);
				}
				g_OBD.LcdPrint_P(19, 3, g_psCurrentUnit);
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
	m_temperature = m_temperatureSensorsReader.getTempC(m_insideThermometerAddr);
	if (m_temperature == DEVICE_DISCONNECTED)
	{
		m_temperature = 0;
	}
	m_temperatureSensorsReader.requestTemperatures();
}

CustomProcessingClass CustomProcessing;

