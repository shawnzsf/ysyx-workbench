#include <am.h>
#include <klib.h>
#include <klib-macros.h>
#include <stdarg.h>
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

int printf(const char *fmt, ...) {
  panic("Not implemented");
}

int vsprintf(char *out, const char *fmt, va_list ap) {
  // panic("Not implemented");
  return vsnprintf(out, -1, fmt, ap);
}

int sprintf(char *out, const char *fmt, ...) {
  // panic("Not implemented");
  va_list ap;
  int ret = -1;
  va_start(ap, fmt);
  ret = vsprintf(out, fmt, ap);
  va_end(ap);
  return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
  panic("Not implemented");
}

int vsnprintf(char *out, size_t n, const char *fmt, va_list ap) {
  // panic("Not implemented");
  int pos = 0;

  while(*fmt != '\0') {
    while (*fmt != '%' && *fmt != '\0') {
      out[pos++] = *fmt++;
      if (pos > n) return n;
    }

    if (*fmt == '%') fmt++;
    else if (*fmt == '\0') break;

    switch (*fmt) {
      case 's': {
        char *s = va_arg(ap, char *);
        while (*s != '\0') {
          out[pos++] = *s++;
          if (pos > n) return n;
        }
        break;
      }
      case 'd': {
        int d = va_arg(ap, int);
        if (d < 0) {
            d = -d;
            out[pos++] = '-';
            if (pos > n) return n;
        }
        char num[20] = {};
        int rem = 0, length = 0;
        do {
          rem = d % 10;
          d = d / 10;
          num[length++] = rem + '0';
        } while (d > 0);

        length--;
        while (length >= 0) {
          out[pos++] = num[length];
          if (pos > n) return n;
          length--;
        }
        break;
      }
    }
      fmt++;
    }

  if (pos > n) return n;
  out[pos] = '\0';
  return pos;
}

#endif
