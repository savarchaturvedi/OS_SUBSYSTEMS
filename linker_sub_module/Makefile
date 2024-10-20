OUTPUT_DIR = my-output
SRC = savar_final_code.cpp
EXECUTABLE = linker

all: setup compile run grade

setup:
	mkdir -p $(OUTPUT_DIR)

compile: $(SRC)
	g++ -g $(SRC) -o $(EXECUTABLE)

run: compile
	./runit.sh $(OUTPUT_DIR) ./$(EXECUTABLE)

grade: run
	./gradeit.sh . $(OUTPUT_DIR)
