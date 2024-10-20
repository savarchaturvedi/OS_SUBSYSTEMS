Operating Systems Subsystems Project
Overview
This project consists of various subsystems implemented as part of the Graduate Operating Systems course. It showcases my understanding of OS concepts such as process management, memory management, and file systems. The code is designed to simulate essential OS functionalities, providing a comprehensive learning experience in system-level programming.

Project Structure
The project is divided into the following major subsystems:

Process Management: Handles task scheduling, process creation, and termination.
Memory Management: Implements paging and segmentation to efficiently allocate memory resources.
File System Simulation: Provides a simplified implementation of how an OS manages files, directories, and their attributes.
Inter-process Communication (IPC): Demonstrates how processes can communicate with each other, using shared memory, message queues, or pipes.
I/O Management: Simulates basic input/output operations, illustrating how OS handles device communication.
Features
Process Scheduler: Implements a round-robin or priority-based scheduler.
Paging System: Simulates the memory paging mechanism to handle memory allocations.
File Directory Structure: Mimics file system operations, including file creation, deletion, and searching.
IPC Mechanisms: Allows communication between different simulated processes using shared memory.
I/O Operations: Provides a basic abstraction of how the OS manages input/output devices.
Getting Started
Prerequisites
To compile and run the code, ensure you have the following:

A C++ compiler (e.g., GCC)
Make (for building the project)
Installation
Clone the repository:

bash
Copy code
git clone https://github.com/savarchaturvedi/os-subsystems.git
cd os-subsystems
Build the project using make:

bash
Copy code
make
Run the executable:

bash
Copy code
./os_subsystem
Running Examples
To demonstrate the various subsystems, you can pass specific flags to the executable.

Run the process scheduler:

bash
Copy code
./os_subsystem -scheduler
Test memory management:

bash
Copy code
./os_subsystem -memory
Simulate file system operations:

bash
Copy code
./os_subsystem -file
Run inter-process communication simulation:

bash
Copy code
./os_subsystem -ipc
Project Files
main.cpp: The entry point of the application, handling command-line arguments and initializing subsystems.
scheduler.cpp: Contains the implementation of the process scheduling system.
memory.cpp: Simulates memory management functionalities.
file_system.cpp: Implements basic file system operations.
ipc.cpp: Demonstrates inter-process communication using shared memory.
Technologies Used
C++: The entire project is written in C++ to simulate low-level operating system functions.
Unix/Linux System Calls: Utilized for process creation, memory allocation, and IPC mechanisms.
Future Enhancements
Add a more comprehensive memory management system to handle complex allocations.
Integrate more advanced scheduling algorithms such as Shortest Job First (SJF) or Multilevel Feedback Queue (MLFQ).
Implement network communication between processes using sockets.
Contributing
Feel free to submit issues or pull requests if you would like to contribute to the project.

License
This project is licensed under the MIT License - see the LICENSE file for details.
