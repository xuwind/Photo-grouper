#include "functions.h"


gpNvm_Result gpNvm_BackupAttributes(UInt8 direction)
{
    FILE *_fromFile,*_toFile;
    if (direction==0)
    {
        _fromFile = fopen(LIFE_FILE,"rb");
        _toFile = fopen(BACKUP_FILE,"wb");
    }
    else
    {
        _fromFile = fopen(LIFE_FILE,"rb");
        _toFile = fopen(BACKUP_FILE,"wb");
    }
    if(_fromFile==NULL)   return 1;
    if(_toFile==NULL)     return 2;
    char _c;
    while((_c=fgetc(_fromFile))!=EOF)
          fputc(_c,_toFile);
    fclose(_fromFile);
    fclose(_toFile);
    return 0;
}

gpTestData_t* gpNvm_ReadFromFile(char* fileName)
{
    gpTestData_t* _gpNvm_Attrs=malloc(TOTAL_ATTRS*sizeof(gpTestData_t));
    for (int i=0; i<TOTAL_ATTRS; i++) _gpNvm_Attrs[i].id=0;
    FILE* _gpNvm_file = fopen(fileName,"r");
    if (!_gpNvm_file){
        printf("Cannot open file '%s'", fileName);
        return NULL;
    }
    fseek(_gpNvm_file,0,SEEK_SET);
    UInt8 _indx=0;
    while (fread(&(_gpNvm_Attrs[_indx].id),1,1,_gpNvm_file) == 1)
    {
        fread(&(_gpNvm_Attrs[_indx].length),1,1,_gpNvm_file);
        fread(&(_gpNvm_Attrs[_indx].data),MAX_LENGTH,1,_gpNvm_file);
        UInt8* _valData = malloc(MAX_LENGTH*sizeof(UInt8));
        fread(_valData,MAX_LENGTH,1,_gpNvm_file);
        //validation if two data values are same
        if(memcmp(_valData,_gpNvm_Attrs[_indx].data,MAX_LENGTH)!=0)
        {
            free(_valData);
            free(_gpNvm_Attrs);
            return NULL;
        }
        else free(_valData);
        _indx++;
    }
    fclose(_gpNvm_file);
    return _gpNvm_Attrs;
}

UInt8* _UInt32ToArray(UInt32 iValue)
{
    UInt8* _aValue = malloc(4*sizeof(UInt8));
    memcpy(_aValue,&iValue,sizeof(UInt32));
    return _aValue;
}

UInt32* _ArrayToUInt32(UInt8* aValue)
{
    UInt32 *_iValue=malloc(sizeof(UInt8));
    memcpy(_iValue,aValue,4*sizeof(UInt8));
    return _iValue;
}

gpNvm_Result gpNvm_GetAttribute(gpNvm_AttrId attrId,UInt8* pLength,UInt8* pValue)
{
    gpTestData_t* _gpNvm_Attrs = gpNvm_ReadAllAttribute();
    if (!_gpNvm_Attrs) return 1;
    for (int i=0; i<TOTAL_ATTRS; i++)
    {
        if (_gpNvm_Attrs[i].id==attrId)
        {
            *pLength = _gpNvm_Attrs[i].length;
            memcpy(pValue,_gpNvm_Attrs[i].data, MAX_LENGTH);
            free(_gpNvm_Attrs);
            _gpNvm_Attrs =NULL;
            return 0;
        }
    }
    return 1;
}
//return value 101: both life and backup corrupted
gpTestData_t* gpNvm_ReadAllAttribute()
{
    gpTestData_t* _gpNvm_Attrs = gpNvm_ReadFromFile(LIFE_FILE);
    gpTestData_t* _gpNvm_bk =gpNvm_ReadFromFile(BACKUP_FILE);
    if (_gpNvm_Attrs==NULL && _gpNvm_bk==NULL)return NULL;
    if (_gpNvm_Attrs!=NULL && _gpNvm_bk!=NULL) free(_gpNvm_bk);
    //recover backup file
    if (_gpNvm_Attrs!=NULL && _gpNvm_bk==NULL)
    {
        gpNvm_BackupAttributes(0);
        printf("Life file '%s' was corrupted and has been recovered from backup file '%s' successfully!", LIFE_FILE,BACKUP_FILE);
    }
    //recover life file
    if (_gpNvm_Attrs==NULL && _gpNvm_bk!=NULL)
    {
        gpNvm_BackupAttributes(1);
        printf("Backup file '%s' was corrupted and has been recovered from Life file '%s' successfully!",BACKUP_FILE, LIFE_FILE);
        _gpNvm_Attrs=_gpNvm_bk;
    }
    return _gpNvm_Attrs;
}

gpNvm_Result gpNvm_SetAttribute(gpNvm_AttrId attrId,UInt8 length,UInt8* pValue)
{
    gpTestData_t* _gpNvm_Attrs=gpNvm_ReadAllAttribute();
    if (!_gpNvm_Attrs)
    {
        _gpNvm_Attrs=malloc(TOTAL_ATTRS*sizeof(gpTestData_t));
        for (int i=0; i<TOTAL_ATTRS; i++) _gpNvm_Attrs[i].id=0;
    }
    UInt8 _indx=255;
    for (int i=0; i<TOTAL_ATTRS; i++)
    {
        if (_gpNvm_Attrs[i].id==attrId)
        {
            _indx = i;
            _gpNvm_Attrs[i].length=length;
            memcpy(_gpNvm_Attrs[i].data,pValue,MAX_LENGTH);
            break;
        }
    }
    if (_indx == 255)
    {
        for (int i=0; i<TOTAL_ATTRS; i++)
        {
            if (_gpNvm_Attrs[i].id==0)
            {
                _gpNvm_Attrs[i].id=attrId;
                _gpNvm_Attrs[i].length=length;
                memcpy(_gpNvm_Attrs[i].data,pValue,MAX_LENGTH);
                break;
            }
        }
    }
    gpNvm_WriteAllAttribute(_gpNvm_Attrs);
    free(_gpNvm_Attrs);
    return 0;
}
//write attributes to file
gpNvm_Result gpNvm_WriteAllAttribute(gpTestData_t* gpNvm_Attrs)
{
    FILE* _gpNvm_life = fopen(LIFE_FILE,"wb");
    if (_gpNvm_life == NULL){
        printf("Cannot open the file %s...\n",LIFE_FILE);
        return 0;
    }
    for (int i=0; i< TOTAL_ATTRS; i++)
    {
        if (gpNvm_Attrs[i].id ==0) continue;
        fwrite(&(gpNvm_Attrs[i].id),1,1,_gpNvm_life);
        fwrite(&(gpNvm_Attrs[i].length),1,1,_gpNvm_life);
        fwrite(&(gpNvm_Attrs[i].data),MAX_LENGTH,1,_gpNvm_life);
        fwrite(&(gpNvm_Attrs[i].data),MAX_LENGTH,1,_gpNvm_life);
    }
    fclose(_gpNvm_life);
    gpTestData_t* _attrs_check = gpNvm_ReadFromFile(LIFE_FILE);
    if (_attrs_check != NULL)
    {
        free(_attrs_check);
        _attrs_check = NULL;
        gpNvm_BackupAttributes(0);
        printf("attributes have been saved to both '%s' and have been backed up to '%s' successfully!\n", LIFE_FILE, BACKUP_FILE);
    }
    else
    {
        printf("write to file '%s' failed!\n", LIFE_FILE);
    }
    return 0;
}



