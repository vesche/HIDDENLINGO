# HIDDENLINGO

macOS offensive memory forensics research

## Dump memory with osxpmem

```
$ unzip osxpmem-2.1.post4.zip
$ sudo chown -R root:wheel osxpmem.app/MacPmem.kext/
$ sudo kextutil -t osxpmem.app/MacPmem.kext/
$ # go into System Preferences -> Security & Privacy -> Allow Kernel Extension (system will need to reboot)
$ osxpmem.app/osxpmem --help
$ # go into System Preferences -> Security & Privacy -> Allow osxpmem
$ sudo osxpmem.app/osxpmem -o dump.aff4
$ osxpmem.app/osxpmem -e /dev/pmem -o dump.raw dump.aff4
```
