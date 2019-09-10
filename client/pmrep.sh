#!/bin/bash

list="$(ip l | sed -En '/ (wlan|wlp)/{N;s/.*ether (([a-zA-Z0-9]{2}:){5}[a-zA-Z0-9]{2}).*/\1/p;}')"

