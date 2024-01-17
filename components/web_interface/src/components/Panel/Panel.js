import { h, Component, render } from "preact";
import { Switch } from "Components/switch/Switch.js";
import styles from "./Panel.module.css";

export function Panel({
  label = "test",
  icon,
  switchable = undefined,
  children,
}) {
  return (
    <div className={styles.panel}>
      <div className={styles.header}>
        <div className={styles.title}>
          <div
            className={styles.icon}
            style={`
          -webkit-mask-image: url(${icon});
          mask-image: url(${icon});`}
          ></div>
          <h2>{label}</h2>
        </div>
        {switchable ? <Switch /> : undefined}
      </div>
      {children}
    </div>
  );
}
