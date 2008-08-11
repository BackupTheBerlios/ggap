#ifndef MOO_UTILS_H
#define MOO_UTILS_H

#include <QtGlobal>

#define M_DISABLE_COPY(Class) \
    Class(const Class &); \
    Class &operator=(const Class &);

#define M_N_ELEMENTS(ar) (sizeof(ar)/sizeof((ar)[0]))

#define M_STMT_START do
#define M_STMT_END while (0)

#define m_return_if_fail_(cond,rval) \
M_STMT_START { \
    if (!(cond)) \
    { \
        qCritical("%s: assertion `%s' failed", \
                  Q_FUNC_INFO, #cond); \
        return rval; \
    } \
} M_STMT_END

#define m_return_if_fail(cond) m_return_if_fail_(cond,)
#define m_return_val_if_fail(cond,val) m_return_if_fail_(cond,val)

#define m_return_if_reached() \
M_STMT_START { \
    qCritical("%s, line %d: should not be reached", \
              Q_FUNC_INFO, __LINE__); \
    return; \
} M_STMT_END

#define m_return_val_if_reached(val) \
M_STMT_START { \
    qCritical("%s, line %d: should not be reached", \
              Q_FUNC_INFO, __LINE__); \
    return val; \
} M_STMT_END

inline void m_segfault()
{
    char *a__ = 0;
    a__[-1] = 9;
}

#define M_ABORT() m_segfault()

#ifndef QT_NO_DEBUG
#define M_ASSERT(cond) \
M_STMT_START { \
    if (!(cond)) \
    { \
        qCritical("ASSERT: \"%s\" in file %s, line %d", #cond, __FILE__, __LINE__); \
        M_ABORT(); \
    } \
} M_STMT_END
#else
#define M_ASSERT(cond) Q_ASSERT(cond)
#endif

#define m_code_warning__(msg) \
M_STMT_START { \
    static bool been_here_##__LINE__; \
    if (!been_here_##__LINE__) \
    { \
        been_here_##__LINE__ = true; \
        qCritical(msg ": %s", Q_FUNC_INFO); \
    } \
} M_STMT_END

#define M_IMPLEMENT_ME() m_code_warning__("IMPLEMENT ME")
#define M_FIXME() m_code_warning__("FIXME")

#define M_IMPLEMENT_ME_FATAL() \
M_STMT_START { \
    qCritical("IMPLEMENT ME: %s in file %s, line %d", Q_FUNC_INFO, __FILE__, __LINE__); \
    M_ABORT(); \
} M_STMT_END

#endif // MOO_UTILS_H
