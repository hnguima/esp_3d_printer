import { Panel } from "Panel";
import { SwitchField, PasswordField, TextField } from "Field";

import eye from "Images/eye.svg";

class TestPanel extends Panel{
  constructor() {
    // Always call super first in constructor
    super({
      checked: true,
    });

    this.tab = "info";
    this.name = "test1";

    this.title = "test panel";
    this.icon = eye;
    this.enabled = true;

    const textfield = new TextField();
    textfield.label = "teste text";
    textfield.placeholder = "aaaaaaaaaaasdf";

    const switchfield = new SwitchField();
    switchfield.label = "teste text2";

    const textfield2 = new SwitchField();
    textfield2.label = "teste aaa";

    this.addField(textfield, "test");
    this.addField(switchfield, "test1");
    this.addField(textfield2, "test2");
  }
}

customElements.define("test-panel", TestPanel);
export { TestPanel };
