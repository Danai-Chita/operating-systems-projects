# KIWI – Multi-threaded Data Storage Engine

## Introduction
This project implements **KIWI**, a multi-threaded data storage engine written in **C**,
based on the **Log-Structured Merge Tree (LSM-tree)** architecture.
The system supports concurrent read and write operations on key–value pairs,
ensuring thread-safe access and performance evaluation under different workloads.

The implementation was developed as part of an **Operating Systems** coursework
and utilizes the **Pthreads** library on Linux for concurrency management.

---

## How It Works
The storage engine manages data using a combination of **in-memory** and **on-disk**
data structures, following the LSM-tree design principles.

- Incoming key–value pairs are first inserted into an in-memory structure
  called the **memtable**.
- When the memtable reaches its capacity, it is flushed to disk as a
  **Sorted String Table (SST)**.
- Multiple SST files are periodically merged to maintain efficient access
  and avoid excessive fragmentation.

The system supports both **read (get)** and **write (add)** operations,
which can be executed concurrently by multiple threads.

---

## LSM-tree Design
- **Memtable**:  
  An in-memory sorted data structure (implemented as a skip list) used
  for fast insertions and lookups.
- **SST Files**:  
  Immutable, sorted key–value files stored on disk after flushing the memtable.
- **Compaction / Merging**:  
  Overlapping key ranges across SSTs are merged to improve read performance
  and maintain data organization.

---

## Concurrency
The engine is designed to support **multi-threaded execution**:
- Multiple threads can perform read and write operations concurrently.
- Synchronization is handled using **mutex locks** and **condition variables**
  to ensure consistency and thread safety.
- Thread coordination allows controlled execution of mixed workloads
  (reads and writes).

---

## Performance Measurements
The system includes benchmarking functionality to evaluate performance:
- Measures execution time for read and write operations.
- Supports both **sequential** and **random** key access patterns.
- Reports statistics such as:
  - total number of operations
  - average time per operation
  - execution time per thread

---

## Implementation Details
Key components of the system include:
- **Memtable implementation** using a skip list
- **On-disk SST management**
- **Thread creation and synchronization** using Pthreads
- **Benchmarking utilities** for workload evaluation

Detailed design decisions and analysis are provided in the project report.

---

## How to Use

### Compilation
The project includes Makefiles for building the source code:
```bash
./kiwi-bench write <count>
./kiwi-bench read <count>
./kiwi-bench readwrite <count> <threads> <write_percentage>
