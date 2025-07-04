# Background Apps applet

This is a simple applet that shows XDG Desktop Background Apps running on your system.

Currently, mostly GNOME apps (e.g. Amberol) use that protocol. This applet allows users
to see these applications and kill them if needed.

## Installation

```bash
git clone https://github.com/jinliu/plasma-applet-background-apps.git
cd plasma-applet-background-apps
cmake -B build -S .
cmake --build build
sudo cmake --install build
systemctl --user restart plasma-plasmashell
```

Now you can enable "Background Apps" in the system tray settings.

NOTE: The above installs files to `/usr`, because this applet contains C++ code and Plasma doesn't
look in your $HOME or `/usr/local` for it. 

### Uninstall

```
cd plasma-applet-background-apps/build
sudo make uninstall
```

## Screenshots

![Screenshot_20250507_041228](https://github.com/user-attachments/assets/fa34e633-09ef-4940-9527-12af3077deab)

## Known Problems

It only works with Flatpak apps. This is a limitation in the XDG desktop portal, not in this applet.
