/* <AvailabilityMacros.h> for the Mac OS X 10.0 and 10.1 headers, which have
 * none (it arrived in 10.2). It gives the version numbers and the two
 * limits that code tests. The newest version the headers allow is 10.0.
 * The availability attributes of the 10.2 header are not given. Where the
 * system has the header, it is used. */
#if __has_include_next(<AvailabilityMacros.h>)
#include_next <AvailabilityMacros.h>
#elif !defined(__AVAILABILITYMACROS__)
#define __AVAILABILITYMACROS__
#define MAC_OS_X_VERSION_10_0 1000
#define MAC_OS_X_VERSION_10_1 1010
#define MAC_OS_X_VERSION_10_2 1020
#define MAC_OS_X_VERSION_10_3 1030
#define MAC_OS_X_VERSION_10_4 1040
#define MAC_OS_X_VERSION_10_5 1050
#ifndef MAC_OS_X_VERSION_MIN_REQUIRED
#ifdef __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#define MAC_OS_X_VERSION_MIN_REQUIRED __ENVIRONMENT_MAC_OS_X_VERSION_MIN_REQUIRED__
#else
#define MAC_OS_X_VERSION_MIN_REQUIRED MAC_OS_X_VERSION_10_0
#endif
#endif
#ifndef MAC_OS_X_VERSION_MAX_ALLOWED
#define MAC_OS_X_VERSION_MAX_ALLOWED MAC_OS_X_VERSION_10_0
#endif
#define AVAILABLE_MAC_OS_X_VERSION_10_0_AND_LATER
#define AVAILABLE_MAC_OS_X_VERSION_10_1_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_0_AND_LATER
#define DEPRECATED_IN_MAC_OS_X_VERSION_10_1_AND_LATER
#endif
