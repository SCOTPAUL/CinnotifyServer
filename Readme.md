# Cinnotify Server
Server software for the [Cinnotify Android app](https://github.com/SCOTPAUL/Cinnotify). Mirrors notifications received on an Android device onto a Linux device with a [libnotify](https://developer.gnome.org/libnotify/) compatible notification server installed (e.g. Cinnamon/GNOME).

## Downloading

Download the source from Github with

```bash
git clone --recursive https://github.com/SCOTPAUL/CinnotifyServer.git
```

## Building on Linux Mint
### Install dependencies

```bash
sudo apt-get install libgtk2.0-dev libnotify-dev libgtk2.0-dev
```

If you did not include the `--recursive` flag when cloning, you also have to do the following.

```bash
cd lib/
git clone https://github.com/DaveGamble/cJSON.git
```

Alternatively, cJSON can be installed from elsewhere.

### Compile and install

```bash
cd CinnotifyServer/
make
sudo make install
```

### Running on startup
Edit `/etc/rc.local`, adding `cinnotify-server -s &` before `exit 0`.

### Usage

#### Default

```bash
cinnotify-server
```

#### Silent

Redirects `stderr` and `stdout` to allow running silently in the background.

```bash
cinnotify-server -s
```

#### Custom Port

If the Cinnotify app is using a port other than 6525 (the default), the listening port can be set to a custom value.

```bash
cinnotify-server -p [PORT]
```

## TODO
- Implement end to end encryption to prevent interception of potentially private notifications over public networks.
- Receive app icon images and display these to tell the user which app the notification came from.
- ~~Improve notification parser to allow special characters in notifications.~~
- ~~Allow the user to manually set the port number with a flag.~~
