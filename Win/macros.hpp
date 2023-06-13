#ifndef __Macros
#define __Macros

#ifdef UNICODE
#define QUOTE(a) QUOTEW(a)
#else
#define QUOTE(a) QUOTEA(a)
#endif

#define __QUOTEA(a) #a
#define __QUOTEW(a) L ## #a

#define QUOTEA(a) __QUOTEA(a)
#define QUOTEW(a) __QUOTEW(a)

#define BIT(n) (1u << n)

#endif  // __Macros