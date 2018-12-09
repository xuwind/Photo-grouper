#include "functions.h"
//examples
gpTestData_t* gp_Nvm_mimic_attributes();

int main()
{
    //sample
    gpTestData_t* _attrs = gp_Nvm_mimic_attributes();
    gpNvm_WriteAllAttribute(_attrs);
    free(_attrs);
    _attrs = NULL;
    return 0;
}
//create some attribute examples
gpTestData_t* gp_Nvm_mimic_attributes()
{
    gpTestData_t *_attrs = malloc(TOTAL_ATTRS*sizeof(gpTestData_t));
    for (int i=0; i<TOTAL_ATTRS; i++) _attrs[i].id=0;
    //UInt8 attribute
    _attrs[0] = (gpTestData_t){1,1,{123}};

    //UInt32 attribute
    _attrs[1] = (gpTestData_t){2,4,{0,0,0,0}};
    UInt32 _intV = 123456;
    UInt8* _intArr = _UInt32ToArray(_intV);
    memcpy(_attrs[1].data,_intArr,4);
    free(_intArr);
    //Array attribute
    _attrs[2] = (gpTestData_t){3,20,{12,23,34,54,67,89,97}};

    //string attributes
    _attrs[3] = (gpTestData_t){4,20,"embedded c test"};

    return _attrs;
}
