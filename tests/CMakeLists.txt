add_executable(timestamp_test Timestamp_test.cpp)
target_link_libraries(timestamp_test muduo_base)

add_executable(thread_test Thread_test.cpp)
target_link_libraries(thread_test muduo_base)

add_executable(mutex_test Mutex_test.cpp)
target_link_libraries(mutex_test muduo_base)

add_executable(condition_test Condition_test.cpp)
target_link_libraries(condition_test muduo_base)

add_executable(countDownLatch_test CountDownLatch_test.cpp)
target_link_libraries(countDownLatch_test muduo_base)

add_executable(blockingQueue_test BlockingQueue_test.cpp)
target_link_libraries(blockingQueue_test muduo_base)

add_executable(threadPool_test ThreadPool_test.cpp)
target_link_libraries(threadPool_test muduo_base)

add_executable(timer_test Timer_test.cpp)
target_link_libraries(timer_test muduo_net muduo_base)

add_executable(eventLoop_test_1 EventLoop_test_1.cpp)
target_link_libraries(eventLoop_test_1 muduo_net muduo_base)

add_executable(eventLoop_test_2 EventLoop_test_2.cpp)
target_link_libraries(eventLoop_test_2 muduo_net muduo_base)

add_executable(eventLoop_test_3 EventLoop_test_3.cpp)
target_link_libraries(eventLoop_test_3 muduo_net muduo_base)

add_executable(eventLoop_test_4 EventLoop_test_4.cpp)
target_link_libraries(eventLoop_test_4 muduo_net muduo_base)

add_executable(socket_test Socket_test.cpp)
target_link_libraries(socket_test muduo_net muduo_base)

add_executable(acceptor_test Acceptor_test.cpp)
target_link_libraries(acceptor_test muduo_net muduo_base)

add_executable(buffer_test Buffer_test.cpp)
target_link_libraries(buffer_test muduo_net muduo_base)

add_executable(tcpConnection_test TcpConnection_test.cpp)
target_link_libraries(tcpConnection_test muduo_net muduo_base)

add_executable(tcpServer_single_io_test TcpServer_single_io_test.cpp)
target_link_libraries(tcpServer_single_io_test muduo_net muduo_base)

add_executable(tcpServer_test TcpServer_test.cpp)
target_link_libraries(tcpServer_test muduo_net muduo_base)

add_executable(tcpServer_test_2 TcpServer_test_2.cpp)
target_link_libraries(tcpServer_test_2 muduo_net muduo_base)

add_executable(tcpServer_test_3 TcpServer_test_3.cpp)
target_link_libraries(tcpServer_test_3 muduo_net muduo_base)