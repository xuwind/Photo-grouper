#ifdef assert
#undef assert
#endif
#define assert(expr)  (test_result = test_result && (expr))
#define TEST_FILE "test_result.txt"

#include "functions.h"
static int test_result;
static FILE* fileResult;
gpNvm_Result test_gpNvm_GetAttribute();
gpNvm_Result test_gpNvm_SetAttribute();

int main()
{
    fileResult = fopen(TEST_FILE,"a"); //open test result file
    if (!fileResult)
    {
        printf("cannot open/create test result file! '%s'", TEST_FILE);
        return 1;
    }
    //start test
    time_t now = time(NULL);
    char* timeStr = ctime(&now);
    //initialize 4 attributes
    fprintf(fileResult,"Start test at: %s\n",timeStr);
    //test setAttributes
    test_gpNvm_SetAttribute();
    //test getAttributes
    test_gpNvm_GetAttribute();
    //end test
    now = time(NULL);
    timeStr = ctime(&now);
    fprintf(fileResult,"\nEnd test at: %s\n\n",timeStr);
    fclose(fileResult);
    return 0;
}
//Unit test for SetAttribute, the attribute will be read at getAttribute
gpNvm_Result test_gpNvm_SetAttribute()
{
    fprintf(fileResult,"\tTest Unit: gpNvm_SetAttribute()\n\n");
    test_result=1;
    //set UInt8 value
    gpNvm_AttrId _id = 1;
    UInt8 _length=1, _int8Value=211, _arrValue[20] = {_int8Value};
    gpNvm_Result _setOkay=gpNvm_SetAttribute(_id,_length,_arrValue);
    assert(_setOkay==0);
    if (test_result)
        fprintf(fileResult,"\t\tset UINT8 value successfully: id=%d, length=%d, value=%d\n",_id,_length,_int8Value);
    else
        fprintf(fileResult,"\t\tset UINT8 value failed! : id=%d, length=%d, value=%d\n",_id,_length,_int8Value);

    //set UInt32 value
    _id = 2, _length=4;
    UInt32 _int32Value=1211211;
    UInt8* _arrInt32 = _UInt32ToArray(_int32Value);
    _setOkay=gpNvm_SetAttribute(_id,_length,_arrInt32);
    free(_arrInt32);
    assert(_setOkay==0);
    if (test_result)
        fprintf(fileResult,"\t\tset UINT32 value successfully: id=%d, length=%d, value=%d\n",_id,_length,_int32Value);
    else
        fprintf(fileResult,"\t\tset UINT32 value failed! : id=%d, length=%d, value=%d\n",_id,_length,_int32Value);

    //set Array Value
    _id = 3, _length=20;
    UInt8 _arr[20] = {12,21,42,54,65};
    _setOkay=gpNvm_SetAttribute(_id,_length,_arr);
    assert(_setOkay==0);
    if (test_result)
        fprintf(fileResult,"\t\tset String/Array value successfully: id=%d, length=%d, value={12,21,42,54,65}\n",_id,_length);
    else
        fprintf(fileResult,"\t\tset String/Array value failed! : id=%d, length=%d, value={12,21,42,54,65}\n",_id,_length);
    return 0;
    //set Array Value
}

//Unit test for GetAttribute, using the same attributes created at SetAttribute test
gpNvm_Result test_gpNvm_GetAttribute()
{
    test_result=1;
    fprintf(fileResult,"\n\tTest Unit: gpNvm_GetAttribute()\n\n");
    //test get UInt8 attribute with id=1
    gpNvm_AttrId  _id=1;
    UInt8 _length=0, *_arrValue=malloc(MAX_LENGTH*sizeof(UInt8)), _int8Value;
    gpNvm_Result _getOkay=gpNvm_GetAttribute(_id,&_length,_arrValue);
    assert(_getOkay==0);
    _int8Value = _arrValue[0];
    if (test_result)
        fprintf(fileResult,"\t\tget UINT8 value successfully: id=%d, length=%d, value=%d\n",_id,_length,_int8Value);
    else
        fprintf(fileResult,"\t\tget UINT8 value failed! : id=%d, length=%d, value=%d\n",_id,_length,_int8Value);
    _id=2, _length=0;
    _getOkay=gpNvm_GetAttribute(_id,&_length,_arrValue);
    UInt32* _int32Value;
    _int32Value = _ArrayToUInt32(_arrValue);
    assert(_getOkay==0);
    if (test_result)
        fprintf(fileResult,"\t\tget UINT32 value successfully: id=%d, length=%d, value=%d\n",_id,_length,*_int32Value);
    else
        fprintf(fileResult,"\t\tget UINT32 value failed! : id=%d, length=%d, value=%d\n",_id,_length,_int32Value);

    _id=3, _length=0;
    _getOkay=gpNvm_GetAttribute(_id,&_length,_arrValue);
    assert(_getOkay==0);
    if (test_result)
        fprintf(fileResult,"\t\tget String/Array value successfully: id=%d, length=%d, value={12,21,42,54,65}\n",_id,_length);
    else
        fprintf(fileResult,"\t\tget String/Array value failed! : id=%d, length=%d, value={12,21,42,54,65}\n",_id,_length);
    return 0;
}


