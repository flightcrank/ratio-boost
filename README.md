
# RATIO-BOOST

This program is intended to be used to obtain a good ratio on private torrent trackers.
It reports to the tracker that you are uploading data when you are not actually uploading anything at all.
This program was created due to the lack of ratio boosting programs for the linux operating system. Booting into windows or using
an emulator or virtual machine seems overkill for a simple program such as this.

this program can also spoof downloads but that functionality isnt included for now, as it is not needed. the program will report to the tracker
that 100% of the file has already been downloaded.

This program is dependant on GTK, openssh and libcurl libs installed on your system

BUGS:
there are some torrent files which trip up the program when calculating the info hash. I am not sure why. but it works with most torrent files.
if you find one that trips it up, just locate other ones that do work to spoof your ratio.

to compile GTK GUI version with GCC

    gcc blex.c hash.c list.c urle.c gtk_main.c -lcrypto -lcurl -export-dynamic `pkg-config gtk+-3.0 --cflags --libs` -o app

This program is still under development

## Images (GTK version)
![animation](https://i.imgur.com/uY6znXG.gif)

![spoofed ratio result](https://i.imgur.com/ApvQTT2.png)


