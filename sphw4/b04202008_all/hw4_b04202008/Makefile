all:
	gcc hw4.c -lpthread -o hw4
	gcc check.c -o check
clean:
	rm bidding_system_log
	rm customer_log

run:
	./hw4 -data ../data -output ./submission.csv -tree 10000 -thread 100
	./check submission.csv ans.csv

inst:
	perf stat -e instructions:u -v ./hw4 -data ../data -output ./submission.csv -tree 100 -thread 100
	./check submission.csv ans.csv
