#ifndef _LIB__MISC_H
#define _LIB__MISC_H

#include <lib/panic.h>

#define MIN(A, B) ({ \
    typeof(A) MIN_a = A; \
    typeof(B) MIN_b = B; \
    MIN_a < MIN_b ? MIN_a : MIN_b; \
})

#define MAX(A, B) ({ \
    typeof(A) MAX_a = A; \
    typeof(B) MAX_b = B; \
    MAX_a > MAX_b ? MAX_a : MAX_b; \
})

#define DIV_ROUNDUP(VALUE, DIV) ({ \
    typeof(VALUE) DIV_ROUNDUP_value = VALUE; \
    typeof(DIV) DIV_ROUNDUP_div = DIV; \
    (DIV_ROUNDUP_value + (DIV_ROUNDUP_div - 1)) / DIV_ROUNDUP_div; \
})

#define ALIGN_UP(VALUE, ALIGN) ({ \
    typeof(VALUE) ALIGN_UP_value = VALUE; \
    typeof(ALIGN) ALIGN_UP_align = ALIGN; \
    DIV_ROUNDUP(ALIGN_UP_value, ALIGN_UP_align) * ALIGN_UP_align; \
})

#define ALIGN_DOWN(VALUE, ALIGN) ({ \
    typeof(VALUE) ALIGN_DOWN_value = VALUE; \
    typeof(ALIGN) ALIGN_DOWN_align = ALIGN; \
    (ALIGN_DOWN_value / ALIGN_DOWN_align) * ALIGN_DOWN_align; \
})

#define SIZEOF_ARRAY(ARRAY) (sizeof(ARRAY) / sizeof(ARRAY[0]))

#define ASSERT(COND) do { \
    if (!(COND)) { \
        panic(NULL, true, "Assertion failed: " #COND); \
    } \
} while (0)

#define ASSERT_MSG(COND, ...) do { \
    if (!(COND)) { \
        panic(NULL, true, __VA_ARGS__); \
    } \
} while (0)

#define CAS __sync_bool_compare_and_swap

typedef char symbol[];

#endif
