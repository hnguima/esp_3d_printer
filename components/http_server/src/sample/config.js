import { Page } from "Page";
import { addTooltip } from "Tooltip";
import * as Panels from "./panels";

import { addStyleSheet } from "Base/utils";
import colors from "Styles/colors.css";
import fonts from "Styles/fonts.css";

import info from "Images/info.svg";
import web from "Images/web.svg";
import dial from "Images/dial.svg";

import config from "./config.json";

addStyleSheet(document.body, colors);
addStyleSheet(document.body, fonts);

document.body.classList.add("light-mode");

// const header = new Header({ container: document.body });
const page = new Page({
  container: document.body,
  configFile: detaultConfig,
});

page.header.buttonCenter.addEventListener("click", (e) => {
  location.href = "index.html";
});

const tabs = {
  info: {
    container: page.addContainer(),
    img: info,
    active: true,
  },
  world: {
    container: page.addContainer(),
    img: web,
  },
  asdf: {
    container: page.addContainer(),
    img: dial,
  },
};

for (const tab in tabs) {
  page.sidebar.appendTab(
    tab,
    tabs[tab].img,
    tabs[tab].container,
    tabs[tab].active
  );
}

for (const panel in Panels) {
  const newPanel = new Panels[panel]();

  if (tabs[newPanel.tab]) {
    tabs[newPanel.tab].container.appendChild(newPanel);
  }else{ 
    delete window.newPanel
  }
}
