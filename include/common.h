#ifndef __COMMON_H__
#define __COMMON_H__

#ifdef DEBUG
#define LOG_DEBUG(format, ...) printf("%s:%d: " format, __FILE__, __LINE__, ##__VA_ARGS__)
#define LOG(format, ...) printf(format, ##__VA_ARGS__)
#else
#define LOG_DEBUG(...) /*** expands to nothing ***/
#define LOG(...)       /*** expands to nothing ***/
#endif

#endif