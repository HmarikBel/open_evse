// 
// 
// 

#include "CustomProcessing.h"
#include "open_evse.h"

void CustomProcessingClass::init()
{
	m_pzem.setAddress(m_pzem_ip);
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

	int e;
	switch (m_currentParam)
	{
		case 0: 
		{
			int e = (int)m_pzem.energy(m_pzem_ip);
			if (e > 0)
			{
				if (m_startE == 0)
				{
					m_startE = e;
				}
				else
				{
					m_e = e - m_startE;
				}
			}
			else
			{
				m_e = e;
			}

			g_OBD.LcdPrint_P(0, 2, g_psEnergy);
			if (m_e > 0)
			{
				sprintf(g_sTmp, "%5dWh", m_e);
				g_OBD.LcdPrint(2, 2, g_sTmp);
			}
			else
			{
				g_OBD.LcdPrint_P(2, 2, g_psEnergyEmpty);
			}
			g_OBD.LcdPrint_P(7, 2, g_psEnergyUnit);
			break;
		}
		case 1: 
			m_p = (int)m_pzem.power(m_pzem_ip);
			g_OBD.LcdPrint_P(0, 3, g_psPower);
			if (m_p > 0)
			{
				sprintf(g_sTmp, "%4d", m_p);
				g_OBD.LcdPrint(2, 3, g_sTmp);
			}
			else
			{
				g_OBD.LcdPrint_P(2, 3, g_psEnergyEmpty);
			}
			g_OBD.LcdPrint_P(7, 3, g_psPowerUnit);
			break;
		case 2: 
			m_v = (int)m_pzem.voltage(m_pzem_ip);
			g_OBD.LcdPrint_P(10, 2, g_psVoltage);
			if (m_v > 0)
			{
				sprintf(g_sTmp, "%3d", m_v);
				g_OBD.LcdPrint(12, 2, g_sTmp);
			}
			else
			{
				g_OBD.LcdPrint_P(12, 2, g_psVoltageEmpty);
			}
			g_OBD.LcdPrint_P(16, 2, g_psVoltageUnit);
			break;
		case 3: 
			m_c = m_pzem.current(m_pzem_ip);
			g_OBD.LcdPrint_P(10, 3, g_psCurrent);
			if (m_c > 0)
			{
				sprintf(g_sTmp, "%2d", (int)m_c);
				g_OBD.LcdPrint(12, 3, g_sTmp);
				sprintf(g_sTmp, "%01d", (int)((m_c - (int)m_c) * 100));
				g_OBD.LcdPrint(15, 3, g_sTmp);
			}
			else
			{
				g_OBD.LcdPrint_P(12, 3, g_psVoltageEmpty);
			}
			g_OBD.LcdPrint_P(16, 3, g_psCurrentUnit);
			break;
	}

	m_currentParam++;

	if (m_currentParam > 3)
	{
		m_currentParam = 0;
	}
}


CustomProcessingClass CustomProcessing;

