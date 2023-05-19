import { Panel } from "Panel";
import {
  TextField,
  SwitchField,
  PasswordField,
  FileField,
  ButtonField,
} from "Field";
import { addTooltip } from "Tooltip";

import icon from "Images/person.svg";
import gear from "Images/gear.svg";
import download from "Images/download.svg";
import factory from "Images/factory.svg";

export class Test1Panel extends Panel {
  constructor() {
    // Always call super first in constructor
    super({
      checked: true,
    });

    this.name = "main";
    this.tab = "main";

    this.title = "Main panel";
    this.icon = icon;
    this.enabled = true;

    const textfield = new TextField({
      panel: this,
      name: "texttest",
      label: "text test",
      placeholder: "aaaaaaaaaaasdf",
    });

    addTooltip({
      element: textfield,
      msg: "hello this is a tooltip",
      img: gear,
    });

    const switchField = new SwitchField({
      panel: this,
      name: "switch",
      label: "switch test",
    });

    const passwordField = new PasswordField({
      panel: this,
      name: "password",
    });

    const fileField = new FileField({
      panel: this,
      label: "File field",
      img: download,
      message:
        "Escolha um arquivo de firmware para fazer o upload ou solte-o aqui",
      formats: [".bin", ".hex"],
      buttonMsg: "Atualizar",
      callback: () => {
        console.log(fileField);
      },
    });

    const buttonField = new ButtonField({
      panel: this,
      label: "Button field",
      img: factory,
      color: "gold",
      callback: () => {
        console.log(this.value);
        alert("hello from button");
      },
    });
  }
}

customElements.define("test1-panel", Test1Panel);