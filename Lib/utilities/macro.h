#pragma once

#define ARRAYNUM(arr_nanme) (uint32_t) (sizeof(arr_nanme) / sizeof(*(arr_nanme)))

// 字节序转换宏 (Big Endian -> Little Endian)
#define CONVERT_ENDIANNESS(src_data, dst_data, bytes)                                                                                                          \
    do                                                                                                                                                         \
    {                                                                                                                                                          \
        if ((src_data) && (dst_data) && (bytes) > 0)                                                                                                           \
        {                                                                                                                                                      \
            for (uint8_t i = 0; i < (bytes); i++)                                                                                                              \
            {                                                                                                                                                  \
                (dst_data)[i] = (src_data)[(bytes) - 1 - i];                                                                                                   \
            }                                                                                                                                                  \
        }                                                                                                                                                      \
    } while (0)
