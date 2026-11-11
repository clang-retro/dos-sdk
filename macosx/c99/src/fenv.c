/* The default floating point environment for <fenv.h>: round to nearest,
 * no exceptions raised or enabled. */
#include <fenv.h>

const fenv_t _FE_DFL_ENV = 0;
