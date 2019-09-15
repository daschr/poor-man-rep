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

jarray(){ jq -Rsc 'split("\n") | map(select(length>0))';}


main(){
	if [ $UID != 0 ]; then
		blame "how may I set up a wlan network, when i don't have root privileges?!"
		exit 1
	fi

	ether_dev="$(ip l | sed -En '/ (enp|eth)/{ s/.*((enp|eth)[^:]+):.*/\1/p;q; }')"
	wlan_dev="$(ip l | sed -En '/ (wlp|wlan)/{ s/.*((wlp|wlan)[^:]+):.*/\1/p;q; }')"

	mac="$(ip l | sed -En "/ $wlan_dev/{N;s/.*ether (([a-zA-Z0-9]{2}:){5}[a-zA-Z0-9]{2}).*/\1/p;q}")"
	
	if [ -z "$ether_dev" -o -z "$wlan_dev" -o -z "$mac" ]; then
		blame "well, I could not find every information about the nics I need!"
		exit 1;
	fi
	
	echo "ether_dev: $ether_dev wlan_dev: $wlan_dev|$mac @ $HOSTNAME" >&2

	#now get get information wether we can be an ap or not...
	wlan_info="$(curl -s -d"{\"mac\":\"$mac\",\"login\":true}" "$wlan_balancer")"
	if ! jq <<<"$wlan_info"; then
		echo "no ap for us :("
		exit 0
	fi
	
	ssid="$(jq -r .ssid <<<"$wlan_info" )"
	password="$(jq -r .password <<<"$wlan_info" )"
	
	if [ -z "$ssid" -o -z "$password" ]; then
		blame "I need more information from the balancer :(" 
		exit 1
	fi
	
	echo "I'm creating the vlan interface now..."
	
	if ! ip l add link $ether_dev name vlan$vlan_id type vlan id $vlan_id; then
		blame "got an error while create vlan interface!"
		exit 1
	fi

	echo "ok, I will now start create_ap :)"
	
	create_ap --country DE -c $(( $RANDOM % 13 +1 )) -m bridge $wlan_dev vlan$vlan_id $ssid $password
	
	curl -s -d"{\"mac\":\"$mac\",\"login\":false}" "$wlan_balancer" >/dev/null
	ip l del vlan$vlan_id
}

main $*
