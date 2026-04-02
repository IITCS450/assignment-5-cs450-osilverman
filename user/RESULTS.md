# Assignment 5 Results

## Context-Switching Approach
Context switching is implemented using a dedicated x86 assembly routine in `uthread_switch.S`. When `thread_switch(old_esp, new_esp)` is called, it saves the current thread's callee-saved registers (ebp, ebx, esi, edi) onto the stack, saves the stack pointer into the old thread's control block, loads the new thread's saved stack pointer, and restores the new thread's registers. This is a cooperative switch — threads must explicitly call `thread_yield()` to give up the CPU.

## Limitations
- Maximum of 8 threads (MAX_THREADS = 8), including the main thread
- Stack size is fixed at 4096 bytes per thread
- Cooperative scheduling only — a thread that never yields will starve others
- No preemption or timer-based scheduling
- Thread IDs are indices 0-7; thread 0 is always the main thread
