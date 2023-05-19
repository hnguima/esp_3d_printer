import { SwitchPanel } from "Panel";
import { TextField, SwitchField } from "Field";

class TestPanel extends SwitchPanel {
  constructor() {
    // Always call super first in constructor
    super();

    this.title = "test panel";
    this.icon = "Images/eye.svg";
    this.checked = true;

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

    this.value = {
      enabled: false,
      test: "asdfasdf",
      test1: false,
      test2: true,
    };
  }
}

customElements.define("test-panel", TestPanel);
export { TestPanel };
