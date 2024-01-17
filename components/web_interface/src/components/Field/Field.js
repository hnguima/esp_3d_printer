import { h, Component, render } from "preact";
import styles from "./Field.module.css";

export function Field({ label, placeholder }) {
  return (
    <div className={styles.field}>
      <h2 className={styles.label}>{label}</h2>
      <input className={styles.data} placeholder={placeholder}></input>
    </div>
  );
}
