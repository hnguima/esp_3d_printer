import { Panel } from "Panel";
import { IPField, IntegerField } from "Field";

import icon from "Images/web.svg";

export class ServerPanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "server";

    this.title = "Servidor";
    this.icon = icon;

    new IPField({
      panel: this,
      label: "Endereço do servidor",
      name: "ip",
    });

    new IntegerField({
      panel:this, 
      name: "port", 
      label: "Porta",
      max: 65535
    })
  }
}

customElements.define("server-panel", ServerPanel);
