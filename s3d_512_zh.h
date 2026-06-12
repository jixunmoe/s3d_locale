#pragma once

#define ROTL32(x, r) ((((uint32_t)x) << r) | (((uint32_t)x) >> (32 - r)))
#define ROR13(hash, c) (ROTL32(hash, 13) + (c | 0x20))

// hash： "Simplify3D.exe" "about.exe"
#define HASH_S3D (0x0f9a570a)
#define HASH_ABOUT (0x83662042)

// pt -> zh_TW.qm, es -> zh_CN.qm
#define HASH_REPLACE_PT (0x24e97667)
#define HASH_REPLACE_ES (0x243975e7)

#define IS_HASH_SIMPLIFIED_CHINESE(hash) ((hash) & (1 << 7))
#define LOCALE_NAME_SIMPLIFIED_CHINESE  L"zh_CN"
#define LOCALE_NAME_TRADITIONAL_CHINESE L"zh_TW"

#define LOCALE_PATH_SIMPLIFIED_CHINESE  L"locale/" LOCALE_NAME_SIMPLIFIED_CHINESE  L".qm"
#define LOCALE_PATH_TRADITIONAL_CHINESE L"locale/" LOCALE_NAME_TRADITIONAL_CHINESE L".qm"
#define LOCALE_PATH_CHINESE_LEN (15)
