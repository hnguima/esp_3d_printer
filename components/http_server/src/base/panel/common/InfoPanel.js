import { Panel } from "Panel";
import { LongTextField, TextField } from "Field";

import icon from "Images/info.svg";

export class InfoPanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "info";
    this.tab = "info";

    this.title = "Info";
    this.icon = icon;

    const description = new LongTextField();
    description.placeholder = "Escreva uma descrição para o dispositivo";


    this.addField(description, "description");
  }
}

customElements.define("info-panel", InfoPanel);
