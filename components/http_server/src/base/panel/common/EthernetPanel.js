import { Panel } from "Panel";
import { IPField } from "Field";

import icon from "Images/net.svg";

export class EthernetPanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "ethernet";

    this.title = "Ethernet";
    this.icon = icon;

    new IPField({
      panel: this,
      label: "Endereço IP",
      name: "ip",
    });

    new IPField({
      panel: this,
      label: "Máscara de rede",
      name: "mask",
    });

    new IPField({
      panel: this,
      label: "Gateway",
      name: "gw",
    });
  }
}

customElements.define("ethernet-panel", EthernetPanel);
