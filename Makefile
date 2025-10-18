.DEFAULT_GOAL := reset
clean: 
	rm -f ./bin/*

readelf: 
	gcc -w ./utils/readelf.c ./consts/value_tables.c -o ./bin/readelf

reset: clean readelf

run: reset
	./bin/readelf -l /bin/ls

test: run
	readelf -l /bin/ls
