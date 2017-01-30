/**************************************************************************************************
                                                  Revision History

    Name          Version    Date modified               Code Modified

 Suman Reddy      1.00       05/08/2007              Initial Creation

***************************************************************************************************/
/* Start Spillway4.h file */

//Variables declaration
char str[256];
CB_TABLE_INFO m_gAITable, m_gDITable, m_gDOTable;
float *m_gAIColPtr, paramArr[10];
short *m_gDIColPtr, *m_gDOColPtr, samplerNo, noOfSamplers, *m_gResetCounter[8], m_gPrevResetCounter[8],*m_gConfirmCounter[8], m_gPrevConfirmCounter[8];
unsigned char *m_gDOReqColPtr, *m_gDIStatusColPtr;
unsigned long calcTimer;
CB_FUNC LogMsgFnPtr;


//Function Prototypes
void Flow();
void MainTask();
void SWSDIDiagDetails(void *pControl, void *User);
void SWSDIDiag(void *pControl, void *User);
int FlowSTA2(int gateRowNo);
void getConfirmation();
void CheckResetRequest(); // Added on 06/09/06 V500
void TriggerSampler(short samplerNo);// Added on 06/09/06 V500



