#include <klib.h>
#include <klib-macros.h>
#include <stdint.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

size_t strlen(const char *s) {
  assert(s);
  size_t cnt = 0;
  while(*s++) cnt++;
  return cnt;
  // panic("Not implemented");
}

char *strcpy(char *dst, const char *src) {
  char* p = NULL;
  assert(dst && src);
  p = dst;
  while((*dst++ = *src++) != '\0');
  return p;
  // panic("Not implemented");
}

char *strncpy(char *dst, const char *src, size_t n) {
  char* tmp = dst;
  while(n){
    if((*tmp = *src) != 0) src++;
    tmp++;
    n--;
  }
  return dst;
  // panic("Not implemented");
}

char *strcat(char *dst, const char *src) {
  assert(dst && src);
  char* p = dst;
  while(*p != '\0') p++;
  while(*src != '\0'){
    *p = *src;
    p++;
    src++;
  }
  p = '\0';
  return dst;
  // panic("Not implemented");
}

int strcmp(const char *s1, const char *s2) {
  assert(s1 && s2);
  while(*s1 == *s2 && *s1 && *s2){
    s1++;
    s2++;
  }
  return (*s1 - *s2);
  // panic("Not implemented");
}

int strncmp(const char *s1, const char *s2, size_t n) {
  assert(s1 != NULL && s2 != NULL);
  while(n--){
    if(*s1 == 0 || *s1 != *s2) return (*s1 - *s2);
    s1++;
    s2++;
  }
  return 0;
  // panic("Not implemented");
}

void *memset(void *s, int c, size_t n) {
  const unsigned char uc = c;
  unsigned char *su;
  for(su = s; n > 0; ++su, --n) *su = uc;
  return s;
  // panic("Not implemented");
}

void *memmove(void *dst, const void *src, size_t n) {
  assert(dst && src && n);
  if(dst < src){
    for(int i = 0; i <= n; i++) *((char*)dst + i) = *((char*)src + i);
    return dst;
  }
  else if(dst > src){
    for(int i = n; i >= 0; i--) *((char*)dst + i) = *((char*)src + i);
    return dst;
  }
  else{
    return dst;
  }
  // panic("Not implemented");
}

void *memcpy(void *out, const void *in, size_t n) {
  void* ret = out;
  assert(out && in);
  while(n--){
    *(char*)out = *(char *)in;
    out = (char*)out + 1;
    in = (char*)in + 1;
  }
  return ret;
  // panic("Not implemented");
}

int memcmp(const void *s1, const void *s2, size_t n) {
  assert(s1 && s2 && (n > 0));
  while(n--){
    if(*(char*)s1 != *(char*)s2) return *(char*)s1 - *(char*)s2;
    s1 = (char*)s1 + 1;
    s2 = (char*)s2 + 1;
  }
  return 0;
  // panic("Not implemented");
}

#endif
