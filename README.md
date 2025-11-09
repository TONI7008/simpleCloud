# RemoteFileStore

**RemoteFileStore** is a desktop application built using **Qt and C++** that allows users to store and retrieve files on a remote computer over a network. The app supports both **Server** and **Client** modes and is fully GUI-based using Qt Widgets.

---

## 🚀 Features

- 📁 Upload and download files over LAN
- 🔒 Basic authentication for access control
- 💻 Cross-platform support (Windows, Linux, macOS)
- 📦 Lightweight and dependency-free (except Qt)

---

## 🖼️ Screenshots
<div align="center">
  <a href="https://github.com/TONI7008/simpleCloud/blob/main/screenshots/Screenshot_20250529_161654.png">
    <img src="https://github.com/TONI7008/simpleCloud/blob/main/screenshots/Screenshot_20250529_161654.png" alt="Screenshot 1" width="400" style="margin: 10px;"/>
  </a>
  <br>
  <a href="https://github.com/TONI7008/simpleCloud/blob/main/screenshots/Screenshot_20250529_161825.png">
    <img src="https://github.com/TONI7008/simpleCloud/blob/main/screenshots/Screenshot_20250529_161825.png" alt="Screenshot 2" width="400" style="margin: 10px;"/>
  </a>
  <br>
  <a href="https://github.com/TONI7008/simpleCloud/blob/main/screenshots/simplecloudv00.png">
    <img src="https://github.com/TONI7008/simpleCloud/blob/main/screenshots/simplecloudv00.png" alt="Screenshot 3" width="400" style="margin: 10px;"/>
  </a>
</div>
---

## 🛠️ Build Instructions

### Prerequisites

- Qt 5 or Qt 6 (tested with Qt 6.5+)
- CMake (or qmake)
- C++17 or newer

### Using CMake

```bash
mkdir build
cd build
cmake ..
make
