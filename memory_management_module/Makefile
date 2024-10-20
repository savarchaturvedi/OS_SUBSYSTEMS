all: build run grade

# Build the program
build:
	g++ -std=c++11 -o main main.cpp


# Run the program using the script
run:
	mkdir sa_out
	./runit.sh sa_out ./a.out

# Grade the output using the script
grade:
	./gradeit.sh sa_out refout
# Clean target to remove the executable
clean:
	rm -f $(TARGET)
