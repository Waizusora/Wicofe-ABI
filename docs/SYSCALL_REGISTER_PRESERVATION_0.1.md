# Wicofe Syscall Register Preservation Contract 0.1

- **Status:** Accepted design contract for the initial x86-64 and AArch64 syscall boundary.
- **Owner:** Wicofe-ABI.
- **Scope:** General-purpose register preservation, stack pointer, and condition-code effects at syscall return.

## 1. General rule

A syscall is a controlled kernel transition, not a normal language-level function call. Nevertheless, Wicofe defines a C-call-compatible preservation rule so compiler-generated callers and handwritten stubs can share one predictable contract.

- Callee-saved registers under the platform's selected userspace procedure-call ABI must be preserved by the kernel across a syscall.
- Caller-saved registers may be clobbered, including argument and result registers.
- The stack pointer is preserved exactly; the kernel must not return with a changed userspace SP.
- The instruction pointer resumes at the instruction immediately following the trap instruction.
- The carry condition is the only condition flag with a defined syscall-result meaning. Other condition flags are unspecified after return.
- Syscalls may read or write user memory through pointers. The raw stub must declare a compiler memory clobber or use an equivalent out-of-line assembly boundary.

The selected userspace procedure-call ABI is part of the platform ABI baseline. This document does not independently redefine it.

## 2. x86-64

The baseline userspace procedure-call ABI is System V AMD64.

### Preserved

- RBX
- RBP
- R12–R15
- RSP (exactly)

### Clobbered / unspecified

- RAX (syscall identifier input; result output)
- RCX (architecturally overwritten by SYSCALL)
- RDX, RSI, RDI (argument registers)
- R8, R9, R10 (argument registers)
- R11 (architecturally overwritten with saved RFLAGS)
- RFLAGS condition bits other than the defined result CF

The kernel must restore the preserved GPR set on every return path, including error, interruption and signal-related paths. RCX/R11 must be treated as clobbered even if a particular kernel implementation happens to restore them.

## 3. AArch64

The baseline userspace procedure-call ABI is AAPCS64.

### Preserved

- X19–X29
- SP (exactly)
- X18 is platform-reserved; its preservation/ownership follows the Wicofe platform ABI and must not be repurposed by syscall entry code.

### Clobbered / unspecified

- X0 (argument 1 and result)
- X1–X5 (arguments)
- X8 (syscall identifier)
- X9–X17 (caller-saved temporaries; X16/X17 are IP0/IP1)
- X30 (link register; caller-saved across a function call)
- NZCV bits other than the defined result C

The kernel must restore X19–X29 and the exact user SP on every return path. X18 must follow the platform ABI and must not be silently used as scratch.

## 4. Signal and interruption interaction

Signal delivery may intentionally construct a handler context, but signal return must restore the interrupted user context according to a separately specified signal-frame ABI. The syscall return carry result must not be confused with the interrupted context's saved condition state. Until signal-frame semantics are specified, no implementation may claim complete signal-safe syscall behavior.

An interrupted syscall may return EINTR or be restarted only according to that syscall's specification. Register preservation does not imply automatic restart.

## 5. Compiler obligations

- x86-64 stubs declare RCX, R11, condition-code output, and memory effects.
- AArch64 stubs declare X8 and condition-code output, plus memory effects.
- All explicit register operands must match the compiler target ABI.
- Verify GCC and Clang output at supported optimization levels and with LTO if enabled.
- Do not read condition flags in a separate C statement after the inline assembly.

## 6. Required tests

- Canary values in every preserved GPR survive successful and failed syscalls.
- Stack pointer is identical before and after return.
- Caller-saved clobbers are declared and compiler tests do not rely on their values.
- CF/C reports success and failure correctly.
- Other condition flags are treated as unspecified.
- Interrupt/preemption and exception return paths preserve the saved user context.
- Run on QEMU x86-64 and AArch64 before claiming runtime validation.
