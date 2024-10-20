

# Define targets
all: create_dir compile runit gradeit

# Rule to compile main.cpp
create_dir:
	mkdir -p savar_out

compile: main.cpp
	 g++ main.cpp -o a.out

# Rule to run runit.sh
runit:
	./runit.sh "./savar_out" "./a.out"

# Rule to run gradeit.sh
gradeit:
	./gradeit.sh "./refout" "./savar_out"

# Target to clean up generated files
clean:
	rm -f a.out

# Ensure that 'clean' is not interpreted as a file name
.PHONY: clean
