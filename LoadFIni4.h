/**************************************************************************************************
                                                  Revision History

    Name          Version    Date modified               Code Modified

 Suman Reddy      1.00       05/08/2007              Initial Creation

***************************************************************************************************/
/* Start LoadFIni4.h file */

//Variable declarations

char * _lastp; // this a trick to work strtok and probably atof also
CB_TABLE_INFO m_gFlowTable;
float *m_gFlowColPtr[8];
struct token_info
{
	char name[12];
	float val;
	char row, col;
};

struct token_info tokens[MAX_TOKENS];

//Function Prototypes

void InitStrcut();
char *StrUpr(char *str);
void CopyArray(int col);
char *RemoveSpaces(char *str);
int LoadIniFile();

/* End LoadFIni4.h file */