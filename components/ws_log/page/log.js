import { Page } from "Page";
import { addTooltip } from "Tooltip";

import { addStyleSheet } from "Base/utils";
import colors from "Styles/colors.css";
import fonts from "Styles/fonts.css";

import info from "Images/info.svg";
import web from "Images/web.svg";
import dial from "Images/dial.svg";

import { LogPanel } from "./panels";

addStyleSheet(document.body, colors);
addStyleSheet(document.body, fonts);

document.body.classList.add("light-mode");

// const header = new Header({ container: document.body });
const page = new Page({
  container: document.body,
});

page.header.buttonCenter.addEventListener("click", (e) => {
  location.href = "index";
});

page.header.buttonRight.addEventListener("click", (e) => {
  location.href = "config";
});

const container = page.addContainer();
container.appendChild(new LogPanel({
  title: "Log"
}));
