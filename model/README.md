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

# Install Modelica library
tee tmp.mos >/dev/null <<"EOF"
installPackage(Modelica)
EOF
omc tmp.mos
rm tmp.mos
```


## Run manual simulation
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


## Run Python tools
1. Create a virtual environment
  ```sh
  python3 -m venv venv
  . ./venv/bin/activate
  pip3 install -r requirements.txt
  ```
1. Run Python tools (linearization, etc.)
  ```sh
  make
  ```
