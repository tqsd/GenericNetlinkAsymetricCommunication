
# Table of Contents

1.  [Description](#orgfcd398a)
2.  [Instructions](#orgc72f694)
    1.  [Automatic setup](#orgb73b514)
    2.  [Manual Setup](#orgc1cce9e)



<a id="orgfcd398a"></a>

# Description

Generic Netlink Asymetric Communication kernel module allows for different
processes to communicate through the netlink protocol.


<a id="orgc72f694"></a>

# Instructions


<a id="orgb73b514"></a>

## Automatic setup

A command line utility is provided in order to interact with the
kernel module.

Compile the module with specific netlink family name:

    

To load the last compiled module:

    

To unload the module with specific netlink family name:


<a id="orgc1cce9e"></a>

## Manual Setup

To compile the module:

    make

To load the module into the kernel:

    sudo insmod driver.ko

