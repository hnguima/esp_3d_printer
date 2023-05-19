import { SwitchPanel } from "Panel";
import * as Field from "Field";

import dial from "Images/dial.svg";
import download from "Images/download.svg";
import factory from "Images/factory.svg";

class TestPanel2 extends SwitchPanel {
  constructor() {
    // Always call super first in constructor
    super({
      checked: true,
    });

    this.tab = "world";
    this.name = "test2";

    this.icon = dial;
    this.title = "new panel";

    const textField = new Field.TextField({
      panel: this,
      name: "text",
      label: "text test",
      placeholder: "placeholder",
    });

    const switchField = new Field.SwitchField({
      panel: this,
      name: "switch",
      label: "switch test",
    });

    // const passwordField = new PasswordField({
    //   panel: this,
    //   name: "password",
    // });

    const fileField = new Field.FileField({
      panel: this,
      name: "file",
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

    const buttonField = new Field.ButtonField({
      panel: this,
      name: "button",
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

customElements.define("test-panel2", TestPanel2);
export { TestPanel2 };
