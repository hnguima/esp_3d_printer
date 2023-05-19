import websocket
import sys
import time


def on_open(ws):
  # ws.send("timestamp: " + str(int(time.time())))
  print(str(int(time.time())))

def on_message(ws, message):
  print(message, end="")

def on_error(ws, err):
  print("Got a an error: ", err)
  run_ws()
  

def run_ws():
  ws = websocket.WebSocketApp(sys.argv[1],
  on_message = on_message,
  on_error = on_error,
  on_open = on_open)
  ws.run_forever()

run_ws()
