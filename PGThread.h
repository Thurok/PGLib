/************************************************************************
* PGThreadLib v0.9														*
* author: Pau Guillamon													*
* date: July 2012														*
* 																		*
* Based in Oscar Campos posts in GenbetaDev:							*
* http://www.genbetadev.com/tag/introduccion-al-multiprocesamiento-en-c *
*                                                               		*
*More info at: http://obj.sytes.net										* 
*                                                               		*
************************************************************************/

#ifndef _PGThread_h_
#define _PGThread_h_


#include <iostream>
using namespace std;
#include <exception>

#ifdef WIN32
    #include <windows.h>
    #include <map>
    #include <process.h>    /* _beginthread, _endthread */
#else
	#include <pthread.h>
	#include <unistd.h>
	#include "bits/local_lim.h"
#endif
#include <stdio.h>
#include <stdlib.h>

namespace PGThreadLib
{
    /****************************
    * Some definitions          *
    ****************************/
    typedef void (*ThreadFunc)(void*);
    #ifdef WIN32
        typedef DWORD ThreadID;
		// this are used for thread attributes
		typedef LPSECURITY_ATTRIBUTES threadAttrs;
    #else
        typedef pthread_t ThreadID;
		// this are used for thread attributes
		typedef pthread_attr_t threadAttrs;
    #endif


    /****************************
    * Some usefull functions    *
    ****************************/
    void sleepThread(int milliseconds);



    /************************************************
    * Proxy class and funciton                      *
    * it is used to wrap some                       *
    * Windows and POSIX native functions            *
    * around a common interface.                    *
    * This is because Windows CreateThread() needs  *
    * a DWORD WINAPI function and                   *
    * POSIX pthread_create() needs a void* function *
    *                                               *
    ************************************************/
    class ProxyData{
        public:
            ProxyData(){}
            ~ProxyData(){}
            ThreadFunc m_func;
            void *m_params;
    };
    // Here is where really is made the call to the
    // function which has to execute the thread
    #ifdef WIN32
    DWORD WINAPI ProxyFunc(void *p_data);
    #else
    void* ProxyFunc(void *p_data);
    #endif


    /************************************************************
	* PGMutex allows to sync threads (PGThreads and others)     *
	************************************************************/
	class PGMutex{
	    public:
            PGMutex();
            ~PGMutex();

            void lockMutex();
            void unlockMutex();
	    protected:
	    private:
            #ifdef WIN32
                CRITICAL_SECTION mutex;
            #else
                pthread_mutex_t mutex;
            #endif
	};

	/**
	* TODO: PGcondvar
	* http://msdn.microsoft.com/en-us/library/windows/desktop/ms682052(v=vs.85).aspx
	*/


    /************************************************
    * PGThreads class allows to create a threads    *
    * PGThreads use the ProxyData class and         *
    * ProxyFunc function to call native             *
    * create thread function                        *
    ************************************************/
	class PGThread{
		public:
			// ctor
			PGThread();
			// dtor
			~PGThread();

			bool createThread(ThreadFunc p_func, void *p_param, threadAttrs);
			bool createThread(ThreadFunc p_func, void *p_param);
			void killThread();
			void waitToEnd();
			void lockMyMutex();
			void unlockMyMutex();

			ThreadID getID();
			PGMutex *getMutex();
			
            void setMutex(PGMutex *);
		protected:
		private:
            ThreadID id;
            PGMutex *myMutex;
            #ifdef WIN32
                std::map<DWORD, HANDLE> g_handlemap;
            #endif
	};

}


#endif // _PGThread_h_














