import { h, Component, render } from "preact";
import styles from "./Switch.module.css";

export function Switch() {
  return (
    <label className={styles.switch}>
      <input type="checkbox"></input>
      <div className={styles.slider}></div>
    </label>
  );
}
