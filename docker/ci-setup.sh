#!/bin/sh
# Common CI setup

git config --global --add safe.directory /code
git config --global --add safe.directory '*'

cat >> /root/.bashrc <<"EOF"
echo 'WARNING: This is an internal CI container'
echo 'Do not use for production'
EOF
