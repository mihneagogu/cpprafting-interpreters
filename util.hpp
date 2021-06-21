#ifndef UTIL_H_
#define UTIL_H_
#include <cstdio>
#include <optional>

#define ASSERT_COND(__cond, __msg) do {                  \
        if (!(__cond)) { printf("%s\n", __msg); } } while (0)

#define ASSERT_ALLOC(__ptr) ASSERT_COND(__ptr, "Allocation failure")

#define Option std::optional
#define None (std::nullopt)




#endif // UTIL_H_
