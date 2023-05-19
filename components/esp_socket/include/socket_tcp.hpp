#pragma once

#include <iostream>
#include <functional>
#include <string>
#include <vector>

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <esp_err.h>

#include <esp_log.h>
#include <esp_event.h>

#include "lwip/sockets.h"

#define MAX_SOCKETS 2

#define SOCKET_RETRY_INTERVAL 5000

#define SOCKET_RX_BUFFER_MAX 1024
#define SOCKET_TX_BUFFER_MAX 1024

#define RETURN_OR_CONTINUE(condition) \
    if (condition)                    \
    {                                 \
        return;                       \
    }                                 \
    else                              \
    {                                 \
        continue;                     \
    }

#define RETURN_OR_BREAK(condition) \
    if (condition)                 \
    {                              \
        return false;              \
    }                              \
    else                           \
    {                              \
        break;                     \
    }

class Socket
{
protected:
public:
    Socket();

    typedef std::function<void(void)> handler_func_t;
    typedef std::function<void(uint8_t *, int16_t &)> recv_handler_func_t;

    esp_err_t on_recv(recv_handler_func_t func);
    bool send_bytes(int client_fd, const uint8_t *buffer, size_t size);

    bool operator==(Socket const &rhs);

protected:
    recv_handler_func_t on_recv_cb;

    bool retry();
    bool recv_loop();
    void delete_task();
    void wait_for_network();

    char task_name[32];
    TaskHandle_t *task_handle;

    int fd;

    uint8_t buffer_rx[SOCKET_RX_BUFFER_MAX];
    int16_t size_rx;
    uint8_t buffer_tx[SOCKET_TX_BUFFER_MAX];
    int16_t size_tx;

    bool is_persistent;
    bool is_vital;

    uint16_t retries;
    uint16_t max_retries;
    uint16_t retry_delay;

    static void socket_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data);
    static bool has_network;
    static char ip[16];

    SemaphoreHandle_t mutex;
};
