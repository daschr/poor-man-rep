#!/bin/bash

check_deps(){
	deps=("jq" "ncat" "create-ap" "sed")

	for dep in "${deps[@]}"; do 
		if ! command -v "$dep" >/dev/null; then
			echo "missing \"$dep\", cannot continue" >&2;
			exit 2
		fi
	done
}

jarray(){ jq -Rsc 'split("\n") | map(select(length>0))';}

main(){
	if [ $UID != 0 ]; then
		echo "how may scan for wlan networks, when i don't have root privileges?!" >&2
		exit 1
	fi

	mac="$(ip l | sed -En '/ (wlan|wlp)/{N;s/.*ether (([a-zA-Z0-9]{2}:){5}[a-zA-Z0-9]{2}).*/\1/p;q}')"
	

}

main $*
