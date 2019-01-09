Watchpoint kernel module for Yocto
========

Wathpoint kernel module test task for Yocto. 
Comes with tester kernel module.

## Compiling & Installation

First you should install Yocto, switch to morty branch and setup build environments in new workspace:

```bash
$ git clone http://git.yoctoproject.org/git/poky
$ cd poky
$ git checkout -b morty origin/morty
$ source oe-init-build-env workspace
```
Then we will import example layer (it should be created in poky/):
```bash
$ cd ..
$ git clone https://github.com/nevmnd/meta-example.git
```
Recipe that fetches and builds module should be in poky/meta-example/recipes-kernel/wp-mod/watchpoint_git.
Open poky/workspace/conf/bblayers.conf and add meta-example to layers list. It should be something like that:
```
...
BBLAYERS ?= " \
  <workdir>/poky/meta \
  <workdir>/poky/meta-poky \
  <workdir>/poky/meta-yocto-bsp \
  <workdir>/poky/meta-example \
  "
  ```
Now as we added our layer, we can bitbake core-image-minimal with our module built-in (additional recipe in meta-example allows us to do that). Check that bitbake is run from poky/workspace.
```
$ cd ..
$ bitbake core-image-minimal
```
## Module testing

Run qemu to check the result of bitbake, using login data:
User: root, password: 123.
```
$ runqemu qemux86
```
There are two modules in this project: watchpoint.ko and test_wp.ko. 
watchpoint.ko get an address of memory as a parameter and when this memory area read or written, it writes appropriate message to system log. Another way to pass address to module is to write it to /sys/kernel/modules/watchpoint/address.
test_wp.ko run with no parameters. It has internal variable "count", that ticks every 20 seconds. After start module writes appropriate message with address of "count" to system log.
Now let's start test_wp:
```
$ insmod /lib/modules/kernel/extra/test_wp.ko
```
Output of qemu should say something like that:
```
Loading test_wp module...
Counter address: 0x11111111
```
Now we'll load watchpoint module:
```
$ insmod /lib/modules/kernel/extra/watchpoint.ko address = 0x11111111
```
When "count" ticks, watchpoint should give a message:
```
0x11111111 value is changed
```
We can check that watchpoint module works fine by time marks in system log.
Let's unload test_wp and load it again:
```
$ rmmod test_wp
$ insmod /lib/modules/kernel/extra/test_wp.ko
```
Variable "count" got another address. We will pass this address to watchpoint module:
```
$ echo "0x11111112" > /sys/kernel/watchpoint/address
```
If test run right, unload modules and shut down machine:
```
$ rmmod watchpoint
$ rmmod test_wp
$ shutdown now
```
