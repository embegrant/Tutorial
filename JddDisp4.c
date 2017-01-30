/*                                           Revision History
***************************************************************************************************
    Name          Version    Date modified               Code Modified
 Jayesh Patel     1.00        01/18/2006     Initial Creation to show sampler details as below
                                             ------------------------------------------
											 | ABCDE 01/18/06 12:39 HW:99.99 TW:99.99 |
									 		 | Req:99999  Conf:99999   Flow:99999 Kcf |
				                             ------------------------------------------
 Jayesh Patel     2.00        01/18/2006     Modified to show water sampler details as below
                                             ------------------------------------------
										     |HW:99.99 R1:99999 C1:99999 F1:99999 Kcf |
								 			 |TW:99.99 R2:99999 C2:99999 F2:99999 Kcf |
                               			     ------------------------------------------
 Jayesh Patel     3.00        06/12/2006     Modified to show water sampler details as below
                                             -----------------------------------------
											 |R1:99999 C1:99999 F1:99999Kc TV:99999Kc|
											 |R2:99999 C2:99999 F2:99999Kc TV:99999Kc|
                                	    	 -----------------------------------------
 Jayesh Patel     3.10        06/19/2006     Modified to show water sampler details as below
                                             -----------------------------------------
											 | R1:99999 R2:99999 R3:99999  HW:99.99  |
											 | C1:99999 C2:99999 C3:99999  TW:99.99  |
                                 			 -----------------------------------------
 Suman Reddy      4.00        05/08/2007     Added sampler battery voltage display &
                                             Modified to display based on noOfSamplers

 Suman Reddy      5.00        10/12/2007     Modified GetFirstRow()to display <=40 characters
                                             to accommodate battery voltage
***************************************************************************************************/


#define  CAPPLIC_MOS_ML
#include "c:\mctk610\include\Capplic.h"
#include "string.h"
#include "Globals4.h"
#include "JddDisp4.h"



void dummy_func()
{
}
unsigned char *InitI2PhysPort()
{
    CAPLIC_I2PHYS_MODE_PARMS set_mode_parms;
    short err=0;

	m_gGotTable = 0;
	if (MOSCAD_set_i2phys_func(C_APLIC_SIGNAL_RXD_END, dummy_func)) err=1;
    else if (MOSCAD_set_i2phys_func(C_APLIC_SIGNAL_RETRY, dummy_func)) err=2;
    else if (MOSCAD_set_i2phys_func(C_APLIC_SIGNAL_TIMESYNC, dummy_func)) err=3;
    else if (MOSCAD_get_link_id_key(I2PHYS_PORT, &p_linkKey)) err=4;
    else if (MOSCAD_i2phys_get_key(p_linkKey, &p_i2physKey)) err=5;
    else if (p_i2physKey == NULL) err=6;
    else
    {
		set_mode_parms.mode         =  C_APLIC_PHYS_USER_PROTOCOL;
		set_mode_parms.dvc_key      =  p_i2physKey;
		set_mode_parms.link_key     =  p_linkKey;
		set_mode_parms.no_buf_in_q  =  100;
		set_mode_parms.driver_inx   =  0;
		if (!MOSCAD_i2phys_set_mode(&set_mode_parms))
		{
		    if (MOSCAD_get_table_info(MOSCAD_TABLE_NUMBER, &m_gValTable))
				MOSCAD_error("Flow: Could not get table#0 details");
		    else m_gGotTable = 1;
			GetSiteName();
			return(p_i2physKey);
		}
		else err=7;
	}
	MOSCAD_sprintf((char*)buff, "Flow: Failed to set i2phys functions [err=%d]", err);
	MOSCAD_error((char*)buff);
	return(NULL);
}
void Uninit_I2PhysPort()
{
    CAPLIC_I2PHYS_MODE_PARMS set_mode_parms;  /* I2phys set MODE structure.*/
    set_mode_parms.mode         =  C_APLIC_PHYS_MDLC_PROTOCOL;
    set_mode_parms.dvc_key      =  p_i2physKey;
    set_mode_parms.link_key     =  p_linkKey;
    set_mode_parms.no_buf_in_q  =  100;
    set_mode_parms.driver_inx   =  0;
    MOSCAD_i2phys_set_mode(&set_mode_parms);
}
void GetSiteName()
{
    short len, i;
    if (m_gGotTable)
    {
        colPtr = (short*)(m_gValTable.ColDataPtr[MOSCAD_COL_FOR_SNAME]);
        colPtr += MOSCAD_ROW_FOR_SNAME;
        siteName[0] = ((*colPtr)>>8) & 0x00ff;
        siteName[1] = (*colPtr) & 0x00ff;
        colPtr = (short*)(m_gValTable.ColDataPtr[MOSCAD_COL_FOR_SNAME+1]);
        colPtr += MOSCAD_ROW_FOR_SNAME;
        siteName[2] = ((*colPtr)>>8) & 0x00ff;
        siteName[3] = (*colPtr) & 0x00ff;
        colPtr = (short*)(m_gValTable.ColDataPtr[MOSCAD_COL_FOR_SNAME+2]);
        colPtr += MOSCAD_ROW_FOR_SNAME;
        siteName[4] = ((*colPtr)>>8) & 0x00ff;
        siteName[5] = 0;
        len = strlen(siteName);
        if (len < 5)
        {
            for(i=0; i<5; i++)
               if (i<len)
                   siteName[5-i-1] = siteName[len-i-1];
               else
                   siteName[5-i-1] = ' ';
        }
    }
}
void SendBytes(unsigned char *buff)
{
    static unsigned short bytesCount;
    MOSCAD_i2phys_send_struct(p_i2physKey, buff, strlen((char*)buff), &bytesCount, 0);
}
void JuddDisplay()
{
	unsigned short bytesCount;

	GetFirstRow();
	GetSecondRow();

	buff[0] = 1;
	MOSCAD_i2phys_send_struct(p_i2physKey, buff, 1, &bytesCount, 0); // set cursor at Row:1, Col:1
	SendBytes((unsigned char *)screenStrs[0]); // now write line 1
	buff[0] = 0x2;
	MOSCAD_i2phys_send_struct(p_i2physKey, buff, 1, &bytesCount, 0); // set cursor at Row:2, Col:1
	SendBytes((unsigned char *)screenStrs[1]); // now write line 2
}
void GetFirstRow()
{
	//switch(m_gFlowTable.NumOfCols)
	switch(noOfSamplers)
	{
	case 1:
		MOSCAD_get_datetime(&dateTime);
		if (m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_SBV]>0)
		{
			MOSCAD_sprintf(screenStrs[0], "%2.2d/%2.2d %2.2d:%2.2d BV:%5.2f HW:%5.2f TW:%5.2f",
			dateTime.month, dateTime.date,
			dateTime.hours, dateTime.minutes,
		    m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_SBV]],
			m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_HW]],
			m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TW]]);

		}
		else
		{
			MOSCAD_sprintf(screenStrs[0], "%2.2d/%2.2d/%2.2d %2.2d:%2.2d HW:%5.2f TW:%5.2f",
			dateTime.month, dateTime.date, dateTime.year%100,
			dateTime.hours, dateTime.minutes,
			m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_HW]],
			m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TW]]);

	    }
		break;
	case 2: // two samplers
		//	|R1:99999 C1:99999 F1:99999Kcf TV:99999Kc|
		MOSCAD_sprintf(screenStrs[0], "R1:%5.d C1:%5.d F1:%5.dKc TV:%5.dKc",
			m_gDOColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_DO]],
			m_gDIColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_DI]],
			(int)(m_gAIColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_FLOW]] / 1000),
			(int)(m_gFlowColPtr[0][FLOW_TABLE_INDEX_TV] / 1000));

		break;
	case 3: // three samplers
		//	| R1:99999 R2:99999 R3:99999  HW:99.99  |
		MOSCAD_sprintf(screenStrs[0], " R1:%5.d R2:%5.d R3:%5.d  HW:%5.2f",
			m_gDOColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_DO]],
			m_gDOColPtr[(int)m_gFlowColPtr[1][FLOW_TABLE_INDEX_DO]],
			m_gDOColPtr[(int)m_gFlowColPtr[2][FLOW_TABLE_INDEX_DO]],
			m_gAIColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_HW]]);

		break;
	default:
		strcpy(screenStrs[0], "More than 3 sampler are not supported");
		break;
	}

}
void GetSecondRow()
{
	//switch(m_gFlowTable.NumOfCols)
	switch(noOfSamplers)
	{
	case 1:
		  MOSCAD_sprintf(screenStrs[1], " Req:%5.d  Conf:%5.d   Flow:%5.d Kcf",
			m_gDOColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DO]],
			m_gDIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DI]],
			(int)(m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]] / 1000));
			break;
	case 2: // two samplers
		//	|R2:99999 C2:99999 F2:99999Kcf TV:99999Kc|
		MOSCAD_sprintf(screenStrs[1], "R2:%5.d C2:%5.d F2:%5.dKc TV:%5.dKc",
			m_gDOColPtr[(int)m_gFlowColPtr[1][FLOW_TABLE_INDEX_DO]],
			m_gDIColPtr[(int)m_gFlowColPtr[1][FLOW_TABLE_INDEX_DI]],
			(int)(m_gAIColPtr[(int)m_gFlowColPtr[1][FLOW_TABLE_INDEX_FLOW]] / 1000),
			(int)(m_gFlowColPtr[1][FLOW_TABLE_INDEX_TV] / 1000));
		break;
	case 3: // three samplers
		//	| C1:99999 C2:99999 C3:99999  TW:99.99  |
		MOSCAD_sprintf(screenStrs[1], " C1:%5.d C2:%5.d C3:%5.d  TW:%5.2f",
			m_gDIColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_DI]],
			m_gDIColPtr[(int)m_gFlowColPtr[1][FLOW_TABLE_INDEX_DI]],
			m_gDIColPtr[(int)m_gFlowColPtr[2][FLOW_TABLE_INDEX_DI]],
			m_gAIColPtr[(int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_TW]]);
		break;
	default:
		strcpy(screenStrs[1], "in this version of the software.");
		break;
	}
}

