#include <windows.h>

#ifdef __clang__
size_t getpagesize() {
  SYSTEM_INFO S;
  GetNativeSystemInfo(&S);
  return S.dwPageSize;
}
#endif