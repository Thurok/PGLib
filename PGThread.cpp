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

#include "PGThread.h"


using namespace PGThreadLib;


/********************************
*                               *
* Some PGThreadLib functions    *
*                               *
********************************/

void PGThreadLib::sleepThread(int milliseconds = 1)
{
    #ifdef WIN32
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
}



// Here is where really is made the call to the
// function which has to execute the thread
#ifdef WIN32
DWORD WINAPI PGThreadLib::ProxyFunc(void *p_data)
#else
void* PGThreadLib::ProxyFunc(void *p_data)
#endif
{
    // Convertimos los datos a ProxyData
    ProxyData *data = (ProxyData*)p_data;

    // Ejecutamos la funcion
    data->m_func(data->m_params);

    // Eliminamos los datos
    delete data;

    return 0;
}





/************************************
*                                   *
* PGThread class implementation     *
*                                   *
*************************************/
PGThread::PGThread(){
    myMutex = new PGMutex();
}

PGThread::~PGThread(){
    delete myMutex;
}

bool PGThread::createThread(ThreadFunc p_func, void *p_param, threadAttrs attrs){
    ThreadID th = 0;
    // Creamos un nuevo bloque de datos ProxyData en el heap
    ProxyData *data = new ProxyData;
    // Rellenamos la estructura con los punteros a los datos
    data->m_func = p_func;
    data->m_params = p_param;
    #ifdef WIN32
        // Creamos un thread en Windows
        HANDLE h;
        h = CreateThread(attrs, 0, ProxyFunc, data, 0, &th);
        if (h != 0) {
            // Añadimos el handle al mapa de handles
            g_handlemap[th] = h;
        }
        else cerr << "PGThreadLib -> can't create Windows thread" << endl;
    #else
        // Creamos un thread en UNIX
        pthread_create(&th, &attrs, ProxyFunc, data);
    #endif
    if(th == 0){
        // Borramos los datos primero
        delete data;
        // Lanzamos una excepcion
        //throw Exception(CreationFail);
        fprintf(stderr, "No se pudo crear el thread\n");
		perror("ERROR on creating thread");
        return false;
    }

    this->id = th;
    return true;
}

bool PGThread::createThread(ThreadFunc p_func, void *p_param){
    ThreadID th = 0;
    // Creamos un nuevo bloque de datos ProxyData en el heap
    ProxyData *data = new ProxyData;
    // Rellenamos la estructura con los punteros a los datos
    data->m_func = p_func;
    data->m_params = p_param;
    #ifdef WIN32
        // Creamos un thread en Windows
        HANDLE h;
        h = CreateThread(NULL, 0, ProxyFunc, data, 0, &th);
        if (h != 0) {
            // Añadimos el handle al mapa de handles
            g_handlemap[th] = h;
        }
        else cerr << "PGThreadLib -> can't create Windows thread" << endl;
    #else
        // Creamos un thread en UNIX
        pthread_create(&th, NULL, ProxyFunc, data);
    #endif
    if(th == 0){
        // Borramos los datos primero
        delete data;
        // Lanzamos una excepcion
        //throw Exception(CreationFail);
        fprintf(stderr, "No se pudo crear el thread\n");
		perror("ERROR on creating thread");
        return false;
    }

    this->id = th;
    return true;
}

void PGThread::killThread(){
    #ifdef WIN32
        // Termina el thread
        TerminateThread(g_handlemap[this->id], 0);
        // Cierra el handle al thread
        CloseHandle(g_handlemap[this->id]);
        // Elimina el handle del mapa
        g_handlemap.erase(this->id);
    #else
        // Cancelamos el thread
        pthread_cancel(this->id);
    #endif
}


void PGThread::waitToEnd(){
    #ifdef WIN32
        // Esperamos a que el thread finalice
        WaitForSingleObject(g_handlemap[this->id], INFINITE);
        // Cerramos el handle al thread
        CloseHandle(g_handlemap[this->id]);
        // Eliminamos el handle del mapa
        g_handlemap.erase(this->id);
    #else
        // Esperamos a que el thread termine haciendo un "join"
        pthread_join(this->id, NULL);
    #endif
}


ThreadID PGThread::getID(){
    return this->id;
}

PGMutex PGThread::getMutex(){
	return this->myMutex;
}

void PGThread::setMutex(PGMutex *newMutex){
    this->myMutex = newMutex;
}

void PGThread::lockMyMutex(){
    this->myMutex->lockMutex();
}

void PGThread::unlockMyMutex(){
    this->myMutex->unlockMutex();
}




/************************************
*                                   *
* PGMutex class implementation      *
*                                   *
*************************************/
PGMutex::PGMutex(){
    #ifdef WIN32
        InitializeCriticalSection(&this->mutex);
    #else
        pthread_mutex_init(&this->mutex, NULL);
    #endif
}
PGMutex::~PGMutex(){
    #ifdef WIN32
        DeleteCriticalSection(&this->mutex);
    #else
        pthread_mutex_destroy(&this->mutex);
    #endif
}

void PGMutex::lockMutex(){
    #ifdef WIN32
        EnterCriticalSection(&this->mutex);
    #else
        pthread_mutex_lock(&this->mutex);
    #endif
}

void PGMutex::unlockMutex(){
    #ifdef WIN32
        LeaveCriticalSection(&this->mutex);
    #else
        pthread_mutex_unlock(&this->mutex);
    #endif
}


















