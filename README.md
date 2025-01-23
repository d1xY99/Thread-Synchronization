# Multithreaded Restaurant Simulation

## Overview
A C-based multithreaded simulation demonstrating concurrent restaurant operations with guests, waiters, and table management.

## Features
- Simulates restaurant workflow using POSIX threads
- Manages shared resources with mutexes and semaphores
- Handles concurrent guest and waiter interactions
- Implements thread-safe table allocation and meal preparation

## System Requirements
- POSIX threads library
- C compiler with C11 support
- Make and CMake build tools

## Compilation

### Build Steps
```bash
mkdir build
cd build
cmake ..
make
```

### Running the Simulation
```bash
./restaurant [num_waiters] [num_guests]
```

## Key Components
- Guests: Request tables, order meals
- Waiters: Prepare meals, clean tables
- Synchronization primitives: Mutexes, condition variables, semaphores
- Dynamic table management

## Thread Synchronization
- Ensures thread-safe access to shared resources
- Prevents race conditions
- Manages restaurant opening and table availability

