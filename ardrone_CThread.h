/** File: CThread.cpp
* Module: Common
* Description: Class for thread encapsualation
* Note:
* Author: Jan Faigl
* Created on: 23.06.2004
* History: 
*/

#ifndef ardrone_CThread_h
#define ardrone_CThread_h

#include <pthread.h>

extern "C" void* CThread_thread_func(void *arg);

class CThread {
public:
    virtual int DoExecute(void) = 0;

    int start_thread(void);

    friend void* CThread_thread_func(void*);

public:
    virtual ~CThread() = 0;

protected:
    pthread_t   thread;
};

#endif
