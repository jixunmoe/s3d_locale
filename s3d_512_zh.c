#include "s3d_512_zh.h"

#include <stdint.h>
#include <stdio.h>

#include <Windows.h>

#include <ntdef.h>
#include <wchar.h>
#include <winternl.h>

typedef void *(__fastcall *QTranslatorLoadTranslation_t)(void *p1, void *p2,
                                                         void *p3);
typedef void *(__fastcall *QTStringFromFixedAddress_t)(void *p1, void *p2,
                                                       void *p3);

#define HOOK_SIZE (0x10)
#define EXE_PATH_MAX (0x400)

typedef struct unicode_span_t {
  wchar_t *ptr;
  size_t len;
} unicode_span_t;

typedef struct global_app_data {
  QTranslatorLoadTranslation_t pfnQTranslatorLoadTranslation;

  unicode_span_t exe_dir;

  void *hook_point;
  uint8_t backup_data[HOOK_SIZE];
  int replace_count;
} global_app_data_t;

global_app_data_t g_app_data;

struct QtStringWide {
  uint32_t ref_count;
  uint32_t str_len;
  uint32_t cap_lo32;
  uint32_t cap_hi32;
  uint64_t flags;
  wchar_t data[0];
};

static void UninstallPatch() {
  WriteProcessMemory((HANDLE)-1, g_app_data.hook_point, g_app_data.backup_data,
                     HOOK_SIZE, NULL);
  memset(&g_app_data, 0, sizeof(g_app_data));
}

void *__fastcall __attribute__((naked, aligned(8)))
returnToQTranslatorLoadTranslation(void *p1, void *p2,
                                   struct QtStringWide **p3) {
  asm("push %rbp; push %r15; push %r14; push %r13; push %r12; push %rsi; push "
      "%rdi; push %rbx; subq $0x58, %rsp");
  asm("jmp *%0" : : "m"(g_app_data.pfnQTranslatorLoadTranslation));
}

void *__fastcall MyHookedQTranslatorLoadTranslation(void *p1, void *p2,
                                                    struct QtStringWide **p3) {
  struct QtStringWide *p3_str = *p3;
  if (p3_str->str_len == 0x36) {
    uint32_t hash = 0;
    for (int i = 0; i < 0x36; i++)
      hash = ROR13(hash, p3_str->data[i]);

    const wchar_t *suffix = IS_HASH_SIMPLIFIED_CHINESE(hash)
                                ? LOCALE_PATH_SIMPLIFIED_CHINESE
                                : LOCALE_PATH_TRADITIONAL_CHINESE;
    if (hash == HASH_REPLACE_ES || hash == HASH_REPLACE_PT) {
      size_t cap = (g_app_data.exe_dir.len + LOCALE_PATH_CHINESE_LEN + 1) * 2;
      struct QtStringWide *buffer =
          (struct QtStringWide *)malloc(sizeof(struct QtStringWide) + cap);
      if (buffer) {
        buffer->ref_count = 2;
        buffer->flags = 0x18; // no idea what this is
        buffer->str_len =
            (uint32_t)(g_app_data.exe_dir.len + LOCALE_PATH_CHINESE_LEN);
        buffer->cap_lo32 = cap;
        buffer->cap_hi32 = 0;
        wcsncpy(buffer->data, g_app_data.exe_dir.ptr, g_app_data.exe_dir.len);
        wcsncpy(&buffer->data[g_app_data.exe_dir.len], suffix,
                LOCALE_PATH_CHINESE_LEN);
        struct QtStringWide *temp = buffer;
        void *result = returnToQTranslatorLoadTranslation(p1, p2, &temp);
        free(buffer);

        if (++g_app_data.replace_count == 2) {
          UninstallPatch();
        }
        return result;
      }
    }
  }
  return returnToQTranslatorLoadTranslation(p1, p2, p3);
}

static void decode_string(uint8_t *buffer, size_t len, uint32_t key) {
  for (int i = 0; i < len; i += 4)
    *(uint32_t *)&buffer[i] ^= key;
}

static void PatchTarget(void) {
  wchar_t *exe_path = calloc(EXE_PATH_MAX, sizeof(wchar_t));
  HANDLE exe_module = GetModuleHandleA(NULL);
  GetModuleFileNameW(exe_module, exe_path, EXE_PATH_MAX - 1);
  size_t exe_path_len = wcslen(exe_path);

  const wchar_t *p_module_name = &exe_path[exe_path_len];
  uint32_t hash = 0;
  while (p_module_name[-1] != L'\\')
    hash = ROR13(hash, *--p_module_name);

  switch (hash) {
  case HASH_ABOUT: {
    SetProcessDPIAware();
    MessageBoxW(NULL,
                L"Simplify3D v5.1.2 本地化外挂模组 (v0.4)\r\n"
                L"模组开发: Jixun (jixun.uk)\r\n"
                L"资源提供: 真夜",
                L"** 关于 **", MB_ICONINFORMATION);
    ExitProcess(1);
  }

  case HASH_S3D: {
    memset(&g_app_data, 0, sizeof(g_app_data));

    g_app_data.exe_dir.ptr = exe_path;
    g_app_data.exe_dir.len = p_module_name - exe_path;

    uint8_t *base = exe_module;
    // call +0x12B310
    uint8_t *hook_point = base + 0x12B310;
    g_app_data.hook_point = hook_point;
    g_app_data.pfnQTranslatorLoadTranslation =
        (QTranslatorLoadTranslation_t)(hook_point + HOOK_SIZE);
    memcpy(g_app_data.backup_data, hook_point, HOOK_SIZE);

    uint8_t payload[HOOK_SIZE] = {0xff, 0x25}; // jmp qword ptr [rip + 0]
    *(void **)(payload + 6) = (void *)MyHookedQTranslatorLoadTranslation;
    WriteProcessMemory((HANDLE)-1, hook_point, &payload, HOOK_SIZE, NULL);
    break;
  }

  default:
    break;
  }
}

BOOL WINAPI DllMainCRTStartup(HINSTANCE hinstDLL, DWORD fdwReason,
                              LPVOID lpvReserved) {
  ((void)lpvReserved);

  if (fdwReason == DLL_PROCESS_ATTACH) {
    PatchTarget();
  }
  return TRUE;
}
