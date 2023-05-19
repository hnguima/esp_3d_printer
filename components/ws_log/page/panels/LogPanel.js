import { Panel } from "Panel";

import { addStyleSheet } from "Base/utils";
import styles from "./LogPanel.css" assert { type: "css" };

import binary from "Images/binary.svg";

export class LogPanel extends Panel {
  constructor(options) {
    // Always call super first in constructor
    super(options);

    addStyleSheet(this.shadow, styles);

    this.name = (options && options.name) || "";
    this.title = (options && options.title) || "";

    this.icon = binary;
    this.enabled = true;

    const logField = (this.field = document.createElement("div"));
    logField.classList.add("log-field");

    const logText = document.createElement("div");
    logText.classList.add("log-text");

    this.wrapper.appendChild(logField);

    this.init_ws(this.field);
  }

  init_ws(field) {
    var ws = WebSocketTest(location.host);
    var msgCount = 0;

    console.log("got here");

    function WebSocketTest(ip) {
      if ("WebSocket" in window) {
        // Let us open a web socket
        var ws = new WebSocket("ws://" + ip + "/ws_log");
        // var ws = new WebSocket("ws://" + ip + "/ws_log");

        ws.onopen = function () {};

        ws.onmessage = function (evt) {
          var received_msg = evt.data;

          var justText = received_msg.replace(
            /[\u001b\u009b][[()#;?]*(?:[0-9]{1,4}(?:;[0-9]{0,4})*)?[0-9A-ORZcf-nqry=><]/g,
            ""
          );
          var escape = received_msg.slice(4, 6);

          var color = "#cccccc";
          if (escape == "32") {
            color = "#02d894";
          } else if (escape == "31") {
            color = "#e0797a";
          } else if (escape == "33") {
            color = "#d8e23a";
          }

          if (color != "white") {
            field.innerHTML +=
              '<p style="color: ' +
              color +
              '; margin-block: 5px">' +
              justText +
              "</p>\n";
            field.scrollTop = field.scrollHeight;
          }

          // var status = document.getElementById("status");
          // status.innerHTML = "connecting...";
          // status.style.color = "gold";
          // var led = document.getElementById("led");
          // led.style.background = "gold";

          msgCount++;
          if (msgCount > 5) {
            // status.innerHTML = "connected";
            // status.style.color = "#02d894";
            // led.style.background = "#02d894";
          }
        };

        ws.onclose = function () {
          console.log("Connection is closed... ");

          msgCount = 0;

          // var status = document.getElementById("status");
          // status.innerHTML = "disconnected";
          // status.style.color = "grey";

          // var led = document.getElementById("led");
          // led.style.background = "grey";

          // Lets try to reconnect every 5 seconds
          setTimeout(function () {
            if (ws.readyState == WebSocket.CLOSED) {
              ws = WebSocketTest(ip);
            }
          }, 1000);
          // websocket is closed.
        };
      } else {
        // The browser doesn't support WebSocket
        alert("WebSocket NOT supported by your Browser!");
      }

      return ws;
    }
  }
}

customElements.define("log-panel", LogPanel);
