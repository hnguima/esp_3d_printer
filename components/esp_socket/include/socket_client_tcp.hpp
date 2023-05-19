#pragma once

#include "socket_tcp.hpp"

#include <iostream>
#include <functional>
#include <string>
#include <vector>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>

#include <esp_log.h>

#include "lwip/sockets.h"

#define MAX_SOCKETS 2

#define SOCKET_RETRY_INTERVAL 5000

#define SOCKET_RX_BUF_MAX 1024
#define SOCKET_TX_BUF_MAX 1024

class SocketClient : public Socket
{
protected:
public:
    SocketClient(std::string addr, uint16_t port);

    esp_err_t on_connect(handler_func_t func);
    esp_err_t on_disconnect(handler_func_t func);

    bool operator==(SocketClient const &rhs);

private:
    static std::vector<SocketClient *> open_sockets;

    std::string addr;
    uint16_t port;

    handler_func_t on_connect_cb;
    handler_func_t on_disconnect_cb;

    int connect_socket();
    bool retry();

    static void task(void *param);
    void delete_task();
};
