//
// time.h - Заголовок с полезными кроссплатформенными способами работы со временем.
//

#pragma once


// Подключаем:
#include "std.h"
#include "math.h"


#ifdef _WIN32
    #include <windows.h>
#else
    #include <time.h>
    #include <sys/time.h>
#endif


// Возвращает время с начала Unix-эпохи в секундах (double) с точностью до мс:
static inline double Time_now(double *x) {
    double result;
    #ifdef _WIN32
        FILETIME ft;
        ULARGE_INTEGER uli;
        GetSystemTimeAsFileTime(&ft);
        uli.LowPart  = ft.dwLowDateTime;
        uli.HighPart = ft.dwHighDateTime;
        // Вычитаем разницу между 1601 и 1970 годами (в 100-нс интервалах):
        result = (double)(uli.QuadPart - 116444736000000000ULL) / 10000000.0;
    #else
        struct timeval tv;
        gettimeofday(&tv, NULL);
        result = (double)tv.tv_sec + (double)tv.tv_usec / 1e6;
    #endif
    if (x != NULL) { *x = result; }
    return result;
}


// Остановить выполнение кода на определенное время в секундах с дробной частью (малая точность чем у Time_delay):
static inline void Time_sleep(double seconds) {
    if (seconds <= 0.0) { return; }
    #ifdef _WIN32
        Sleep((unsigned long)(seconds*1000.0));
    #else
        struct timespec ts;
        ts.tv_sec  = (time_t)floor(seconds);
        ts.tv_nsec = (long)((seconds - ts.tv_sec) * 1e9);
        nanosleep(&ts, NULL);
    #endif
}


// Задержать выполнение кода на определенное время в секундах с дробной частью (большая точность чем у Time_sleep):
// Комбинированный sleep (по возможности) + active wait для докрутки ожидания в коде.
static inline void Time_delay(double seconds) {
    if (seconds <= 0.0) return;
    // Задержка переключения со сна в разных системах:
    #ifdef _WIN32
        const double OS_TICK = 0.015625;
    #else
        const double OS_TICK = 0.001;
    #endif

    double start = Time_now(NULL);                        // Получаем текущее время.
    double sleep_dur = seconds - fmod(seconds, OS_TICK);  // Сколько реально времени точно можно поспать.
    if (sleep_dur > 0.0) { Time_sleep(sleep_dur); }       // Спим не нагружая процессор циклом.
    double target = start + seconds;
    while (Time_now(NULL) < target);  // Докручиваем время проверяя текущее время с целевым временем.
}
