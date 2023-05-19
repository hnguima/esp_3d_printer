import { SwitchPanel } from "Panel";
import { TextField, PasswordField } from "Field";

import icon from "Images/wifi.svg";

export class WifiPanel extends SwitchPanel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "wifi";

    this.title = "Wifi";
    this.icon = icon;

    new TextField({
      panel: this,
      name: "ssid",
      label: "SSID da rede",
      placeholder: "Insira o SSID da rede",
    });

    new PasswordField({
      panel: this,
      name: "password",
    });
  }
}

customElements.define("wifi-panel", WifiPanel);
