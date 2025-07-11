#include "yfy_data.h"

int main()
{
    uint8_t data[8] = {1,2,3,4,5,6,7,8};
    yfy_data_parse(0, 0x03, 0, data);
    
}