#include <esp_serial.hpp>
#include <esp_generic_test.hpp>

#include <esp_log.h>

GenericTest test_serial_rs485_loopback(
    "Serial RS486 Loopback", __LINE__, __FILE__,
    []()
    {
      char resp[32];
      int ret = 0;

      printf("\n\nIniciando teste do modbus\n\n");
      QUESTION("O cabo loopback RS485 está conectado? ", "\n", "Conecte o cabo loopback e reinicie este teste.\n", resp, ret);

      if (!ret)
      {
        TEST_ASSERT(false);
      }

      Serial *serial_1 = new Serial(UART_NUM_1, 115200, UART_DATA_8_BITS, UART_PARITY_DISABLE, UART_STOP_BITS_1,
                                    SERIAL_DEFAULT_FLOW_CONTROL, UART_MODE_RS485_HALF_DUPLEX);
      Serial *serial_2 = new Serial(UART_NUM_2, 115200, UART_DATA_8_BITS, UART_PARITY_DISABLE, UART_STOP_BITS_1,
                                    SERIAL_DEFAULT_FLOW_CONTROL, UART_MODE_RS485_HALF_DUPLEX);

      char str_test_1[32] = "Hello World do canal 1";
      char str_test_2[32] = "Hello World do canal 2";

      printf("\nEnviando pacote via RS485: CANAL 1 -> CANAL 2:");

      printf("\nEnviando do CANAL 1: %s\n", str_test_1);
      serial_1->send_bytes((uint8_t *)str_test_1, strlen(str_test_1));

      bool *serial_2_received = new bool;
      *serial_2_received = false;

      serial_2->on_recv(
          [serial_1, serial_2, serial_2_received, str_test_1](uint8_t *data, size_t lenght)
          {
            char data_str[lenght + 1] = {0};
            memcpy(data_str, data, lenght);

            printf("Recebido do CANAL 2: %s", data_str);

            if (lenght <= 0 || strcmp(data_str, str_test_1) != 0)
            {
              printf("Falha na leitura do CANAL 2\n");
              delete serial_1;
              delete serial_2;
              TEST_ASSERT(false);
            }

            *serial_2_received = true;
          });

      // wait for message
      int i = 0;
      while (*serial_2_received == false)
      {

        if (i++ > 5)
        {
          delete serial_1;
          delete serial_2;
          TEST_ASSERT(false);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("Waiting (%ds)...\n", i);
      }

      printf("\nSucesso!\n\n");

      printf("Enviando pacote via RS485: CANAL 2 -> CANAL 1:\n");

      printf("\nEnviando do CANAL 2: %s\n", str_test_2);
      serial_2->send_bytes((uint8_t *)str_test_2, strlen(str_test_2));

      bool *serial_1_received = new bool;
      *serial_1_received = false;

      serial_1->on_recv(
          [serial_1, serial_2, serial_1_received, str_test_2](uint8_t *data, size_t lenght)
          {
            char data_str[lenght + 1] = {0};
            memcpy(data_str, data, lenght);

            printf("Recebido do CANAL 1: %s", data_str);

            if (lenght <= 0 || strcmp(data_str, str_test_2) != 0)
            {
              printf("Falha na leitura do CANAL 1\n");
              delete serial_1;
              delete serial_2;
              TEST_ASSERT(false);
            }

            *serial_1_received = true;
          });

      // wait for message
      i = 0;
      while (*serial_1_received == false)
      {
        if (i++ > 5)
        {
          delete serial_1;
          delete serial_2;
          TEST_ASSERT(false);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);

        printf("Waiting (%ds)...\n", i);
      }

      printf("Sucesso!\n\n");

      TEST_ASSERT(true);
    });
