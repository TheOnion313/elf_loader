.DEFAULT_GOAL := reset
clean: 
	rm -f ./bin/*

readelf: 
	gcc ./utils/readelf.c -o ./bin/readelf

reset: clean readelf

test: reset
	./bin/readelf /bin/ls
	readelf -h /bin/ls