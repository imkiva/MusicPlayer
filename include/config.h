#pragma once


#ifdef ANDROID
#	define DATA_ENV "EXTERNAL_STORAGE"
#	define DATA_DEFAULT_PATH "/sdcard"
#else
#	define DATA_ENV "HOME"
#	define DATA_DEFAULT_PATH "/tmp"
#endif

