#NAME: YASH CHOUDHARY
#UID: 704630134
#EMAIL : YASHC@G.UCLA.EDU


build:
	gcc -pthread -o lab2_list lab2_list.c SortedList.c 
	gcc -pthread -o lab2_add lab2_add.c 
graphs:
	gnuplot ./lab2_add.gp
	gnuplot ./lab2_list.gp
clean:
	rm -rf lab2_add lab2_list *.csv *.png 
dist:
	tar -czf lab2a-704630134.tar.gz lab2_add.c lab2_list.c Makefile README lab2_add.csv lab2_list.csv lab2_add.gp lab2_list.gp test.sh SortedList.c SortedList.h *.png 



tests:
	./test.sh
	-./lab2_list --threads=2  --iterations=1              >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=10             >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=100            >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1000           >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1              >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=10             >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=100            >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1000           >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1              >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=10             >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=100            >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1000           >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1              >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=10             >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=100            >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1000           >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=i  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=i  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=d  >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=d  >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1   --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=il >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=il >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=il >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=il >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=1   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=2   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=4   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=8   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=16  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=2  --iterations=32  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=1   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=2   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=4   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=8   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=4  --iterations=16  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=1   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=2   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=4   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=8   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=8  --iterations=16  --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=1   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=2   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=4   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=8   --yield=dl >> lab2_list.csv
	-./lab2_list --threads=12 --iterations=16  --yield=dl >> lab2_list.csv
