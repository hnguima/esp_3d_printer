import styles from "./Tooltip.css";
import { addStyleSheet } from "../utils";

export class Tooltip extends HTMLElement {
  constructor() {
    // Always call super first in constructor
    super();
    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    this.wrapper = document.createElement("div");
    this.wrapper.setAttribute("class", "wrapper");

    const message = (this.message = document.createElement("div"));
    message.className = "message";

    const img = (this.img = document.createElement("div"));
    img.className = "img";
    const label = (this.label = document.createElement("span"));
    label.className = "label";

    const tip = (this.tip = document.createElement("div"));
    tip.setAttribute("class", "tip");

    document.addEventListener("mousemove", (e) => {
      this.x = e.clientX;
      this.y = e.clientY;
    });

    document.addEventListener(
      "touchmove",
      (e) => {
        var touch = e.touches[0] || e.changedTouches[0];
        this.x = touch.pageX;
        this.y = touch.pageY;
      },
      { passive: true }
    );

    // document.addEventListener("touchstart", (e) => {
    //   var touch =
    //     e.touches[0] || e.changedTouches[0];
    //   this.x = touch.pageX;
    //   this.y = touch.pageY;
    // });

    message.appendChild(img);
    message.appendChild(label);
    message.appendChild(tip);
    this.wrapper.appendChild(message);
    this.shadow.appendChild(this.wrapper);
  }

  show(options) {
    if (options && options.img) {
      this.img.style.display = "inline-block";
      this.img.style.setProperty("-webkit-mask-image", `url(${options.img})`);
      this.img.style.setProperty("mask-image", `url(${options.img})`);
    } else {
      this.img.style.display = "none";
    }

    if (options && options.msg) {
      this.label.style.display = "inline-block";
      this.label.innerHTML = options.msg;
    } else {
      this.img.style.display = "none";
    }

    const wrapperRect = this.message.getBoundingClientRect();

    this.message.style.left =
      Math.min(
        Math.max(this.x - wrapperRect.width / 2 + 2, 10),
        document.documentElement.clientWidth - (wrapperRect.width + 10)
      ) + "px";

    if (this.y < wrapperRect.height + 20) {
      this.message.style.top = this.y + 20 + "px";
      // this.tip.style.top = "auto";
      this.tip.style.top = this.y + 7 + "px";
      this.tip.style.transform = "scaleY(-1)";
    } else {
      this.message.style.top = this.y - (wrapperRect.height + 20) + "px";
      this.tip.style.top = this.y - 22 + "px";
      this.tip.style.transform = "scaleY(1)";
    }

    this.tip.style.left =
      Math.min(
        Math.max(this.x - 10, 20),
        document.documentElement.clientWidth - 45
      ) + "px";

    this.message.classList.remove("hide");
    this.message.classList.add("show");
  }

  hide() {
    this.message.classList.add("hide");
    this.message.classList.remove("show");
  }
}

customElements.define("tool-tip", Tooltip);

const tooltip = new Tooltip();
document.body.appendChild(tooltip);

export function addTooltip(options) {
  try {
    if (!options) {
      throw new Error("Options argument must be provided");
    }
    if (!options.element) {
      throw new Error("options.element must be provided");
    }

    var timeout;

    function startHandler() {
      if (!options.disabled) {
        if (timeout != null) {
          clearTimeout(timeout);
        }

        timeout = setTimeout(function () {
          tooltip.show(options);
        }, 500);
      }
    }

    function endHandler() {
      if (timeout != null) {
        clearTimeout(timeout);
        timeout = null;
      }
      tooltip.hide();
    }

    options.element.addEventListener("mouseenter", startHandler, false);
    options.element.addEventListener("touchstart", startHandler, {
      passive: true,
    });
    options.element.addEventListener("mouseleave", endHandler, false);
    options.element.addEventListener("touchend", endHandler, { passive: true });
  } catch (error) {
    console.error(error);
  }
}
