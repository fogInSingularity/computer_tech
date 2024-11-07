#ifndef ATTR_H_
#define ATTR_H_

#if defined(__cplusplus)
    #if __cplusplus >= 201703L
        #define UNUSED [[maybe_unused]]
    #else
        #if defined(__GNUG__) || defined(__clang__)
            UNUSED __attribute__((unused))
        #else
            #define UNUSED
        #endif 
#endif
#else
    #if defined(__GNUC__) || defined(__clang__)
        #define UNUSED __attribute__((unused))
    #else
        #define UNUSED
    #endif
#endif

#endif // ATTR_H_
