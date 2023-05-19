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

#define MAX_CLIENTS 10

class SocketServer : public Socket
{
protected:
public:
    SocketServer(uint16_t port);

    typedef std::function<void(int)> client_handler_func_t;
    typedef std::function<void(int, uint8_t *, int16_t &)> client_recv_handler_func_t;

    esp_err_t on_client_connect(client_handler_func_t func);
    esp_err_t on_client_disconnect(client_handler_func_t func);
    esp_err_t on_client_recv(client_recv_handler_func_t func);

    bool operator==(SocketServer const &rhs);

private:
    std::vector<int> clients;

    uint16_t port;

    client_handler_func_t on_client_connect_cb;
    client_handler_func_t on_client_disconnect_cb;
    client_recv_handler_func_t on_client_recv_cb;

    fd_set all_fds;

    static std::vector<SocketServer *> open_sockets;

    int bind_socket();
    bool server_loop();

    static void task(void *param);
    void delete_task();

};
