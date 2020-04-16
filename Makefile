build:
	g++ -std=c++11 -Wall -g test/personal_test_suite.cpp -o Personal
	g++ -std=c++11 -Wall -g test/serialize_tests.cpp -o Serialize
	g++ -std=c++11 -Wall -g test/m2.cpp -o m2
	g++ -std=c++11 -Wall -g -pthread test/m3.cpp -o m3
	g++ -std=c++11 -Wall -g -pthread test/m4.cpp -o m4

personal_tests: 
	g++ -std=c++11 -Wall -g test/personal_test_suite.cpp -o Personal
	./Personal

serialize_tests: 
	g++ -std=c++11 -Wall -g test/serialize_tests.cpp -o Serialize
	./Serialize

milestone2:
	g++ -std=c++11 -Wall -g test/m2.cpp -o m2
	./m2

milestone3:
	g++ -std=c++11 -Wall -g -pthread test/m3.cpp -o m3
	./m3

milestone4:
	docker build -t cs4500:0.1 .
	docker run -ti -v "$$(pwd):/code" cs4500:0.1 bash -c "cd code; g++ -std=c++11 -Wall -g -pthread test/m4.cpp -o m4; ./m4 data/long.txt"

valpersonal:
	docker build -t cs4500:0.1 .
	docker run -ti -v "$$(pwd):/src" cs4500:0.1 bash -c "cd src; g++ -std=c++11 -Wall -g test/personal_test_suite.cpp -o Personal; valgrind  --leak-check=full --track-origins=yes ./Personal"

valserialize:
	docker build -t cs4500:0.1 .
	docker run -ti -v "$$(pwd):/src" cs4500:0.1 bash -c "cd src; g++ -std=c++11 -Wall -g test/serialize_tests.cpp -o Serialize; valgrind  --leak-check=full --track-origins=yes ./Serialize"

val2:
	docker build -t cs4500:0.1 .
	docker run -ti -v "$$(pwd):/src" cs4500:0.1 bash -c "cd src; g++ -std=c++11 -Wall -g test/m2.cpp -o m2; valgrind  --leak-check=full --track-origins=yes ./m2"

val3:
	docker build -t cs4500:0.1 .
	docker run -ti -v "$$(pwd):/src" cs4500:0.1 bash -c "cd src; g++ -std=c++11 -Wall -g  -pthread test/m3.cpp -o m3; valgrind  --leak-check=full --fair-sched=yes --track-origins=yes ./m3"

val4:
	docker build -t cs4500:0.1 .
	docker run -ti -v "$$(pwd):/src" cs4500:0.1 bash -c "cd src; g++ -std=c++11 -Wall -g  -pthread test/m4.cpp -o m4; valgrind  --leak-check=full --fair-sched=yes --track-origins=yes ./m4 data/simple.txt"
	
# handin:
# 	./sorer -f test/0.sor -print_col_type 0  
# 	#BOOL

# 	./sorer -f test/0.sor -is_missing_idx 0 0
# 	#1

# 	./sorer -f test/1.sor -print_col_type 0  
# 	#STRING

# 	./sorer -f test/1.sor -is_missing_idx 0 1
# 	#0

# 	./sorer -f test/1.sor -print_col_idx 0 3
# 	#"+1"

# 	./sorer -f test/2.sor -print_col_type 0
# 	#BOOL

# 	./sorer -f test/2.sor -print_col_type 1
# 	#INT

# 	./sorer -f test/2.sor -print_col_type 2
# 	#FLOAT

# 	./sorer -f test/2.sor -print_col_type 3
# 	#STRING

# 	./sorer -f test/2.sor -is_missing_idx 1 0
# 	#1

# 	./sorer -f test/2.sor -is_missing_idx 1 1
# 	#0

# 	./sorer -f test/2.sor -print_col_idx 3 0
# 	#"hi"

# 	./sorer -f test/2.sor -print_col_idx 3 1
# 	#"ho ho ho"

# 	./sorer -f test/2.sor -print_col_idx 2 0
# 	#1.2

# 	./sorer -f test/2.sor -print_col_idx 2 1
# 	#-0.2

# 	./sorer -f test/3.sor -print_col_type 4
# 	#BOOL

# 	./sorer -f test/3.sor -print_col_idx 2 10
# 	#1.2

# 	./sorer -f test/3.sor -print_col_idx 2 384200
# 	#1.2

# 	./sorer -f test/4.sor -print_col_idx 0 1
# 	#2147483647

# 	./sorer -f test/4.sor -print_col_idx 0 2
# 	#-2147483648

# 	./sorer -f test/4.sor -print_col_idx 1 1
# 	#-2e-09

# 	./sorer -f test/4.sor -print_col_idx 1 2
# 	#1e+10

# 	./sorer -f test/1.sor -from 1 -len 74 -print_col_type 0
# 	#STRING

# 	./sorer -f test/1.sor -from 1 -len 74 -is_missing_idx 0 0
# 	#0

# 	./sorer -f test/1.sor -from 1 -len 74 -print_col_idx 0 6
# 	#"+2.2"
