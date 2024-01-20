# micrOS
## About
The *micrOS* operating system is an operating system aiming for POSIX compatibility for *amd64* PC compatibles.
## Building
To build *micrOS*, you'll need to be on a UNIX-like system that consists of a toolchain capable of producing `amd64` binaries. You'll also need a program such as `GNU Make` or `BSD Make` that is capable of executing UNIX-style makefiles. You'll also need *Limine* bootloader set up for creating the *micrOS* CDROM image, as well as *xorriso*.
After reaching the requirements, run the following command only once:
```sh
make limine
```
And for everytime you want to (re)build *micrOS* run this command:
```sh
make 3RDPARTY=false -j$(nproc)
```
## Testing
To test *micrOS*, you can use an emulator such as QEMU. On a UNIX system, you can run these different commands for testing *micrOS*:
```sh
# Use this to normally run it on an emulator
make run

# Use this to run it on an emulator with KVM acceleration
make run-kvm
```
If you're testing on WSL, you'll want to run it this way:
```sh
qemu-system-x86_64.exe -cdrom $(wslpath -a -w image.iso)
```