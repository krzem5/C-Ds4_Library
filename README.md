# C - Ds4 Library

## Linux Setup

Required libraries:

```
apt install -y libudev-dev
```

Write the following code in `/etc/X11/xorg.conf.d/ds4.conf`:

```
Section "InputClass"
    Identifier "ds4-touchpad"
    Driver "libinput"
    MatchProduct "Wireless Controller Touchpad"
    Option "Ignore" "True"
EndSection
```

Write the following code in `/etc/udev/rules.d/99-ds4-hidraw.rules`:

```
KERNEL=="hidraw*", SUBSYSTEM=="hidraw", MODE="0664", GROUP="ds4_hidraw"
```

Add all required users to the group `ds4_hidraw`: `usermod -a -G ds4_hidraw <username>`

Reboot to apply all changes.

## Linux bluetooth pairing process

Hold Logo + Share and wait for connection.
