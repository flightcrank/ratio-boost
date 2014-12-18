# RATIO-SPOOF

This program is intended to be used to obtain a good ratio on private torrent trackers.
It reports to the tracker that you are uploading data when you are not actually uploading anything at all.

This program is dependant on openssh and libcurl libs installed on your system

to compile with GCC

`gcc list.c blex.c hash.c urle.c main.c -lcrypto -lcurl -Wall -g -o app`

This program is in early development and has bugs

## Images
![spoofed ratio result](http://i.imgur.com/0FCZTEt.png)

