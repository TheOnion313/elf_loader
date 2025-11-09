.DEFAULT_GOAL := reset
clean: 
	rm -f ./bin/*

readelf: 
	gcc -w ./utils/readelf.c ./consts/value_tables.c -o ./bin/readelf

reset: clean readelf

run: reset
	./bin/readelf -S /bin/ls

test: run
	readelf -S /bin/ls
