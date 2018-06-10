# moonrise

WIP Implementation of Moonlight for the Nintendo Switch

Based on [moonlight-embedded](https://github.com/irtimmer/moonlight-embedded)

## Dependencies :
 
**Devkitpro repo :**
 - switch-pkg-config
 - switch-curl
 - switch-zlib
 - gcc (don't scoff, it's the _host_ gcc, needed to build other packages)

**From https://github.com/Stary2001/switch-pkgs:**
 - switch-ffmpeg (ffmpeg)
 
**From https://github.com/natinusala/switch-pkgs:**
 - switch-libexpat (libexpat)
 - switch-enet (enet)
 - switch-wolfssl (wolfssl) (add [this file](http://unix.superglobalmegacorp.com/BSD4.4/newsrc/sys/un.h.html) to libnx includes directory under `sys/`)
 - switch-libuuid (libuuid)