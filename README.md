*This project has been created as part of the 42 curriculum by serromer.*

# CODEXION

## Description

Codexion is a concurrency simulation written in C that models a shared co-working hub where coders compete for limited hardware resources. Each coder must acquire two rare USB dongles simultaneously to compile their quantum code before burning out from inactivity. This project is a complex variant of the classic Dining Philosophers problem, introducing additional constraints such as resource cooldowns and dynamic scheduling policies (FIFO and EDF) to prevent deadlocks and ensure fair resource distribution.

## Instructions

### Compilation
```bash
# From the repository root
make -C coders

# Or from inside coders/
cd coders && make
```

### Execution
```bash
./coders/codexion number_of_coders time_to_burnout time_to_compile \
                  time_to_debug time_to_refactor number_of_compiles_required \
                  dongle_cooldown scheduler
```

| Argument | Description |
|---|---|
| `number_of_coders` | Number of coders and dongles |
| `time_to_burnout` | Max ms without compiling before burnout |
| `time_to_compile` | Duration of compilation in ms |
| `time_to_debug` | Duration of debugging in ms |
| `time_to_refactor` | Duration of refactoring in ms |
| `number_of_compiles_required` | Compiles needed per coder to finish |
| `dongle_cooldown` | Ms a dongle is unavailable after release |
| `scheduler` | `fifo` or `edf` |

### Example
```bash
./coders/codexion 5 500 100 100 100 7 50 edf
```

## Resources

### References

- [POSIX Threads Documentation](https://man7.org/linux/man-pages/man7/pthreads.7.html)
- [Dining Philosophers Problem](https://en.wikipedia.org/wiki/Dining_philosophers_problem)
- [POSIX Threads Programming Guide](https://hpc-tutorials.llnl.gov/posix/)
- [Earliest Deadline First Scheduling](https://en.wikipedia.org/wiki/Earliest_deadline_first_scheduling)

### AI Usage

AI tools were used to clarify complex concurrency concepts, understand pthread primitives, and debug race conditions identified during testing. The core synchronization logic, thread management, priority queue implementation, and all C89-compliant data structures were implemented by the developer to ensure full understanding during peer evaluation.

## Blocking cases handled

### Deadlock prevention

To prevent circular wait — a key Coffman condition — coders with even-numbered IDs wait briefly at startup before attempting to acquire dongles. This breaks the symmetry that causes all coders to grab their left dongle simultaneously, ensuring at least one coder can always access both resources and make progress.

### Starvation prevention

The EDF (Earliest Deadline First) scheduler prioritizes coders closest to burning out. Each dongle maintains a priority queue (min-heap) of waiting coders ordered by their deadline (`last_compile_time + time_to_burnout`). The coder with the nearest deadline is always served first, preventing indefinite waiting.

### Cooldown handling

Each `t_dongle` structure tracks a `cooldown_until` timestamp in milliseconds. When a coder releases a dongle, `cooldown_until` is set to `current_time + dongle_cooldown`. Any coder attempting to acquire the dongle before this timestamp uses `pthread_cond_timedwait` to sleep precisely until the cooldown expires, avoiding busy-waiting.

### Burnout detection

A dedicated monitor thread runs independently, checking every 500 microseconds whether `now - last_compile_time >= time_to_burnout` for each coder. Upon detection, the burnout message is printed immediately within the required 10ms window, `simulation_over` is set to 1, and all waiting coders are woken via `pthread_cond_broadcast`.

### Log serialization

A global `log_lock` mutex wraps every `printf` call across all threads. This ensures that state change messages are printed atomically — no two messages can interleave on the same output line, regardless of how many threads are running simultaneously.

## Thread synchronization mechanisms

### Mutex usage

Four distinct mutexes protect different shared resources:

- **`log_lock`**: Global mutex wrapping every `printf`. Prevents interleaved output between coder and monitor threads.

- **`env->state_lock`**: Protects `simulation_over`, the global stop flag. The monitor writes it via `set_sim_over()` and all coders read it via `sim_is_over()`. Without this mutex, a coder could read a partially written value and continue running after burnout is detected.

- **`dongle->mutex`**: One per dongle. Protects `in_use`, `cooldown_until`, and the `waiters` priority queue. Prevents two coders from simultaneously acquiring the same dongle.

- **`coder->state_lock`**: One per coder. Protects `last_compile_time` and `compile_count`. The monitor reads these while the coder thread updates them — this mutex prevents the monitor from reading a corrupted intermediate value.

### Condition variables

Each dongle has a `pthread_cond_t cond` used to manage its waiting queue efficiently without busy-waiting:

- **`pthread_cond_wait`**: Used in `take_one_dongle` when the dongle is occupied or not yet the coder's turn according to the scheduler.
- **`pthread_cond_timedwait`**: Used when the dongle is in cooldown — the coder sleeps until the exact moment the cooldown expires.
- **`pthread_cond_broadcast`**: Called in `drop_dongles` after releasing a dongle, and in `handle_burnout` to wake all waiting coders so they can detect `simulation_over` and exit cleanly.

### Monitor thread

A dedicated monitor thread runs `monitor_routine`, checking two stop conditions every 500 microseconds:

- **Burnout**: `check_health()` computes `now - last_compile_time` for each coder using `coder->state_lock`. If this exceeds `time_to_burnout`, `handle_burnout()` prints the burnout message, sets `simulation_over`, and broadcasts to all dongles.
- **Completion**: `check_all_compiled()` verifies that every coder's `compile_count >= number_of_compiles_required`. If true, `set_sim_over()` is called and the simulation ends silently.

### Extra

```
.
├── coders/               # Mandatory: Core source files 
│   ├── codexion.h        # Global header with structures and prototypes 
│   ├── main.c            # Entry point and argument handling 
│   ├── parsing.c         # Validation of mandatory arguments 
│   ├── init.c            # Initialization of mutexes, threads, and data
│   ├── routine.c         # Coder thread lifecycle (compile, debug, refactor)
│   ├── actions.c         # Dongle acquisition and state changes
│   ├── actions_utils.c   # Helpers for timing and synchronization 
│   ├── monitor.c         # Separate thread for burnout detection 
│   ├── heap.c            # Priority queue implementation for FIFO/EDF 
│   ├── heap_utils.c      # Min-heap utility functions for scheduling 
│   └── utils.c           # General helpers (timing, cleanup, strings) 
├── docs/                 # Project documentation and planning
│   ├── en.subject_codexion.pdf
│   ├── codexion_plan.html
│   ├── ES/               # Spanish translations/notes
│   └── tests/            # Unit tests for internal modules
│       └── test_heap.c   # Verification of the priority queue logic
├── Makefile              # Compiles with -Wall -Wextra -Werror -pthread
└── README.md             # This file


```