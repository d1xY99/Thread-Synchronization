# Thread-Synchronization

# Airport Check-In Simulation

## Main Idea

### Roles
- **Passengers**: Check-in via self-service or traditional desk
- **Employees**: Assist passengers at check-in desks
- **Self-check-in kiosks**: Automated check-in machines
- **Maintainers**: Service and repair self-check-in machines

### Traditional Check-In Workflow
- Lane opens when first employee arrives
- Employees process check-ins only when passengers are queued
- Employee notifies passenger upon completion
- Synchronization ensures proper employee release

### Self Check-In Workflow
- Machines always available
- Machine states: 
  1. **READY**
  2. **START**
  3. **ID_SCAN**
  4. **PASSENGER_IDENTIFIED**
- After ticket printing, passenger proceeds to baggage drop or gate
- Machines return to **READY** or **OUT_OF_SERVICE**
- Maintainers independently check and repair machines

## Building the Project

### Prerequisites
- CMake build system

### Build Methods

#### In-Source Build
```bash
cd A2/
cmake .
make
```

#### Out-of-Source Build
```bash
cd A2/
mkdir build && cd build
cmake ..
make
```

### Running the Executable
```bash
./airport num_passengers num_employees num_selfcheckins
```

**Example:**
```bash
./airport 1 1 1
```
