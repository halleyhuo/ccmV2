
#include "Types.h"
#include "cfg.h"
#include <errno.h>
#include <string.h>

#include <stdio.h>
static bool ifGetCfgFile = false; 

/*
static LogCfgStruct logCfgStruct;

static FILE* OpenCfgFile(const char* const path, const char* const mode)
{    
    FILE* result = fopen( path, mode );
    return result;
}

static bool GetReferralFiles (FILE* pFile)
{    
    bool rc = false;
    fseek( pFile, 0L, SEEK_END);
    unsigned int32 iDataSize = ftell(pFile);
    fseek( pFile, 0L, SEEK_SET);
    if (iDataSize != 0)
    {
        char* pGetFile = NULL;
        pGetFile = new char[ iDataSize + 1 ];
        const unsigned int32 iBlockRead = fread(pGetFile, sizeof(char), iDataSize, pFile);

        if (iBlockRead == 0)
        {
            printf( "###error, can not open Log Cfg file\n" );
            delete[] pGetFile;
            pGetFile = NULL;            
            return false;
        }
        
        char *cTokenLine = strtok(pGetFile, "\n");
        while (cTokenLine != NULL)
        {
            if(cTokenLine[0]==';')//Comment
            {
                printf("Comment: %s\n",&cTokenLine[1]);
            }
            else
            {                
                char *cEnd;
                cEnd = strchr(cTokenLine,':');
                
                if( (cEnd!=NULL) && ((cEnd-cTokenLine)==2) )
                {                    
                    if(strncmp(cTokenLine,"PF",2)==0)
                    {                       
                        if(logCfgStruct.originPassCnt<MAX_FILTER_CNT)
                        {
                            logCfgStruct.originPassFilter[logCfgStruct.originPassCnt]=
                                new  char[iDataSize + 1];
                            strncpy(logCfgStruct.originPassFilter[logCfgStruct.originPassCnt],
                                &cEnd[1],iDataSize);
                            logCfgStruct.originPassCnt++;
                            printf("Raw %s to List, cnt %d\n",cTokenLine,
                                logCfgStruct.originPassCnt);                            
                        }                        
                    }
                }
                else
                {
                    printf( "###error, Unknown Cfg Line\n" );
                }
            }      
            
            cTokenLine = strtok(NULL, "\n");
        }
    }
    return true;
}
*/

#include <fstream>
#include <string>
#include <iostream>       // std::cout
#include <vector>

std::vector<std::string> strPFVec;

static bool ReadCfg(const char* const path)
{
    std::ifstream file(path);
    std::string str; 
  
    while (std::getline(file, str))
    {  
        ifGetCfgFile = true;
        if(std::string::npos!= str.find(";"))
        {
            std::cout<<"Comment:"<<str<<'\n';
        }
        else if(2==str.find(":"))
        {
            if(str.substr(0,2).compare(0,2,"PF")==0)
            {               
                strPFVec.push_back(str.substr(3)); 
            }
            else
            {
                std::cout<<"Unrecognized Line:"<<str<<'\n';
            }
        }
        else
        {
            std::cout<<"Unrecognized Line:"<<str<<'\n';
        }       
    }
    return true;
}


void LoadCfg()
{    
    ReadCfg("/IDC_ROOT/ETC/LogCfg.ini");
    std::cout<<"PASS Filter:"<<std::endl;
    for(int32 i = 0; i < strPFVec.size(); i++) 
    {
        std::cout << strPFVec[i] << "\n";
    }
}


bool PassFilter(char originName[])
{
    bool rc = false;
    if(!ifGetCfgFile)
    {
        rc = true;
    }
    else
    {
        for(int32 i = 0; i < strPFVec.size(); i++) 
        {            
            if(strPFVec[i].compare(0,strPFVec[i].size(),originName)==0)
            {
                rc = true;
            }
        }    
    }
    return rc;
}
