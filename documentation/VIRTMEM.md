# Virtual memory
## About
This page is to document virtual memory in amd64, specifically just notes I take from the Intel SDM about paging.
## Note on paging with Limine bootloader
Limine already sets up paging for us, mapping the kernel at a higher half, and mapping physical RAM to a specific virtual address, with it being Limine's HHDM offset. Therefore, we don't actually need to enable paging, as it's already enabled.
## What is paging?
Paging is a mechanism that involves amd64's MMU. It allows us to have a virtual address space, where we can map RAM (memory) and other types of memory such as storage devices & other I/O devices to the virtual address space.