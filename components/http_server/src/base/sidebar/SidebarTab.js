import { addStyleSheet } from "../utils.js";
import styles from "./SidebarTab.css";

class SidebarTab extends HTMLElement {
  // active = false;
  constructor() {
    // Always call super first in constructor
    super();

    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    const wrapper = (this.wrapper = document.createElement("div"));
    wrapper.classList.add("wrapper");

    const icon = (this._icon = document.createElement("div"));

    const name = (this._name = document.createElement("h3"));

    this.wrapper.appendChild(icon);
    this.wrapper.appendChild(name);
    this.shadow.appendChild(wrapper);

    this._active = false;
    this._container;
  }

  get name() {
    return this._name.innerHTML;
  }
  set name(name) {
    this._name.innerHTML = name;
  }

  get container() {
    return this._container;
  }
  set container(container) {
    this._container = container;
  }

  set icon(icon) {
    this._icon.style.setProperty("-webkit-mask-image", `url(${icon})`);
    this._icon.style.setProperty("mask-image", `url(${icon})`);
  }

  get active() {
    return this._active;
  }

  set active(active) {
    if (active) {
      this.wrapper.classList.add("active");
      this._container.classList.remove("hidden");
    } else {
      this.wrapper.classList.remove("active");
      this._container.classList.add("hidden");
    }
  }
}

customElements.define("side-bar-tab", SidebarTab);
export { SidebarTab };
