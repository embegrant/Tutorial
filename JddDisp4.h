
/**************************************************************************************************
                                                  Revision History

    Name          Version    Date modified               Code Modified

 Suman Reddy      1.00       05/08/2007              Initial Creation

***************************************************************************************************/
/* Start JddDisp4.h file */

// Macros
#define BUFF_SIZE       		128
// Port definition
#define I2PHYS_PORT				31    // computer 1
// Table number
#define MOSCAD_TABLE_NUMBER      0
#define MOSCAD_COL_FOR_SNAME     1
#define MOSCAD_ROW_FOR_SNAME     0

// Variables declaration
unsigned char *p_i2physKey;
unsigned char *p_linkKey;
unsigned char buff[BUFF_SIZE];
char screenStrs[2][100], siteName[10];
CB_TABLE_INFO m_gValTable;
int m_gGotTable;
MOSCAD_DATE_TM dateTime;
short *colPtr;

//Function Prototypes
void dummy_func();
void GetSiteName();
void GetFirstRow();
void GetSecondRow();
void SendBytes(unsigned char *buff);

/* End JddDisp4.h file */