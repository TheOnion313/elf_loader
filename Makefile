.DEFAULT_GOAL := reset
clean: 
	rm -f ./bin/*

readelf: 
	gcc ./utils/readelf.c ./consts/value_tables.c -o ./bin/readelf

reset: clean readelf

test: reset
	./bin/readelf /bin/ls
	readelf -h /bin/ls