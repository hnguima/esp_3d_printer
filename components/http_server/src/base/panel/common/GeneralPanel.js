import { Panel } from "Panel";
import { IntegerField, ButtonField, TextField, TimeField } from "Field";

import icon from "Images/upload.svg";
import save from "Images/save.svg";
import factory from "Images/shutdown.svg";

export class GeneralPanel extends Panel {
  constructor() {
    // Always call super first in constructor
    super();

    this.name = "general";

    this.title = "Gerais";
    this.icon = icon;

    new TextField({
      inline: true,
      readonly: true,
      panel: this,
      name: "hwVersion",
      label: "Versão do Hardware",
    });

    new IntegerField({
      inline: true,
      readonly: true,
      panel: this,
      name: "upTime",
      label: "Tempo ligado",
    });

    new TimeField({
      inline: true,
      readonly: true,
      panel: this,
      name: "installTime",
      label: "Data da instalação",
    });

    new ButtonField({
      panel: this,
      label: "Reiniciar o dispositivo",
      img: factory,
      color: "gold",
      callback: () => {
        console.log(this.value);
        alert("hello from button");
      },
    });
  }
}

customElements.define("general-panel", GeneralPanel);
