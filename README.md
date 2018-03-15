
# RATIO-BOOST

This program is intended to be used to obtain a good ratio on private torrent trackers.
It reports to the tracker that you are uploading data when you are not actually uploading anything at all.
This program was created due to the lack of ratio boosting programs for the Linux operating system. Booting into windows or using
an emulator or virtual machine seems overkill for a simple program such as this.

this program can also spoof downloads but that functionality isn't included for now, as it is not needed. the program will report to the tracker
that 100% of the file has already been downloaded.

This program is dependent on GTK and libcurl libs installed on your system as well as posix regex.h. It should compile fine on Windows and Mac. however is up to the user
to install these dependency's and follow compilation instructions for those operating systems

the compilation instructions with GCC on a Linux system are as follows...

    gcc blex.c hash.c list.c urle.c gtk_main.c -lcurl -export-dynamic `pkg-config gtk+-3.0 --cflags --libs` -o ratio-boost

This program has achieved its goal, and is no longer under active devleopment. However minor improvments may still be made

## Images (GTK version)
![animation](https://i.imgur.com/MDSNu3Q.gif)

![spoofed ratio result](https://i.imgur.com/ApvQTT2.png)


