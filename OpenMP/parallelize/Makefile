all: build

build: 4.c 5.c 6.c p4.c p5.c p6.c
	g++ 4.c -o run4
	g++ -fopenmp p4.c -o prun4
	g++ 5.c -o run5
	g++ -fopenmp p5.c -o prun5
	g++ 6.c -o run6
	g++ -fopenmp p6.c -o prun6

run: run4 run5 run6

run4: run4 prun4
	./run4
	./prun4

run5: run5 prun5
	./run5
	./prun5	

run6: run6 prun6
	./run6
	./prun6

clean:
	rm -rf *.out
	rm ./run*
	rm ./prun*

