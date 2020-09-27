#pragma once

#ifdef _WINDOWS
#pragma warning(disable:4530)
#pragma warning(disable:4068)
#endif

#define __STRINGIFY__(x)        #x
#define __EXPAND__(x)           __STRINGIFY__(x)

#define VERSION_MAJOR			0
#define	VERSION_MINOR			1
#define VERSION_PATCH			1
#define VERSION					__EXPAND__(VERSION_MAJOR) "." __EXPAND__(VERSION_MINOR) "." __EXPAND__(VERSION_PATCH)
#define BUILDTIMESTAMP	        __DATE__ " " __TIME__

#define APP_NAME_LONG           "Adventures in Tucson"
#define APP_DOMAIN              "ttvg"
#define APP_TITLE               APP_NAME_LONG " " VERSION

