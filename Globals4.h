/*                                          Revision History
***************************************************************************************************
    Name                            Code Modified                                Date modified
 Suman Reddy                       Initial Creation                               08 May 2007
 Suman Reddy                       Made Max_Tokens 70 and                         13 Oct 2007
                                    added more defines
 Suman Reddy                       Made Max_Tokens 80 and                         28 Jan 2008
                                   modified couple of defines
***************************************************************************************************/

/* Start Globals.h file */

#define C_PI							(22.0/7.0)

// Table details
#define MOSCAD_FLOW_TABLE_NUMBER		10
#define MOSCAD_FLOW_TABLE_COL_NUMBER	0
#define MOSCAD_AI_TABLE_NUMBER			7  // change this to 7
#define MOSCAD_AI_TABLE_COL_NUMBER		4  // change this to 4
#define MOSCAD_DI_TABLE_NUMBER			3
#define MOSCAD_DI_TABLE_COL_NUMBER		6
#define MOSCAD_DO_TABLE_NUMBER			5
#define MOSCAD_DO_TABLE_COL_NUMBER		6
#define MOSCAD_DO_TABLE_REQ_COL_NUMBER	4
#define MOSCAD_DI_TABLE_STATUS_COL_NUM	5

// Ini file flash id
#define FLASH_ID_FLOW_INI				29

#define MAX_TOKENS						80

// Following indexes are common for Gates and Pumps
#define FLOW_TABLE_INDEX_PARAM_EXIST	0	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_TIMER			1	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_STOP_RUN		2	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_FORMULA_TYPE	3	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_MATH_ERR		4   // Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_RESET			5	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_HW				6	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_TW				7 	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DI				8 	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DO				9 	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_FLOW 			10	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DI_TM 			11	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_CALC_ERR		12	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_CASE_SEL		13	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE		14	// Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_TV 			15	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_MATH_ERR_CNT	16  // Does not exist in Ini(.Dat) file
#define FLOW_TABLE_INDEX_ID 			17	// Does not exist in Ini(.Dat) file

//Floowing Indexes are for Pumps
#define FLOW_TABLE_INDEX_NUM_PUMP		18 	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_ND 			19	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_AD 			20	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_BD				21	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_CD				22	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DD				23	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_NE 			24	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_AE 			25	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_BE				26	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_CE				27	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DE				28	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM1 			29	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM2 			30	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM3 			31	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM4 			32	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM5 			33	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM6 			34	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_RPM7 			35	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE1		36	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE2		37	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE3		38	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE4		39	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE5		40	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE6		41	// From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE7		42	// From Ini(.Dat) file


// Following indexes are for Gates
#define FLOW_TABLE_INDEX_NUM_GO			43 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GW 			44	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_G 				45	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_D 				46	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_L 				47	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_B 				48	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_KE 			49	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_N 				50	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GH 			51	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_SE 		    52	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_INLET 			53	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_OUTLET			54	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GO1			55 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GO2			56 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GO3			57 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GO4			58 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GO5			59 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_GO6			60 	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE_G1	61	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE_G2	62	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE_G3	63	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE_G4	64	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE_G5	65	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_DISCHARGE_G6	66	// 	From Ini(.Dat) file

//Following indexes for Spillways
#define FLOW_TABLE_INDEX_CSFC 		    67	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_CFFC 		    68	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_USFC 		    69	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_UFFC 		    70	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_OTFC 		    71	// 	From Ini(.Dat) file
#define FLOW_TABLE_INDEX_SW           	72	// 	From Ini(.Dat) file

#define FLOW_TABLE_INDEX_SBV 			73	// 	From Ini(.Dat) file

#define FLOW_TABLE_INDEX_TOTAL_FLOW 	75


typedef struct _time
{
  int autosamplerNo;
  MOSCAD_DATE_TM confirmTime;
  MOSCAD_DATE_TM triggerTime;
  MOSCAD_DATE_TM flowTime;
  MOSCAD_DATE_TM resetTime;

}TIMES;

//Global variables
extern CB_TABLE_INFO m_gFlowTable;
extern float *m_gFlowColPtr[8], *m_gAIColPtr;
extern short *m_gDIColPtr, *m_gDOColPtr, samplerNo, noOfSamplers, *m_gResetCounter[8], m_gPrevResetCounter[8],*m_gConfirmCounter[8], m_gPrevConfirmCounter[8];
extern unsigned char *m_gDOReqColPtr, *m_gDIStatusColPtr;



/* End Globals.h file */