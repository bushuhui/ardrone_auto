/** File: CThread.cpp
* Module: Common
* Description: Class for thread encapsualation
* Note:
* Author: Jan Faigl
* Created on: 23.06.2004
* History: 
*/

#include <stdio.h>
#include <string.h>
#include <errno.h>

#include "ardrone_CThread.h"


extern "C" {

void* CThread_thread_func(void* arg) {
    CThread * thread = static_cast<CThread*>(arg);
    thread->DoExecute();
    return NULL;
}

}

//----------------------------------------------------------------------------
//Class CThread
//----------------------------------------------------------------------------

int CThread::start_thread(void)
{
    //thread = SDL_CreateThread(StartThread, static_cast<void*>(this));
    if ( pthread_create( &thread, NULL, CThread_thread_func, this ) ) {
        printf("ERR: pthread_create: %s\n", strerror(errno));
        return -1;
    }

    return 0;
}

CThread::~CThread() {

}
