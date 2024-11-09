#ifndef DEBUG_H_
#define DEBUG_H_

#if defined (DEBUG)
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__);
#else
#define DEBUG_PRINT(...) ;
#endif // DEBUG

#if defined (DEBUG)
#define $ fprintf(stderr, ">> %s:%d\n", __FILE__, __LINE__);
#else 
#define $
#endif // DEBUG

#endif // DEBUG_H_
