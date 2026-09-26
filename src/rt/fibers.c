// Native stacks for the game's four threads. See fibers.h and the design note
// (agent-docs specs/2026-09-21-m5-fibers-design.md). The context switch saves the callee-saved
// registers on the outgoing stack, swaps stack pointers and restores from the incoming one; a
// fresh fiber's stack holds an initial frame that "returns" into fiber_tramp, which calls
// fiber_main with the GB. Stacks are mmap regions with a guard page at the low end. The switch is
// file-scope assembly rather than naked functions, which GCC ignores on arm64.
#include "rt/fibers.h"
#include "hooks/hooks.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define NFIBERS 4
#define STACK_SIZE (1u << 20)

typedef struct {
  void *sp;
  uint8_t *stack;
  bool live;
  HookCtx ctx;
  void (*start)(GB *);
} Fiber;

struct Fibers {
  Fiber f[NFIBERS];
  int current, request;
  void *kernel_sp;
  HookCtx kernel_ctx;
};

static void fiber_main(void *arg);


#ifdef __APPLE__
#define ASM_FUNC(name) ".globl _" #name "\n.p2align 4\n_" #name ":\n"
#else
#define ASM_FUNC(name) ".globl " #name "\n.type " #name ", %function\n.p2align 4\n" #name ":\n"
#endif

void oracles_fiber_swap(void **save_sp, void *load_sp);
void oracles_fiber_tramp(void);

#if defined(__aarch64__)
__asm__(".text\n" ASM_FUNC(oracles_fiber_swap)
        "sub sp, sp, #160\n"
        "stp x19, x20, [sp, #0]\n"
        "stp x21, x22, [sp, #16]\n"
        "stp x23, x24, [sp, #32]\n"
        "stp x25, x26, [sp, #48]\n"
        "stp x27, x28, [sp, #64]\n"
        "stp x29, x30, [sp, #80]\n"
        "stp d8, d9, [sp, #96]\n"
        "stp d10, d11, [sp, #112]\n"
        "stp d12, d13, [sp, #128]\n"
        "stp d14, d15, [sp, #144]\n"
        "mov x9, sp\n"
        "str x9, [x0]\n"
        "mov sp, x1\n"
        "ldp x19, x20, [sp, #0]\n"
        "ldp x21, x22, [sp, #16]\n"
        "ldp x23, x24, [sp, #32]\n"
        "ldp x25, x26, [sp, #48]\n"
        "ldp x27, x28, [sp, #64]\n"
        "ldp x29, x30, [sp, #80]\n"
        "ldp d8, d9, [sp, #96]\n"
        "ldp d10, d11, [sp, #112]\n"
        "ldp d12, d13, [sp, #128]\n"
        "ldp d14, d15, [sp, #144]\n"
        "add sp, sp, #160\n"
        "ret\n"
        ASM_FUNC(oracles_fiber_tramp)
        "mov x0, x20\n"
        "blr x19\n"
        "brk #1\n");
static void *fiber_init_frame(uint8_t *top, void (*fn)(void *), void *arg) {
  uint64_t *frame = (uint64_t *)(top - 160);
  memset(frame, 0, 160);
  frame[0] = (uint64_t)fn;
  frame[1] = (uint64_t)arg;
  frame[11] = (uint64_t)oracles_fiber_tramp;
  return frame;
}
#elif defined(__x86_64__)
__asm__(".text\n" ASM_FUNC(oracles_fiber_swap)
        "pushq %rbp\n pushq %rbx\n pushq %r12\n pushq %r13\n pushq %r14\n pushq %r15\n"
        "movq %rsp, (%rdi)\n"
        "movq %rsi, %rsp\n"
        "popq %r15\n popq %r14\n popq %r13\n popq %r12\n popq %rbx\n popq %rbp\n"
        "ret\n"
        ASM_FUNC(oracles_fiber_tramp)
        "movq %r12, %rdi\n andq $-16, %rsp\n callq *%r13\n ud2\n");
static void *fiber_init_frame(uint8_t *top, void (*fn)(void *), void *arg) {
  uint64_t *frame = (uint64_t *)(top - 56);
  memset(frame, 0, 56);
  frame[0] = 0;                       // r15
  frame[1] = 0;                       // r14
  frame[2] = (uint64_t)fn;            // r13
  frame[3] = (uint64_t)arg;           // r12
  frame[6] = (uint64_t)oracles_fiber_tramp;   // return address
  return frame;
}
#else
#error "fibers: unsupported architecture"
#endif

static void fiber_prepare(struct Fibers *F, Fiber *f, GB *gb) {
  (void)F;
  if (!f->stack) {
    size_t page = (size_t)sysconf(_SC_PAGESIZE);
    f->stack = mmap(NULL, STACK_SIZE, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (f->stack == MAP_FAILED) { perror("fiber: mmap"); abort(); }
    mprotect(f->stack, page, PROT_NONE);
  }
  f->sp = fiber_init_frame(f->stack + STACK_SIZE, fiber_main, gb);
}
static void fiber_enter(struct Fibers *F, Fiber *f) { oracles_fiber_swap(&F->kernel_sp, f->sp); }
static void fiber_leave(struct Fibers *F, Fiber *f) { oracles_fiber_swap(&f->sp, F->kernel_sp); }

static struct Fibers *fibers(GB *gb) {
  if (!gb->fib) { gb->fib = calloc(1, sizeof *gb->fib); gb->fib->current = -1; }
  return gb->fib;
}

static void fiber_main(void *arg) {
  GB *gb = arg;
  struct Fibers *F = gb->fib;
  F->f[F->current].start(gb);
  fprintf(stderr, "fiber: thread %d returned from its entry\n", F->current);
  abort();
}

bool fiber_in_thread(const GB *gb) { return gb->fib && gb->fib->current >= 0; }
bool fiber_live(const GB *gb, int n) { return gb->fib && n >= 0 && n < NFIBERS && gb->fib->f[n].live; }

void fibers_reset(GB *gb) {
  struct Fibers *F = fibers(gb);
  for (int i = 0; i < NFIBERS; i++) F->f[i].live = false;
  F->current = -1;
}

int fiber_run(GB *gb, int n, void (*start)(GB *), uint16_t fallback_pc) {
  if (hook_mode == HOOK_MODE_VERIFY) { hook_handoff(gb, fallback_pc); return FIBER_YIELD; }
  struct Fibers *F = fibers(gb);
  if (n < 0 || n >= NFIBERS) { fprintf(stderr, "fiber: bad thread %d\n", n); gb->hung = true; return FIBER_EXIT; }
  Fiber *f = &F->f[n];
  if (start) {
    if (!f->start) hook_ctx_init(&f->ctx);
    fiber_prepare(F, f, gb);
    f->start = start;
    f->ctx.depth = f->ctx.jmp_depth = 0;
    f->live = true;
  } else if (!f->live) {
    fprintf(stderr, "fiber: resume of a dead thread %d (frame %llu)\n", n, (unsigned long long)GRID_FRAME(gb->cycles));
    gb->hung = true;
    return FIBER_EXIT;
  }
  F->current = n;
  F->request = FIBER_YIELD;
  hook_ctx_switch(&F->kernel_ctx, &f->ctx);
  fiber_enter(F, f);
  F->current = -1;
  return F->request;
}

void fiber_back(GB *gb, int request) {
  if (hook_mode == HOOK_MODE_VERIFY) { hook_handoff(gb, gb->pc); return; }
  struct Fibers *F = gb->fib;
  if (!F || F->current < 0) { fprintf(stderr, "fiber: switch out of the kernel (pc %04x)\n", gb->pc); gb->hung = true; return; }
  Fiber *f = &F->f[F->current];
  F->request = request;
  if (request != FIBER_YIELD) f->live = false;
  hook_ctx_switch(&f->ctx, &F->kernel_ctx);
  fiber_leave(F, f);
  if (request != FIBER_YIELD) { fprintf(stderr, "fiber: dead thread resumed\n"); abort(); }
}
