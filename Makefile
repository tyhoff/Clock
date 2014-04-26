all:
	P_DEV=true pebble build
	pebble install --logs
