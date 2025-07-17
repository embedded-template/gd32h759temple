#include "yyln_data.h"

yyln_module_data_t yyln_moudle_data = {0};

yyln_single_module_info_t yyln_single_module_info[] = {
    {.message_type = yyln_SetDataResponse,
     .message_cmd = yyln_VoutReference,
     .byte_start = 4u,
     .byte_end = 8u,
     .bit_start = 0,
     .bit_end = 0,
     .pdata = &yyln_moudle_data.v_set[0]},
    {.message_type = yyln_SetDataResponse,
     .message_cmd = yyln_IoutLimit,
     .byte_start = 4u,
     .byte_end = 8u,
     .bit_start = 0,
     .bit_end = 0,
     .pdata = &yyln_moudle_data.i_set[0]},
    {.message_type = yyln_SetDataResponse,
     .message_cmd = yyln_GroupAddress,
     .byte_start = 4u,
     .byte_end = 8u,
     .bit_start = 0,
     .bit_end = 0,
     .pdata = &yyln_moudle_data.i_set[0]},
}