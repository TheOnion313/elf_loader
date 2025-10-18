.DEFAULT_GOAL := reset
clean: 
	rm -f ./bin/*

readelf: 
	gcc -w ./utils/readelf.c ./consts/value_tables.c -o ./bin/readelf

reset: clean readelf

test: reset
	./bin/readelf -h /bin/ls
	readelf -h /bin/ls
