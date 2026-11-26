/* <sched.h> for C++. Before 10.2, the system header does not declare its
 * functions extern "C", so C++ code cannot link to them. */
#ifdef __cplusplus
extern "C" {
#endif
#include_next <sched.h>
#ifdef __cplusplus
}
#endif
