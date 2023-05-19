import { addStyleSheet } from "../utils.js";
import styles from "./Header.css" assert { type: "css" };

import hamburger from "Images/hamburger.svg";
import logo from "Images/logo.svg";
import gear from "Images/gear.svg";

class Header extends HTMLElement {
  constructor(options) {
    // Always call super first in constructor
    super();

    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    this.wrapper = document.createElement("div");
    this.wrapper.classList.add("wrapper");

    const buttonLeft = (this.buttonLeft = document.createElement("div"));
    buttonLeft.classList.add("button");
    buttonLeft.classList.add("button-left");
    buttonLeft.style.setProperty("-webkit-mask-image", `url(${hamburger})`);
    buttonLeft.style.setProperty("mask-image", `url(${hamburger})`);

    const buttonCenter = (this.buttonCenter = document.createElement("div"));
    buttonCenter.classList.add("button");
    buttonCenter.classList.add("button-center");
    buttonCenter.style.setProperty("-webkit-mask-image", `url(${logo})`);
    buttonCenter.style.setProperty("mask-image", `url(${logo})`);

    const buttonRight = (this.buttonRight = document.createElement("div"));
    buttonRight.classList.add("button");
    buttonRight.classList.add("button-right");
    buttonRight.style.setProperty("-webkit-mask-image", `url(${gear})`);
    buttonRight.style.setProperty("mask-image", `url(${gear})`);

    this.wrapper.appendChild(buttonLeft);
    this.wrapper.appendChild(buttonCenter);
    this.wrapper.appendChild(buttonRight);

    this.shadow.appendChild(this.wrapper);

    if (options && options.container) {
      options.container.appendChild(this);
    }
  }
}

customElements.define("header-bar", Header);
export { Header };
