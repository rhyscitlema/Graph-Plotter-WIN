/*
	mthread.c
*/
#include <mthread.h>
#include <windows.h>



mthread_thread mthread_thread_new (void* (*function) (void* argument), void* argument)
{
	return CreateThread (NULL, 0, (LPTHREAD_START_ROUTINE)function, argument, 0, NULL);
}

void mthread_thread_join (mthread_thread thread) { WaitForSingleObject(thread, INFINITE); }



mthread_mutex mthread_mutex_new() { return CreateMutex (NULL, FALSE, NULL); }

bool mthread_mutex_lock   (mthread_mutex mutex, bool wait)
{
	unsigned int time = wait ? INFINITE : 0;
	return ( WaitForSingleObject(mutex, time) == WAIT_OBJECT_0 );
}

void mthread_mutex_unlock (mthread_mutex mutex) { ReleaseMutex(mutex); }

void mthread_mutex_free   (mthread_mutex mutex) { CloseHandle(mutex); }



#define USE_CUSTOM
#ifndef USE_CUSTOM



#else


typedef struct _Signal {
	mthread_mutex mutex;
	bool signaled;
	bool acquired;
	bool waiting;
} Signal;

static Signal signals[64] = {0};
static int signals_count = 0;

mthread_signal mthread_signal_new()
{
	Signal s;
	s.mutex = mthread_mutex_new();
	mthread_mutex_lock(s.mutex,1);
	s.signaled = false;
	s.acquired = true;
	s.waiting = false;
	Signal* signal = &signals[signals_count];
	*signal = s;
	return signal;
}

void mthread_signal_send (mthread_signal signal)
{
	Signal* s = (Signal*)signal;
	bool waiting = s->waiting;
	s->signaled = true;
	if(s->acquired) mthread_mutex_unlock(s->mutex);
	if(waiting) { while(s->signaled); }
	if(!s->signaled){
		mthread_mutex_lock(s->mutex,1);
		s->acquired = true;
	} else s->acquired = false;
}

void mthread_signal_wait (mthread_signal signal)
{
	Signal* s = (Signal*)signal;
	s->waiting = true;
	while(!s->signaled)
	{
		mthread_mutex_lock(s->mutex,1);
		mthread_mutex_unlock(s->mutex);
	}
	s->signaled = false;
	s->waiting = false;
}

void mthread_signal_free (mthread_signal signal)
{
	Signal* s = (Signal*)signal;
	mthread_mutex_free(s->mutex); s->mutex = NULL;
}

#endif
