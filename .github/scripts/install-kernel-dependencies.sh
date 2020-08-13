#!/bin/bash
# Install dependencies to build kernel modules on Ubuntu runners
sudo apt-get update
sudo apt-get install -y linux-headers-$(uname -r)
