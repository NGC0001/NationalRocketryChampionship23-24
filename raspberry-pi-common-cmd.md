# ssh from usb
* command: `ssh pi@raspberrypi.local`
* password: raspberry

# shutdown pi from commandline
`sudo poweroff`

# manipulate network interfaces
* list: `ip link show`
* shutdown: `sudo ip link set wlan0 down`
* start: `sudo ip link set wlan0 up`

# wifi config file
for `wpa_supplicant`: `/etc/wpa_supplicant/wpa_supplicant.conf`
