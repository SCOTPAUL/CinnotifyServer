# Cinnotify Server
Server software for the [Cinnotify Android app](https://github.com/SCOTPAUL/Cinnotify). Mirrors notifications received on an Android device onto a Linux device with a [libnotify](https://developer.gnome.org/libnotify/) compatible notification server installed (e.g. Cinnamon/GNOME).

## Building on Linux Mint
### Install dependencies

```bash
sudo apt-get install libgtk2.0-dev libnotify-dev
```

### Compile

```bash
cd CinnotifyServer/
make
```

### Run

```bash
./cinnotify-server
```

## TODO
- Implement end to end encryption to prevent interception of potentially private notifications over public networks.
- Receive app icon images and display these to tell the user which app the notification came from.
- Improve notification parser to allow special characters in notifications.
- Allow the user to manually set the port number with a flag.
