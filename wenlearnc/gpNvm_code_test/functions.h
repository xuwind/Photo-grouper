#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef FUNCTIONS_H_INCLUDED
#define FUNCTIONS_H_INCLUDED
#define MAX_LENGTH 20
#define TOTAL_ATTRS 10
#define LIFE_FILE "gpNvm_Life.bin"
#define BACKUP_FILE "gpNvm_Backup.bin"

typedef unsigned char UInt8;
typedef unsigned long UInt32;
typedef UInt8 gpNvm_AttrId;
typedef UInt8 gpNvm_Result;

typedef struct
{
    UInt8 id;               //attribute ID
    UInt8 length;           //data length in UInt8
    UInt8 data[MAX_LENGTH]; //raw data
} gpTestData_t;

gpNvm_Result gpNvm_BackupAttributes(UInt8 direction);
UInt32* _ArrayToUInt32(UInt8* aValue);
UInt8* _UInt32ToArray(UInt32 iValue);
/*  check if LIFE_FILE or BACKUP_FILE corruption
*   data value for each attribute saved twice in both LIFE_FILE and BACKUP_File,
*   if two values are same, then no corruption, otherwise corruption happened...*/
gpTestData_t* gpNvm_ReadFromFile(char* fileName);
gpNvm_Result gpNvm_GetAttribute(gpNvm_AttrId attrId,UInt8* pLength,UInt8* pValue);
gpTestData_t* gpNvm_ReadAllAttribute();
gpNvm_Result gpNvm_RecoverFromCorruption();
/*attribute stored in the binay file as: [ID][options][length][data]*/
gpNvm_Result gpNvm_SetAttribute(gpNvm_AttrId attrId,UInt8 length,UInt8* pValue);
gpNvm_Result gpNvm_WriteAllAttribute(gpTestData_t* gpNvm_Attrs);
#endif // FUNCTIONS_H_INCLUDED
