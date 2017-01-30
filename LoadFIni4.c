/**************************************************************************************************
                                                  Revision History

    Name          Version    Date modified               Code Modified

 Jayesh Patel     1.00                       Initial Creation

 Jayesh Patel     2.00        06/22/2005     Modified to work with more than one sampler

 Ron/Suman        3.00        04/11/2007     Modified LoadIniFile() to select # of sampler based on
                                             [FLOW] or [FLOWx] from .dat file

 Suman Reddy      4.00        05/08/2007     Updated InitStruct() based on new header file Globals4.h

 Suman Reddy      5.00        10/14/2007     Updated InitStruct() to accommodate newly added defines
                                             including SBV
***************************************************************************************************/

#define  CAPPLIC_MOS_ML
#include "c:\mctk610\include\Capplic.h"
#include "string.h"
#include "stdlib.h"
#include "Globals4.h"
#include "LoadFIni4.h"



int LoadIniFile()
{
	int i, tokenFound, samplerNo;
	char *file, *str, *chPtr, temp[256];
	unsigned char *pBuffer;
	unsigned long ulFileLength;

	InitStrcut();
	if (MOSCAD_get_flash_block(FLASH_ID_FLOW_INI, (unsigned char**)&pBuffer, &ulFileLength) != 0)
	{
		MOSCAD_error("Error: Could not find Flow Ini file");
		return 1;
	}

	file = MOSCAD_malloc(ulFileLength+1);
	if (!file)
	{
		MOSCAD_error("Error: Could not allocate memory to load Flow Ini file");
		return 2;
	}
	memcpy(file, pBuffer, ulFileLength);
	file[ulFileLength] = 0;
	tokenFound = 0;
	samplerNo = 0;
	str = strtok(file, "\r\n");
	if (MOSCAD_get_table_info(MOSCAD_FLOW_TABLE_NUMBER, &m_gFlowTable))
	{
		MOSCAD_error("Error: Could not get the flow table details");
		return 3;
	}
	for (i=0; i<m_gFlowTable.NumOfCols; i++)
		m_gFlowColPtr[i] = (float*)(m_gFlowTable.ColDataPtr[MOSCAD_FLOW_TABLE_COL_NUMBER+i]);
	for(;str;str = strtok(NULL, "\r\n"))
	{
		str = RemoveSpaces(str);
		if (!strlen(str)) continue;
		if ((str[0] == 13)||(str[0] == 10)) continue;
		if (str[0] == ';') continue;
		chPtr = strchr(str, ';');
		if (chPtr) *chPtr = 0;
		chPtr = strchr(str, '[');
		if (chPtr)
		{
			StrUpr(chPtr+1);
			if (tokenFound) CopyArray(samplerNo);
			if (!strncmp(chPtr+1, "FLOW", 4))
			{

				//Added below statement for No of Samplers-Suman
				if(chPtr[5] != ']')
				samplerNo = atoi(&chPtr[5]);
				else
				samplerNo = 1;
				noOfSamplers = samplerNo;
				tokenFound = 1;
				continue;
			}
			else continue;
		}
		else if (!tokenFound) continue;
		chPtr = strchr(str, '=');

		if ((!chPtr)&&(strncmp(str, "NUM_MAPS", 8)))
		{
			MOSCAD_sprintf(temp, "Error: Invalid line found without \'=\' sign\n %s", str);
			MOSCAD_error(temp);
			continue;
		}
		*chPtr = 0;
		for (i=0; i<MAX_TOKENS; i++)
		{
			StrUpr(str);
			if (!strcmp(tokens[i].name, str))
			{
				tokens[tokens[i].row].val = (float)atof(chPtr+1);
				tokens[tokens[i].row].col = 0;
				break;
			}
		}
	}
	MOSCAD_free(file);
	CopyArray(samplerNo);
	return 0;
}
void CopyArray(int samplerNo)
{
	int i;
	char temp[256];
	samplerNo--;
	if ((samplerNo >= 0)&&(samplerNo < m_gFlowTable.NumOfCols))
	{
		for (i=0; i<MAX_TOKENS; i++)
		{
			m_gFlowColPtr[samplerNo][i] = tokens[i].val;
			tokens[i].val = 0;
		}
	}
	else MOSCAD_error("Error: One of the sampler number is out of range in Ini file.");
}
void InitStrcut()
{


    strcpy(tokens[0].name, "HW");
	tokens[0].row = FLOW_TABLE_INDEX_HW;
	strcpy(tokens[1].name, "TW");
	tokens[1].row = FLOW_TABLE_INDEX_TW;
	strcpy(tokens[2].name, "NUM_GO");
	tokens[2].row = FLOW_TABLE_INDEX_NUM_GO;
	strcpy(tokens[3].name, "DI");
	tokens[3].row = FLOW_TABLE_INDEX_DI;
	strcpy(tokens[4].name, "DO");
	tokens[4].row = FLOW_TABLE_INDEX_DO;
	strcpy(tokens[5].name, "FLOW");
	tokens[5].row = FLOW_TABLE_INDEX_FLOW;
	strcpy(tokens[6].name, "DI-TM");
	tokens[6].row = FLOW_TABLE_INDEX_DI_TM;
	strcpy(tokens[7].name, "ID");
	tokens[7].row = FLOW_TABLE_INDEX_ID;
	strcpy(tokens[8].name, "GW");
	tokens[8].row = FLOW_TABLE_INDEX_GW;
	strcpy(tokens[9].name, "G");
	tokens[9].row = FLOW_TABLE_INDEX_G;
	strcpy(tokens[10].name, "D");
	tokens[10].row = FLOW_TABLE_INDEX_D;
	strcpy(tokens[11].name, "L");
	tokens[11].row = FLOW_TABLE_INDEX_L;
	strcpy(tokens[12].name, "B");
	tokens[12].row = FLOW_TABLE_INDEX_B;
	strcpy(tokens[13].name, "KE");
	tokens[13].row = FLOW_TABLE_INDEX_KE;
	strcpy(tokens[14].name, "N");
	tokens[14].row = FLOW_TABLE_INDEX_N;
	strcpy(tokens[15].name, "GH");
	tokens[15].row = FLOW_TABLE_INDEX_GH;
	strcpy(tokens[16].name, "TV");
	tokens[16].row = FLOW_TABLE_INDEX_TV;
	strcpy(tokens[17].name, "SE");
	tokens[17].row = FLOW_TABLE_INDEX_SE;
	strcpy(tokens[18].name, "INLET");
	tokens[18].row = FLOW_TABLE_INDEX_INLET;
	strcpy(tokens[19].name, "OUTLET");
	tokens[19].row = FLOW_TABLE_INDEX_OUTLET;
	strcpy(tokens[20].name, "TIMER");
	tokens[20].row = FLOW_TABLE_INDEX_TIMER;
	strcpy(tokens[21].name, "GO1");
	tokens[21].row = FLOW_TABLE_INDEX_GO1;
	strcpy(tokens[22].name, "GO2");
	tokens[22].row = FLOW_TABLE_INDEX_GO2;
	strcpy(tokens[23].name, "GO3");
	tokens[23].row = FLOW_TABLE_INDEX_GO3;
	strcpy(tokens[24].name, "GO4");
	tokens[24].row = FLOW_TABLE_INDEX_GO4;
	strcpy(tokens[25].name, "SBV");
	tokens[25].row = FLOW_TABLE_INDEX_SBV;
	strcpy(tokens[26].name, "RESET");
	tokens[26].row = FLOW_TABLE_INDEX_RESET;
    /*strcpy(tokens[0].name, "TIMER");
    tokens[0].row = FLOW_TABLE_INDEX_TIMER;
    strcpy(tokens[1].name, "RESET");
    tokens[1].row = FLOW_TABLE_INDEX_RESET;
    strcpy(tokens[2].name, "HW");
	tokens[2].row = FLOW_TABLE_INDEX_HW;
	strcpy(tokens[3].name, "TW");
	tokens[3].row = FLOW_TABLE_INDEX_TW;
	strcpy(tokens[4].name, "DI");
	tokens[4].row = FLOW_TABLE_INDEX_DI;
	strcpy(tokens[5].name, "DO");
	tokens[5].row = FLOW_TABLE_INDEX_DO;
	strcpy(tokens[6].name, "FLOW");
	tokens[6].row = FLOW_TABLE_INDEX_FLOW;
	strcpy(tokens[7].name, "DI-TM");
	tokens[7].row = FLOW_TABLE_INDEX_DI_TM;
	strcpy(tokens[8].name, "TV");
    tokens[8].row = FLOW_TABLE_INDEX_TV;
	strcpy(tokens[9].name, "ID");
	tokens[9].row = FLOW_TABLE_INDEX_ID;

	//For Pumps
	strcpy(tokens[10].name, "NUM_PUMP");
	tokens[10].row = FLOW_TABLE_INDEX_NUM_PUMP;
	strcpy(tokens[11].name, "ND");
	tokens[11].row = FLOW_TABLE_INDEX_ND;
	strcpy(tokens[12].name, "AD");
	tokens[12].row = FLOW_TABLE_INDEX_AD;
	strcpy(tokens[13].name, "BD");
	tokens[13].row = FLOW_TABLE_INDEX_BD;
	strcpy(tokens[14].name, "CD");
	tokens[14].row = FLOW_TABLE_INDEX_CD;
	strcpy(tokens[15].name, "DD");
	tokens[15].row = FLOW_TABLE_INDEX_DD;
	strcpy(tokens[16].name, "NE");
	tokens[16].row = FLOW_TABLE_INDEX_NE;
	strcpy(tokens[17].name, "AE");
	tokens[17].row = FLOW_TABLE_INDEX_AE;
	strcpy(tokens[18].name, "BE");
	tokens[18].row = FLOW_TABLE_INDEX_BE;
	strcpy(tokens[19].name, "CE");
	tokens[19].row = FLOW_TABLE_INDEX_CE;
	strcpy(tokens[20].name, "DE");
	tokens[20].row = FLOW_TABLE_INDEX_DE;
	strcpy(tokens[21].name, "RPM1");
	tokens[21].row = FLOW_TABLE_INDEX_RPM1;
	strcpy(tokens[22].name, "RPM2");
	tokens[22].row = FLOW_TABLE_INDEX_RPM2;
	strcpy(tokens[23].name, "RPM3");
	tokens[23].row = FLOW_TABLE_INDEX_RPM3;
	strcpy(tokens[24].name, "RPM4");
	tokens[24].row = FLOW_TABLE_INDEX_RPM4;
	strcpy(tokens[25].name, "RPM5");
	tokens[25].row = FLOW_TABLE_INDEX_RPM5;
	strcpy(tokens[26].name, "RPM6");
	tokens[26].row = FLOW_TABLE_INDEX_RPM6;
	strcpy(tokens[27].name, "RPM7");
	tokens[27].row = FLOW_TABLE_INDEX_RPM7;



	//Added below for Gate calculation- Suman
	strcpy(tokens[28].name, "NUM_GO");
	tokens[28].row = FLOW_TABLE_INDEX_NUM_GO;
	strcpy(tokens[29].name, "GW");
	tokens[29].row = FLOW_TABLE_INDEX_GW;
	strcpy(tokens[30].name, "G");
	tokens[30].row = FLOW_TABLE_INDEX_G;
	strcpy(tokens[31].name, "D");
	tokens[31].row = FLOW_TABLE_INDEX_D;
	strcpy(tokens[32].name, "L");
	tokens[32].row = FLOW_TABLE_INDEX_L;
	strcpy(tokens[33].name, "B");
	tokens[33].row = FLOW_TABLE_INDEX_B;
	strcpy(tokens[34].name, "KE");
	tokens[34].row = FLOW_TABLE_INDEX_KE;
	strcpy(tokens[35].name, "N");
	tokens[35].row = FLOW_TABLE_INDEX_N;
	strcpy(tokens[36].name, "GH");
	tokens[36].row = FLOW_TABLE_INDEX_GH;
	strcpy(tokens[37].name, "SE");
	tokens[37].row = FLOW_TABLE_INDEX_SE;
	strcpy(tokens[38].name, "INLET");
	tokens[38].row = FLOW_TABLE_INDEX_INLET;
	strcpy(tokens[39].name, "OUTLET");
	tokens[39].row = FLOW_TABLE_INDEX_OUTLET;
    strcpy(tokens[40].name, "GO1");
	tokens[40].row = FLOW_TABLE_INDEX_GO1;
	strcpy(tokens[41].name, "GO2");
	tokens[41].row = FLOW_TABLE_INDEX_GO2;
	strcpy(tokens[42].name, "GO3");
	tokens[42].row = FLOW_TABLE_INDEX_GO3;
	strcpy(tokens[43].name, "GO4");
	tokens[43].row = FLOW_TABLE_INDEX_GO4;

	//Added below for spillway calculation
	strcpy(tokens[44].name, "CSFC");
	tokens[44].row = FLOW_TABLE_INDEX_CSFC;
	strcpy(tokens[45].name, "CFFC");
	tokens[45].row = FLOW_TABLE_INDEX_CFFC;
	strcpy(tokens[46].name, "USFC");
	tokens[46].row = FLOW_TABLE_INDEX_USFC;
	strcpy(tokens[47].name, "UFFC");
	tokens[47].row = FLOW_TABLE_INDEX_UFFC;
	strcpy(tokens[48].name, "OTFC");
	tokens[48].row = FLOW_TABLE_INDEX_OTFC;

    strcpy(tokens[49].name, "SBV");
	tokens[49].row = FLOW_TABLE_INDEX_SBV;*/

}
char *StrUpr(char *str)
{
	int i,len;
	len=strlen(str);
	for (i=0;i<len;i++)
		if ((str[i]>='a')&&(str[i]<='z')) str[i] -= 32;
	return str;
}
char *RemoveSpaces(char *str)
{
	int i,len,index=0;
	len=strlen(str);
	for (i=0;i<len;i++)
		if (str[i] == 32) index=i+1;  // modifed on 06/02/05 was index=i;
		else break;
	return &str[index];
}
