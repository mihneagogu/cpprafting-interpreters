#ifndef UTIL_H_
#define UTIL_H_
#include <cstdio>
#include <optional>

#define ASSERT_COND(__cond, __msg) do {                  \
        if (!(__cond)) { printf("%s\n", __msg); } } while (0)

#define ASSERT_ALLOC(__ptr) ASSERT_COND(__ptr, "Allocation failure")

#define Option std::optional
#define None (std::nullopt)

/*
** Initializes a union field of type "_ty" with _ty's constructor, passing the expression
** to pass to the constructor.
** Especially useful with move constructors
** This special initialization is necessary because union fields use ininited memory, so we need
** to initialize the pointer in place so that we do not read uninited memory accidentally
*/
#define init_union_field(_fieldname, _ty, _to_init_with) new((void *) &_fieldname) _ty(_to_init_with)



#endif // UTIL_H_
