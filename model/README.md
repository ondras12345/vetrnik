# Vetrnik model
This directory contains Modelica models for software-in-the-loop simulations.


## Install
I am running the simulations with OpenModelica on Ubuntu 24.04:
```sh
sudo apt-get update
sudo apt-get install ca-certificates curl gnupg
sudo curl -fsSL http://build.openmodelica.org/apt/openmodelica.asc | sudo gpg --dearmor -o /etc/apt/keyrings/openmodelica.gpg

sudo tee /etc/apt/sources.list.d/openmodelica.sources >/dev/null <<"EOF"
Types: deb
URIs: https://build.openmodelica.org/apt
Suites: noble
Components: stable
Architectures: amd64
Signed-By: /etc/apt/keyrings/openmodelica.gpg
EOF

sudo apt update
sudo apt install openmodelica

# OMPython does not work: libicuuc.so.74 is too new
wget https://launchpad.net/ubuntu/+source/icu/70.1-2/+build/23145450/+files/libicu70_70.1-2_amd64.deb
# extract it
# copy usr/lib/x86_64-linux-gnu to /usr/local/lib/x86_64-linux-gnu
# sudo cp -d * /usr/local/lib/x86_64-linux-gnu/
sudo ldconfig
```


## Run
1. Build LISP controller:
  ```sh
  cd firmware/vetrnik-control/lisp
  make
  ```
1. Build `libvetrniksil`:
  ```sh
  cd firmware/vetrnik-sil
  cmake .
  make
  ```
1. Open `Vetrnik/package.mo` in OMEdit, choose a model to run and start
   simulation.
