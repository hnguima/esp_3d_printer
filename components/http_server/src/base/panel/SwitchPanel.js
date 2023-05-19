import { addStyleSheet } from "../utils";
import { Panel } from "./Panel";
import { Switch } from "../switch/Switch";
import styles from "./SwitchPanel.css";

class SwitchPanel extends Panel {
  constructor(options) {
    super(options);
    addStyleSheet(this.shadow, styles);

    const sw = (this.sw = new Switch());
    sw.checkbox.onchange = () => {
      this.checked = sw.value;
    };

    this.addField(sw, "enabled");

    setTimeout(() => {
      this.checked = (options && options.checked) || false;
    }, 0);
  }

  get value() {
    return super.value;
  }

  set value(val) {
    super.value = val;
    this.checked = this.sw.value;
  }

  get checked() {
    return this.sw.value;
  }

  set checked(val) {
    this.sw.value = val;

    this.wrapper.style.maxHeight = val
      ? this.wrapper.scrollHeight + 150 + "px"
      : "70px";
  }
}

customElements.define("switch-panel", SwitchPanel);
export { SwitchPanel };
