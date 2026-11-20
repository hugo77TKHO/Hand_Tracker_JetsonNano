# Jetson Hand Tracker - Contrôle IoT par Vision par Ordinateur

Système de détection et de comptage de doigts en temps réel exécuté sur **NVIDIA Jetson Nano** à l'aide de **MediaPipe** et **OpenCV**. 
Le nombre de doigts détectés (de 0 à 9) pilote simultanément un **écran LCD I2C** connecté au Jetson Nano ainsi que **9 LED** et un **afficheur 7 segments** 
reliés à un **ESP32** via le protocole **UDP**.

---

# Architecture du système

```text
[Caméra] ──> [Jetson Nano : OpenCV + MediaPipe]
                  │
                  ├── (Bus I2C) ──> Écran LCD 16x2 (Nombre de LED allumées / éteintes)
                  │
                  └── (Wi-Fi / UDP : port 4210) ──> [ESP32]
                                                       ├── Afficheur 7 segments (Chiffre 0 à 9)
                                                       └── Barre de 9 LED
