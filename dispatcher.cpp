#include "dispatcher.h"

Dispatcher::Dispatcher(const char *if_name)
{
    WorkerThread wThread1;
    WorkerThread wThread2;
    WorkerThread wThread3;

    pWThread1 = &wThread1;
    pWThread2 = &wThread2;
    pWThread2 = &wThread3;

    wThread1.pPrevWorker = &wThread3;
    wThread2.pPrevWorker = &wThread1;
    wThread3.pPrevWorker = &wThread2;

    wThread1.pNextWorker = &wThread2;
    wThread2.pNextWorker = &wThread3;
    wThread3.pNextWorker = &wThread1;

    wThread1.timer.start();
    wThread2.timer.start();
    wThread3.timer.start();

    bool initSuccess = true;
    std::mutex socketMutex;
    QUdpSocket readSocket;
    if(!readSocket.bind(QHostAddress(TELEMETRY_CHAIN_IP), I_PORT))
        initSuccess = false;

    wThread1.set_pointers(&readSocket, this, &wThread2.wake, &wThread2.wake_mtx, &socketMutex, &wThread2.asleep, &wThread2.asleep_mtx);
    wThread2.set_pointers(&readSocket, this, &wThread3.wake, &wThread3.wake_mtx, &socketMutex, &wThread3.asleep, &wThread3.asleep_mtx);
    wThread3.set_pointers(&readSocket, this, &wThread1.wake, &wThread1.wake_mtx, &socketMutex, &wThread1.asleep, &wThread1.asleep_mtx);

    if(!wThread1.init_socket(if_name)) {initSuccess = false;};
    if(!wThread2.init_socket(if_name)) {initSuccess = false;};
    if(!wThread3.init_socket(if_name)) {initSuccess = false;};

    wThread2.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));
    wThread3.start();
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    if(initSuccess)
    {
        wThread1.start();
        while(true)
        {
            std::this_thread::sleep_for(std::chrono::hours::max());
        }
    }
    else
    {
        fprintf(stderr, "Failed to initiate libpcap or udp socket");
        exit(1);
    }
}
