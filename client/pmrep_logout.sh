#!/bin/bash

vlan_id=230
wlan_balancer="http://127.0.0.1:8080/"

blame(){ echo $* >&2; }

check_deps(){
	deps=("jq" "curl" "create-ap" "sed")

	for dep in "${deps[@]}"; do 
		if ! command -v "$dep" >/dev/null; then
			blame "missing \"$dep\", cannot continue"
			exit 2
		fi
	done
}



main(){
	wlan_dev="$(ip l | sed -En '/ (wlp|wlan)/{ s/.*((wlp|wlan)[^:]+):.*/\1/p;q; }')"

	mac="$(ip l | sed -En "/ $wlan_dev/{N;s/.*ether (([a-zA-Z0-9]{2}:){5}[a-zA-Z0-9]{2}).*/\1/p;q}")"
	
	if [ -z "$wlan_dev" -o -z "$mac" ]; then
		blame "well, I could not find every information about the nics I need!"
		exit 1;
	fi
	
	echo "wlan_dev: $wlan_dev|$mac" 

	curl -s -d"{\"mac\":\"$mac\",\"login\":false}" "$wlan_balancer"
}

main $*
