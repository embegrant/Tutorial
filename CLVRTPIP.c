
/**************************************************************************************************
                                                  Revision History

    Name          Version    Date modified               Code Modified

 Jayesh Patel     1.00                       Initial Creation- Added flow calculation for Culvert Pipe

 Jayesh Patel     2.00

 Ron/Suman        3.00        04/11/2007     Modified diags to select # of samplers based on [FLOW] or
                                             [FLOWx] from .dat file and not no of columns in table#10

 Suman Reddy      4.00        05/08/2007     Added code for Total Flow Calculation &
                                             diag of sampler battery voltage

 Suman Reddy      5.00        10/16/2007     Added code for getConfirmation()

 Suman Reddy      5.00        10/17/2007     Added code for rebootTime,triggerTime,flowTime

 Suman Reddy      5.00        10/20/2007     Added code for confirmTime,resetTime

 Suman Reddy      5.00        01/10/2008     Added code to check no of rows in table 10

 Suman Reddy      5.00        01/14/2008     Added code to check if RESET is 0
***************************************************************************************************/

#define  CAPPLIC_MOS_ML
#include "c:\mctk610\include\Capplic.h"
#include "math.h"
#include "float.h"
#include "stdlib.h"
#include "string.h"
#include "Globals4.h"
#include "CLVRTPIP.h"


float cH1, cH4, cHc, cZ, cYc, cC1, cC2, cC3, cS0, cSc, cQc, cTemp1, cTemp2;
float cA0, cA2, cA3, cAc, cAg, cP2, cP3, cPc, cR0, cR2, cR3, cRc, cK2, cK3;
float cInitialTheta, cTheta, cNewTheta, cTheta2, cTheta3, cBeta, cQ, cDelta;
float cFn1Theta, cFn1OldTheta, cFn2Theta, cFn2OldTheta;
float hw, tw, go;

MOSCAD_DATE_TM reBootDateTime; // Scada Master
MOSCAD_DATE_TM confirmTime[5];
MOSCAD_DATE_TM triggerTime[5];
MOSCAD_DATE_TM flowTime[5];
MOSCAD_DATE_TM resetTime[5];
char msg[300];


const CB_JUMPTBL user_jumptable[] =
{
	{ "FLOW", (USERFUNC)Flow},
	{ 0       , 0 }
};

void Flow() // Just to activate this flash file
{
}
void user_control_function(int control)
{
    switch(control)
    {
    case CB_INIT:
		if (MOSCAD_run_task(CB_TaskF, MainTask, 0) != 0)
			MOSCAD_error("Flow: Could not run task MainTask");
		if (MOSCAD_set_diag_func(110, SWSDIDiag, NULL) != 0)
			MOSCAD_error("Flow: Could not set diag DCF6(110)");
		if (MOSCAD_find_func("LOGMSG", &LogMsgFnPtr))
			MOSCAD_error("Flow: MOSCAD_find_func(LOGMSG) failed");
			MOSCAD_get_datetime(&reBootDateTime);
		break;
	case CB_EXIT:
		MOSCAD_unset_all_diag_funcs();
		break;
    }
}

void MainTask()
{
    unsigned long secCounter;
    int i, sampler;

    float tf;

    m_gAIColPtr = NULL;
    m_gDIColPtr = NULL;
    m_gDOColPtr = NULL;
    secCounter = 0;
    calcTimer = 0;
    InitI2PhysPort();
    //Added below code to check if no of rows in Table 10 are 80
	if (MOSCAD_get_table_info(MOSCAD_FLOW_TABLE_NUMBER, &m_gFlowTable))
	{
		MOSCAD_error("Error: Could not get the flow table details");
		return;
	}
	if(m_gFlowTable.NumOfRows <= MAX_TOKENS)
	{
		MOSCAD_error("Error: Too few rows in Table 10");
		return;
    }

    if (!LoadIniFile())
    {
		if (MOSCAD_get_table_info(MOSCAD_AI_TABLE_NUMBER, &m_gAITable))
		{
			MOSCAD_error("Flow: Could not get AI table");
			return;
		}
		else m_gAIColPtr = (float*)(m_gAITable.ColDataPtr[MOSCAD_AI_TABLE_COL_NUMBER]);
		if (MOSCAD_get_table_info(MOSCAD_DI_TABLE_NUMBER, &m_gDITable))
		{
			MOSCAD_error("Flow: Could not get DI table");
			return;
		}
		else m_gDIColPtr = (short *)(m_gDITable.ColDataPtr[MOSCAD_DI_TABLE_COL_NUMBER]);
		m_gDIStatusColPtr = (unsigned char *)(m_gDITable.ColDataPtr[MOSCAD_DI_TABLE_STATUS_COL_NUM]);
		if (MOSCAD_get_table_info(MOSCAD_DO_TABLE_NUMBER, &m_gDOTable))
		{
			MOSCAD_error("Flow: Could not get DO table");
			return;
		}
		m_gDOColPtr = (short *)(m_gDOTable.ColDataPtr[MOSCAD_DO_TABLE_COL_NUMBER]);
		m_gDOReqColPtr = (unsigned char *)(m_gDOTable.ColDataPtr[MOSCAD_DO_TABLE_REQ_COL_NUMBER]);
		calcTimer = (int)m_gFlowColPtr[0][FLOW_TABLE_INDEX_TIMER];
		for (sampler=0; sampler < noOfSamplers; sampler++)
		{
			m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_STOP_RUN] = 3; // activate the flow calculation routine
			m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_PARAM_EXIST] = 1;
			//Added below to check the reset
			if(m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_RESET] == 0)
	           m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_RESET] = -1;

			if((int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_RESET] != -1)
			{
			m_gResetCounter[sampler] = &m_gDIColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_RESET]];
			m_gPrevResetCounter[sampler] = *m_gResetCounter[sampler];
		    }
			m_gConfirmCounter[sampler] = &m_gDIColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DI]];
			m_gPrevConfirmCounter[sampler] = *m_gConfirmCounter[sampler];
		}
    }
    secCounter = 0;
    for(;;)
    {
		MOSCAD_wait(5000);
		secCounter += 5;
		if (secCounter >= calcTimer)
		{
			secCounter = 0;
			for (sampler=0; sampler < noOfSamplers; sampler++)
			{
				tf = 0.0;
				if (!m_gFlowColPtr[sampler]) break;
				samplerNo = sampler;
				//Added below for loop for CLVRTPIP flow calculation - Suman

                for (i=0; i<(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_NUM_GO]; i++)
				{
					if(FlowSTA2(FLOW_TABLE_INDEX_GO1 + i) ==0)
					MOSCAD_get_datetime(&flowTime[sampler]);
					m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DISCHARGE_G1 + i] =
					m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DISCHARGE];
					tf += m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DISCHARGE_G1 + i];
	            }
		         //Added below statement for including Flow value in Table- Suman
                m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TOTAL_FLOW] = tf;

				TriggerSampler(samplerNo);
			}
		}
		getConfirmation();
		CheckResetRequest();
		JuddDisplay();
    }
    Uninit_I2PhysPort();
}
char *GetFloatStr(float val, char *str)
{
	static char localStr[51], *chPtr;
	chPtr = str ? str:localStr;
	MOSCAD_sprintf(chPtr, "%.3f", val);
	return chPtr;
}

void SWSDIDiag(void *pControl, void *User)
{
	int i, diagCaseNo;


	/*if (m_gFlowTable.NumOfCols == 1) diagCaseNo = 7019;
	else if (m_gFlowTable.NumOfCols == 2) diagCaseNo = 7020;
	else if (m_gFlowTable.NumOfCols == 3) diagCaseNo = 7021;
	else if (m_gFlowTable.NumOfCols == 4) diagCaseNo = 7022;
    */
	//Commented above statements and added below statements- Suman


	if (noOfSamplers == 1) diagCaseNo = 7019;
	else if (noOfSamplers == 2) diagCaseNo = 7020;
	else if (noOfSamplers == 3) diagCaseNo = 7021;
	else if (noOfSamplers == 4) diagCaseNo = 7022;



	MOSCAD_diag_message(pControl, diagCaseNo);
	MOSCAD_diag_push(pControl, 4, FRMT_STRING, "CLVRTPIP", FRMT_STRING, "5.00");
	MOSCAD_sprintf(msg, "%2.2d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d",
		     	       reBootDateTime.month,  reBootDateTime.date, reBootDateTime.year,
		               reBootDateTime.hours, reBootDateTime.minutes, reBootDateTime.seconds);
	MOSCAD_diag_push(pControl, 2, FRMT_STRING, msg);

	for (i=0; i<noOfSamplers; i++)
	{
		if (!m_gFlowColPtr[i]) break;
		samplerNo = i;
		SWSDIDiagDetails(pControl, User);
	}
}
void SWSDIDiagDetails(void *pControl, void *User)
{
	char str1[51], str2[51], str3[51], str4[51], strg2[51];
	int hh, mm, ss, i;
	float ff, tf;

	MOSCAD_diag_push(pControl, 4,
		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_HW],
		FRMT_STRING, GetFloatStr(m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_HW]], 0));
	MOSCAD_diag_push(pControl, 4,
		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TW],
		FRMT_STRING, GetFloatStr(m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TW]], str1));
	str[0] = 0;
	str1[0] = 0;
	tf = 0.0;
	/*for (i=0; i<(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_NUM_PUMP]; i++)
	{
		MOSCAD_sprintf(str2, "%d;", (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RPM1+i]);
		strcat(str1, str2);
		if (m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RPM1+i] >= 0)
			MOSCAD_sprintf(str2, "%.3f;", m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RPM1+i]]);
		else
		{
			if (MOSCAD_bitstatus(m_gDIStatusColPtr, -1 * (short)(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RPM1+i])))
				strcpy(str2, "ON;");
			else strcpy(str2, "OFF;");
		}
		strcat(str, str2);
		tf += m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE1+i];
	}*/
	//Added below statements for Gate position- Suman

	for (i=0; i<(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_NUM_GO]; i++)
	{
			MOSCAD_sprintf(strg2, "%d;", (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_GO1+i]);
			strcat(str1, strg2);
			if (m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_GO1+i] >= 0)
				MOSCAD_sprintf(strg2, "%.3f;", m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_GO1+i]]);
			strcat(str, strg2);
			tf += m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE_G1+i];
	}


	MOSCAD_diag_push(pControl, 4, FRMT_STRING, str1, FRMT_STRING, str);

	 MOSCAD_diag_push(pControl, 4,
				FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DI],
			    FRMT_SHRT, (int)m_gDIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DI]]);

	MOSCAD_sprintf(msg, "%2.2d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d",
	       	   confirmTime[samplerNo].month,  confirmTime[samplerNo].date, confirmTime[samplerNo].year,
	           confirmTime[samplerNo].hours, confirmTime[samplerNo].minutes, confirmTime[samplerNo].seconds);
	MOSCAD_diag_push(pControl, 2, FRMT_STRING, msg);

	MOSCAD_diag_push(pControl, 4,
				FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DO],
			    FRMT_SHRT, (int)m_gDOColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DO]]);

	MOSCAD_sprintf(msg, "%2.2d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d",
			   triggerTime[samplerNo].month,  triggerTime[samplerNo].date, triggerTime[samplerNo].year,
		       triggerTime[samplerNo].hours, triggerTime[samplerNo].minutes, triggerTime[samplerNo].seconds);
	MOSCAD_diag_push(pControl, 2, FRMT_STRING, msg);


    if((int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RESET] != -1)
    {
    MOSCAD_diag_push(pControl, 4,
				FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RESET],
			    FRMT_SHRT, (int)m_gDIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RESET]]);

    }
    else
    {
	MOSCAD_diag_push(pControl, 4,
				FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_RESET],
			    FRMT_SHRT, 0);
    }

	MOSCAD_sprintf(msg, "%2.2d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d",
	       	   resetTime[samplerNo].month,  resetTime[samplerNo].date, resetTime[samplerNo].year,
	           resetTime[samplerNo].hours, resetTime[samplerNo].minutes, resetTime[samplerNo].seconds);
	MOSCAD_diag_push(pControl, 2, FRMT_STRING, msg);

	MOSCAD_diag_push(pControl, 4,
	    		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW],
			    FRMT_STRING, GetFloatStr(m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]], 0));

	MOSCAD_sprintf(msg, "%2.2d/%2.2d/%2.2d %2.2d:%2.2d:%2.2d",
		   	   flowTime[samplerNo].month,  flowTime[samplerNo].date, flowTime[samplerNo].year,
		       flowTime[samplerNo].hours, flowTime[samplerNo].minutes, flowTime[samplerNo].seconds);
    MOSCAD_diag_push(pControl, 2, FRMT_STRING, msg);
	/*MOSCAD_diag_push(pControl, 12,
		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DI],
		FRMT_SHRT, (int)m_gDIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DI]],
		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DO],
		FRMT_SHRT, (int)m_gDOColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DO]],
		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW],
		FRMT_STRING, GetFloatStr(m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]], 0));*/
	MOSCAD_diag_push(pControl, 2,
		FRMT_STRING, GetFloatStr(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TV], str1));
	MOSCAD_diag_push(pControl, 6,
		FRMT_STRING, GetFloatStr(tf, str1),
		FRMT_STRING, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_STOP_RUN]?"Running":"Stopped",
		FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR]);
	if (tf>0) // modified condition on 03/15/06 it was "if (tf)"
	{
		ff = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TV] - m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]];
		ff = ff / tf;
		hh = ff / 3600;
		mm = (ff - hh * 3600) / 60;
		ss = ff - hh * 3600 - mm * 60;
		MOSCAD_sprintf(str1, "%d:%2.2d:%2.2d", hh, mm, ss);
		MOSCAD_diag_push(pControl, 2, FRMT_STRING, str1);
	}
	else MOSCAD_diag_push(pControl, 2, FRMT_STRING, "N/A");
	// NEW DISPLAY VALUE = SBV

	    MOSCAD_diag_push(pControl, 4,
	        FRMT_SHRT, (int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_SBV],
			FRMT_STRING, GetFloatStr(m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_SBV]], 0));

}

void CalculateGlobals()
{
	cH1 = hw - m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_OUTLET];
	cH4 = tw - m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_OUTLET];
	cZ = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_INLET] - m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_OUTLET];
	cYc = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]/2.0 *(1-cos(cTheta));
	cHc = cYc + cZ;
	cC1 = (cH1 - cZ) / go;
	cC2 = cH4 / m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D];
	cC3 = cH4 / cHc;
	cS0 = cZ / m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_L];
        cQc = 0.7093 * pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D], 2.5) *
		pow(cTheta-sin(2*cTheta)/2,1.5)/pow(sin(cTheta),0.5);
	cAc =(pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],2.0)/4.0)*(cTheta-sin(2*cTheta)/2.0);
	cPc = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D] * cTheta;
	cRc = cAc / cPc;
	cSc = pow((cQc / cAc), 2) * pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N], 2) /
		(2.21 * pow(cRc, 4.0/3.0));
	cFn1Theta = cH1-(cQc*cQc/2*m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G]*cAc*cAc)-(cYc+cZ);
	cFn2Theta = cH1-(cQc*cQc/2*m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G]*cAc*cAc)*
			(1+2*m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G]*m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_L]/cK2*cK3)
			-cYc;
}
int FlowSTA2(int gateRowNo)
{
	m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FORMULA_TYPE] = 121; //1xx=Gate, x2x=culvert pipe, xx1=version
	m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR] = 0;
	m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CASE_SEL] = 0;
	m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE] = 0;
	m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR] = 0; // Added on 06/21/05
	if (m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_STOP_RUN] != 3)
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR] = 1;
		return -1;
	}
	if (!m_gAIColPtr)
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR] = 2;
		return -1;
	}
	go = m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][gateRowNo]];
	if (go < 0.100)
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR] = 3;
		return -1;
	}
	hw = m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_HW]];
	tw = m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_TW]];
	if (hw <= tw)
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR] = 4;
		return -1;
	}
	//Added below code to check if G and N values are present in Table 10
	if(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G] == 0)
	   m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G] = 32.2;

	if(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N] == 0)
	   m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N] = 0.024;

	cDelta = 0.01;
	cInitialTheta = 1.57; // this is PI/2 radians as initial value.
	cTheta = cInitialTheta;
	CalculateGlobals();
	if ((cC1 > 1.0)&&(cC2 > 1.0))
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CASE_SEL] = 4;
		cTheta = 2 * acos((m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]/2.0-go)/(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]/2));
		cA0 = (pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],2.0)/4.0)*(cTheta-sin(2*cTheta)/2.0);
		cAg =pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]/2.0,2.0)*((cTheta-sin(cTheta))/2.0);
		cR0 = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D] / 4.0;
		cTemp1 = 2 * m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G] * (hw - tw);
		cTemp2 = 2 * 0.85 * 0.85 * (1 + cA0 / cAg +
			((m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G] *
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N] *
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N] *
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_L]) /
			(1.49 * 1.49 * pow(cR0, 4.0/3.0))));
		cQ = 0.85 * cA0 * pow(cTemp1 / ((cA0/cAg) + cTemp2), 0.5);
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE] = cQ;
		if ((cQ >= FLT_MAX) || (cQ <= FLT_MIN))
		{
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR] = 1;
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR_CNT]++;
			return -1;
		}
		m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]] += (cQ * calcTimer);
		return 0;
	}
	else if ((cC1 < 1.5)&&(cC2 < 1.0)&&(cS0 > cSc))
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CASE_SEL] = 1;
		cTheta = cInitialTheta + cDelta;
		cFn1OldTheta = cFn1Theta;
		CalculateGlobals();
		cNewTheta = cInitialTheta - (cFn1OldTheta * cDelta)/(cFn1Theta-cFn1OldTheta+0.00001);
		cTheta = cNewTheta;
		CalculateGlobals();
		cQ = cYc / go * cQc;
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE] = cQ;
		if ((cQ >= FLT_MAX) || (cQ <= FLT_MIN))
		{
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR] = 1;
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR_CNT]++;
			return -1;
		}
		m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]] += (cQ * calcTimer);
		return 0;
	}
	else if ((cC1 < 1.5)&&(cC3 < 1.0)&&(cS0 < cSc))
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CASE_SEL] = 2;
		cTheta2 = acos(1.0-2.0*0.9*cH1/m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]);
		cTheta3 = cTheta;
		cA2 = (pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],2.0)/4.0)*(cTheta2-sin(2.0*cTheta2)/2.0);
		cA3 = (pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],2.0)/4.0)*(cTheta3-sin(2.0*cTheta3)/2.0);
		cP2 = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D] * cTheta2;
		cP3 = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D] * cTheta3;
		cR2 = cA2 / cP2;
		cR3 = cA3 / cP3;
		cK2 = (1.486 / m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N]) *
			pow(cR2, 2.0/3.0) * cA2;
		cK3 = (1.486 / m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N]) *
			pow(cR3, 2.0/3.0) * cA3;
		CalculateGlobals();
		cTheta = cInitialTheta + cDelta;
		cFn2OldTheta = cFn2Theta;
		CalculateGlobals();
		cNewTheta = cInitialTheta - (cFn2OldTheta * cDelta)/(cFn2Theta-cFn2OldTheta+0.00001);
		cTheta = cNewTheta;
		CalculateGlobals();
		cQ = cYc / go * cQc;
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE] = cQ;
		if ((cQ >= FLT_MAX) || (cQ <= FLT_MIN))
		{
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR] = 1;
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR_CNT]++;
			return -1;
		}
		m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]] += (cQ * calcTimer);
		return 0;
	}
	else if ((cC1 < 1.5)&&(cC2 <= 1.0)&&(cC3 > 1.0))
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CASE_SEL] = 3;
		cTheta2 = acos(1.0-2.0*0.9*cH1/m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]);
		cTheta3 = acos(1.0-2.0*cH4/m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]);
		cA2 = (pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],2.0)/4.0)*(cTheta2-sin(2.0*cTheta2)/2.0);
		cA3 = (pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],2.0)/4.0)*(cTheta3-sin(2.0*cTheta3)/2.0);
		cP2 = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D] * cTheta2;
		cP3 = m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D] * cTheta3;
		cR2 = cA2 / cP2;
		cR3 = cA3 / cP3;
		cK2 = (1.486 / m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N]) *
			pow(cR2, 2.0/3.0) * cA2;
		cK3 = (1.486 / m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_N]) *
			pow(cR3, 2.0/3.0) * cA3;
		cTemp1 = 2 * m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G] * (hw - tw);
		cTemp2 = (2 * m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G] * cA3 * cA3 *
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_L] * 0.94 * 0.94) / (cK2 * cK3);
		cQ = 0.94 * cA3 * pow(cTemp1 / (1 + cTemp2), 0.5);
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE] = cQ;
		if ((cQ >= FLT_MAX) || (cQ <= FLT_MIN))
		{
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR] = 1;
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR_CNT]++;
			return -1;
		}
		m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]] += (cQ * calcTimer);
		return 0;
	}
	else if ((cC1 >= 1.5)&&(cC2 <= 1.0))
	{
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CASE_SEL] = 5;
		cYc = go * 1.364 * pow((cH1-cH4)/go, 0.3607);
		cBeta = acos(1.0-2.0*cYc/m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D]);
		cQ = pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_G]*pow(m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_D],5)*
			(pow(cBeta-sin(cBeta)*cos(cBeta),3)/(64*sin(cBeta))),0.5);
		m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_DISCHARGE] = cQ;
		if ((cQ >= FLT_MAX) || (cQ <= FLT_MIN))
		{
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR] = 1;
			m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_MATH_ERR_CNT]++;
			return -1;
		}
		m_gAIColPtr[(int)m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_FLOW]] += (cQ * calcTimer);
		return 0;
	}
	m_gFlowColPtr[samplerNo][FLOW_TABLE_INDEX_CALC_ERR] = 999;
}

void CheckResetRequest()
{
	short sampler;
	for (sampler=0; sampler < noOfSamplers; sampler++)
	{
		if (!m_gFlowColPtr[sampler]) break;
		if((int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_RESET] != -1)
		{
		if (*m_gResetCounter[sampler] != m_gPrevResetCounter[sampler]) // if Reset DI was on at least once so reset the coounters
		{
			m_gDIColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DI]] = 0;
			m_gDOColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DO]] = 0;
            m_gPrevConfirmCounter[sampler] = 0;
			MOSCAD_get_datetime(&resetTime[sampler]);
			m_gPrevResetCounter[sampler] = *m_gResetCounter[sampler];
		}
       }
	}
}


void TriggerSampler(short sampler) // Added on 06/09/06 V500
{
	if (m_gAIColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_FLOW]] >= m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_TV])
	{
		MOSCAD_biton(m_gDOReqColPtr, (int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DO]);
		MOSCAD_get_datetime(&triggerTime[sampler]);
		m_gAIColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_FLOW]] = 0;
		if (LogMsgFnPtr.func_adr)
		{
			paramArr[0]=9;
			paramArr[1]=sampler+1;
			paramArr[2]=m_gDOColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DO]]+1;
			paramArr[3]=m_gDIColPtr[(int)m_gFlowColPtr[sampler][FLOW_TABLE_INDEX_DI]];
			paramArr[4]=paramArr[5]=0;
			MOSCAD_ucall_1(&LogMsgFnPtr, (unsigned char *)paramArr);
		}
	}
}

void getConfirmation()
{
	 short sampler;
	 for (sampler=0; sampler < noOfSamplers; sampler++)
	 {
		if (!m_gFlowColPtr[sampler]) break;
		if (*m_gConfirmCounter[sampler] != m_gPrevConfirmCounter[sampler]) // if Reset DI was on at least once so reset the coounters
		{
			MOSCAD_get_datetime(&confirmTime[sampler]);
			m_gPrevConfirmCounter[sampler] = *m_gConfirmCounter[sampler];
		}
	 }
}