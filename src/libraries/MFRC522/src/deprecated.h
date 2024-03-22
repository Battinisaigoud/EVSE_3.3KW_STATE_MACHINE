// 1 "D:\\Current_Project_15-10-2022\\Current_project_13-12-2022\\Testing_Issues\\Testing_Issues_15-02-2023\\POD_EVRE_GENERIC_IOCL_14-03-2023\\POD_EVRE_GENERIC\\src\\libraries\\MFRC522\\src\\deprecated.h"
/**
 * Copyright (c) 2016 by Ludwig Grill (www.rotzbua.de)
 * Simple deprecated workaround for Arduino IDE
 * IDE 1.6.8 use gcc 4.8 which do not support c++14 [[deprecated]]
 * Later versions should support c++14, then use c++14 syntax
 */
#ifndef DEPRECATED_H
#define DEPRECATED_H

#ifdef __has_cpp_attribute
#if __has_cpp_attribute(deprecated)
#define DEPRECATED [[deprecated]]
#define DEPRECATED_MSG(msg) [[deprecated(msg)]]
#endif // __has_cpp_attribute(deprecated)
#else
#define DEPRECATED __attribute__((deprecated))
#define DEPRECATED_MSG(msg) __attribute__((deprecated(msg)))
#endif // __has_cpp_attribute

#endif // DEPRECATED_H
