#include "types.h"
#include "stat.h"
#include "user.h"
#include "uthread.h"

#define MAX_THREADS 8
#define STACK_SIZE  4096

struct thread {
  int state;        
  uint esp;        
  char stack[STACK_SIZE];
};

#define FREE     0
#define RUNNABLE 1
#define RUNNING  2
#define DONE     3

static struct thread threads[MAX_THREADS];
static int current = 0;


static void thread_switch(uint *old_esp, uint new_esp);

static void thread_exit(void);

void thread_init(void) {
  threads[0].state = RUNNING;
  current = 0;
}

tid_t thread_create(void (*fn)(void*), void *arg) {
  for(int i = 1; i < MAX_THREADS; i++) {
    if(threads[i].state == FREE) {
      threads[i].state = RUNNABLE;

    
      uint *sp = (uint*)(threads[i].stack + STACK_SIZE);
      *--sp = (uint)arg;       
      *--sp = (uint)thread_exit;  
      *--sp = (uint)fn;        
      *--sp = 0; 
      *--sp = 0; 
      *--sp = 0; 
      *--sp = 0; 

      threads[i].esp = (uint)sp;
      return i;
    }
  }
  return -1;
}

void thread_yield(void) {
  int old = current;

  for(int i = 1; i <= MAX_THREADS; i++) {
    int next = (old + i) % MAX_THREADS;
    if(threads[next].state == RUNNABLE) {
      threads[old].state = (threads[old].state == RUNNING) ? RUNNABLE : threads[old].state;
      threads[next].state = RUNNING;
      current = next;
      thread_switch(&threads[old].esp, threads[next].esp);
      return;
    }
  }
}

int thread_join(tid_t tid) {
  if(tid < 0 || tid >= MAX_THREADS) return -1;
  while(threads[tid].state != DONE) {
    thread_yield();
  }
  threads[tid].state = FREE;
  return 0;
}

static void thread_exit(void) {
  threads[current].state = DONE;
  thread_yield();
}

static void thread_switch(uint *old_esp, uint new_esp) {
  __asm__ volatile(
    "push %%edi\n"
    "push %%esi\n"
    "push %%ebx\n"
    "push %%ebp\n"
    "movl %%esp, (%0)\n"   
    "movl %1, %%esp\n"    
    "pop %%ebp\n"
    "pop %%ebx\n"
    "pop %%esi\n"
    "pop %%edi\n"
    :
    : "r"(old_esp), "r"(new_esp)
    : "memory"
  );
}