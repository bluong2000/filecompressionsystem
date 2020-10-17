all: fileCompressor.c
	gcc -o fileCompressor fileCompressor.c
clean: 
	rm fileCompressor

debug: fileCompressor.c
	gcc -g -o fileCompressor fileCompressor.c

testAll: fileCompressor
	./fileCompressor -R -c ./path ./path/HuffmanCodebook; ./fileCompressor -R -d ./path ./path/HuffmanCodebook

test-R-c: fileCompressor
	./fileCompressor -R -c ./path ./path/HuffmanCodebook

test-R-d: fileCompressor
	./fileCompressor -R -d ./path ./path/HuffmanCodebook

val-R-c: fileCompressor
	valgrind --leak-check=full ./fileCompressor -R -c ./path ./path/HuffmanCodebook

val-R-d: fileCompressor
	valgrind --leak-check=full ./fileCompressor -R -d ./path ./path/HuffmanCodebook

