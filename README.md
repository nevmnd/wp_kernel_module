Watchpoint kernel module for Yocto
========

Wathpoint kernel module test task for Yocto

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


