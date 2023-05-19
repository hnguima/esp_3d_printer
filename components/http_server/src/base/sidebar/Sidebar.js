import { addStyleSheet } from "../utils.js";
import { SidebarTab } from "./SidebarTab.js";
import styles from "./Sidebar.css";

class Sidebar extends HTMLElement {
  tabs = {};
  constructor() {
    // Always call super first in constructor
    super();

    this.shadow = this.attachShadow({ mode: "open" });
    addStyleSheet(this.shadow, styles);

    const wrapper = (this.wrapper = document.createElement("div"));
    wrapper.classList.add("wrapper");

    this.shadow.appendChild(wrapper);

    this._tabs = {};
    this._activeTab;
  }

  appendTab(tabName, tabIcon, container, active) {
    if (!this._tabs[tabName]) {
      this._tabs[tabName] = new SidebarTab();
      this._tabs[tabName].name = tabName;
      this._tabs[tabName].icon = tabIcon;
      this._tabs[tabName].container = container;
    }

    this._tabs[tabName].onclick = () => {
      this.activeTab = tabName;
    };

    this.wrapper.appendChild(this._tabs[tabName]);

    if (active) {
      this.activeTab = tabName;
    }
  }

  hide() {
    this.wrapper.classList.toggle("closed");
  }

  get activeTab() {
    return this._activeTab;
  }

  set activeTab(name) {
    if (this._activeTab && this._activeTab.name === name) {
      return;
    }

    for (const tab in this._tabs) {
      this._tabs[tab].active = this._tabs[tab].name === name;
    }
    this._activeTab = this._tabs[name];
  }

  // set container(container) {
  //   this._container = container;

  //   let observer = new MutationObserver((mutationRecords) => {
  //     mutationRecords.forEach((element) => {
  //       element.addedNodes;
  //     });
  //   });

  //   observer.observe(this._container, {
  //     childList: true, // observe direct children
  //   });
  // }

  // #sidebarChangeTab(nodes) {
  //   nodes.forEach((node) => {
  //     console.log(this.activeTab.name);
  //     if (this.activeTab.name !== node.tab) {
  //       node.setAttribute("hidden", true);
  //       console.log(node.tab);
  //     }
  //   });
  // }
  // observe everything except attributes
}

customElements.define("side-bar", Sidebar);
export { Sidebar };
