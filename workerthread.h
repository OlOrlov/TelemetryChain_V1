#ifndef WORKERTHREAD_H
#define WORKERTHREAD_H
#include "dispatcher.h"

class Dispatcher;

class WorkerThread : public QThread
{
    Q_OBJECT
public:
    WorkerThread();
    void set_pointers(QUdpSocket* inp_pSock, Dispatcher* inp_pDispatcher,
                      std::condition_variable* inp_pWakeNext, std::mutex* inp_pWake_mtxNext,
                      std::mutex* inp_pSock_mtx, bool* inp_pNextAsleep, std::mutex* inp_pNextAsleep_mtx);
    bool init_socket(const char* inp_if_name);

    WorkerThread* pPrevWorker;// need to protect?
    WorkerThread* pNextWorker;

    QElapsedTimer timer;

    std::mutex asleep_mtx;
    bool asleep = true;

    std::condition_variable wake;
    std::mutex wake_mtx;

    std::condition_variable* pWakeNext;
    std::mutex* pWake_mtxNext;

protected:
    void run() override;

private:
    ether_header header;
    const char* if_name;

    std::mutex* pSock_mtx;
    bool* pNextAsleep;
    std::mutex* pNextAsleep_mtx;

    void kickNextWorker();//MUST happen only after the last sending
    quint16 i_port;
    QHostAddress ip_source;
    QUdpSocket* pRcvSocket;
    Dispatcher* pDispatcher;
    void sendEmpty();
    void processDG(quint8* received, quint8* rec1,
                   quint8* rec2, quint8* rec3,
                   quint8* rec4, quint8* rec5);
    bool filterIntput(quint8 rec);
    quint16 makeHammingCode(quint32 inp, quint16 numBits);
    void prepareOutput(quint8* inpArr);
    void send(quint8* toSend);
    bool haveFullFifthRec = false;
};

#endif // WORKERTHREAD_H
