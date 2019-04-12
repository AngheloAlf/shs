/** \file ALF_OS.h
 * A Operative System detector using the defined macros.

 * This library tries to detect differents operative systems using the macros defined by the compiler.

 * Any  function return non-zero if at compile time detected it was compiled on the system it think it is.

 * 0 Otherwise.

 * Some alternative macros are implemented, their function is comodity for the dev.

**/

#ifndef ALF_OS_h
#define ALF_OS_h

#include "ALF_common.h"

#include <stdbool.h>

/// Detects the macro _WIN32
bool ALF_OS_isWindows(void);

/// Detects the macro __APPLE__
bool ALF_OS_isOSX(void);
/// Just another name for ALF_isOSX()
#define ALF_OS_isMac() ALF_OS_isOSX()

/// Detects the macro __linux__
bool ALF_OS_isLinux(void);

/// Detects if is a WIN32 and not a WIN64
bool ALF_OS_isWindows32(void);
/// Detects _WIN64
bool ALF_OS_isWindows64(void);

/// Detects __unix__
bool ALF_OS_isUnix(void);
/// Detects if defined __unix__ but is not a linux, apple, or android machine.
bool ALF_OS_isUnixOther(void);

/// Detects _POSIX_VERSION
bool ALF_OS_isPosix(void);

/// Detects __ANDROID__
bool ALF_OS_isAndroid(void);

/// Detects iOS (iPhone)
bool ALF_OS_isIOS(void);
/// Other name for ALF_isIOS()
#define ALF_OS_isIphone() ALF_OS_isIOS()
/// Detects TARGET_IPHONE_SIMULATOR
bool ALF_OS_isIOSSimulator(void);
/// Other name for ALF_isIOSSimulator()
#define ALF_OS_isIphoneSimulator() ALF_OS_isIOSSimulator()
/// Detects TARGET_OS_MAC
bool ALF_OS_isOSXOther(void);
/// Other name for ALF_isOSXOther()
#define ALF_OS_isMACOther() ALF_OS_isOSXOther()
/// Detect is Apple product, but not detected by the others macros. 
bool ALF_OS_isAppleOther(void);

/// Detects FreeBSD
bool ALF_OS_isFreeBSD(void);

/// Detects the not detected by this library.
/**

 * Returns 1 only if none of the others are detected by this library.

 * 0 Otherwise.
**/
bool ALF_OS_isOther(void);

/* OS DETECTOR */
/*
#ifdef __WIN32__
    //define something for Windows (32-bit and 64-bit, this part is common)
    #include <windows.h>
    // #include <winbase.h>
    // #include <windef.h>

    #ifdef __WIN64__
        //define something for Windows (64-bit only)
    #else
        //define something for Windows (32-bit only)
    #endif

#elif __unix__ 
    // Unix

    #if __APPLE__
        #include "TargetConditionals.h"
        #if TARGET_IPHONE_SIMULATOR
            // iOS Simulator
        #elif TARGET_OS_IPHONE
            // iOS device
        #elif TARGET_OS_MAC
            // Other kinds of Mac OS
        #else
            #error "Unknown Apple platform"
        #endif
    #elif defined(__ANDROID__)
        // Android
    #elif __linux__
         // linux

    #include <unistd.h>
    #elif defined(_POSIX_VERSION)
        // POSIX
        #error "Supported?"

    #endif
#else
    #error "Unknown compiler"
#endif
*/

#endif /* ALF_OS_h */
