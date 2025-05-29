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

Here's our logo (hover to see the title text):

Inline-style: 
![alt text](https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 1")

Reference-style: 
![alt text][logo]

[logo]: https://github.com/adam-p/markdown-here/raw/master/src/common/images/icon48.png "Logo Title Text 2"
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
