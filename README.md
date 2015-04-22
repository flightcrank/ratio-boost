# RATIO-BOOST

This program is intended to be used to obtain a good ratio on private torrent trackers.
It reports to the tracker that you are uploading data when you are not actually uploading anything at all.
This program was created due to the lack of ratio boosting programs for the linux operating system. Booting into windows or using
an emulator or virtual machine seems overkill for a simple program such as this.

This program is dependant on openssh and libcurl libs installed on your system

to compile with GCC

```
gcc list.c blex.c hash.c urle.c main.c -lcrypto -lcurl -o app
```

to compile GTK GUI version with GCC

```
gcc blex.c hash.c list.c urle.c gtk_main.c -export-dynamic -lcrypto -lcurl `pkg-config --cflags --libs gtk+-3.0` -o app
```

This program is still in early development

## Images (GTK version)
![spoofed ratio result](http://i.imgur.com/Jjrgj45.png)

## Images (console version)
![spoofed ratio result](http://i.imgur.com/0FCZTEt.png)

