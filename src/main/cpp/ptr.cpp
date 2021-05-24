//
// Created by tianyang on 2021/5/6.
//
#include <signal.h>
#include <setjmp.h>
#include "pudge.h"

static volatile bool segv_flag;
static jmp_buf jmp_env;

static void segv_handler( int sig )
{
    LOGD("segv_handler sig=%d",sig);
    segv_flag = false;
    longjmp( jmp_env , 1);
}

bool pudge::isGoodPtr(void* ptrInput){
    struct sigaction oldaction, newaction;
    int *ptr = (int *)ptrInput;
    if(ptrInput == 0){
        return false;
    }

    newaction.sa_handler = segv_handler;
    newaction.sa_flags = 0;
    sigemptyset( &newaction.sa_mask);

    sigaction( SIGSEGV, &newaction, &oldaction);

    segv_flag = true;

    if (setjmp(jmp_env) == 0)
    {
        int origin = *ptr; //try read
        LOGD("isGoodPtr addr=%p,val=%d",ptr,origin);
        ptr[0] = 1; //try write
        ptr[0] = origin;
    } else {
        //LOGD("isGoodPtr setjmp fail %p",ptr);
    }

    sigaction(SIGSEGV, &oldaction, NULL);
    return segv_flag;
}