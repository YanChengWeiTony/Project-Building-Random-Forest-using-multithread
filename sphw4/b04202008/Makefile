all:
	gcc hw4.c -lpthread -o hw4

run:
	./hw4 -data ../data -output ./submission.csv -tree 10000 -thread 100

inst:
	perf stat -e instructions:u -v ./hw4 -data ../data -output ./submission.csv -tree 100 -thread 100
