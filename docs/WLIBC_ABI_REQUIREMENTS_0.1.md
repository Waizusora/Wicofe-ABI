# Wlibc Native ABI Requirements 0.1

- **Status:** Draft requirements; carry-flag result convention, initial errno values, and register preservation contract are adopted; syscall numbers remain unassigned.
- **Owner:** Wicofe-ABI.
- **Consumer:** [Wlibc](https://github.com/Waizusora/WLibc).
- **Kernel implementation:** [Wicofe-Kernel](https://github.com/Waizusora/Wicofe-Kernel).
- **Conformance target:** ISO C17 and IEEE Std 1003.1-2024 (POSIX.1-2024).

## 1. Purpose

Define the kernel/userspace ABI capabilities required to port the pinned musl 1.2.6 source baseline to Wicofe. This document specifies requirements only; it does not assign syscall numbers, registers, structures or raw error values. Those belong in reviewed ABI specifications.

## 2. Mandatory ABI decisions

The following contracts are prerequisites for a native Wlibc backend:

1. **Raw syscall boundary:** x86-64 and AArch64 entry mechanism, argument registers, clobbers, return register, carry-flag result encoding and interruption rules.
2. **Process startup:** executable entry state, stack alignment, argc/argv/envp, auxiliary data policy, environment/security metadata, process exit and exec semantics.
3. **Virtual memory:** page-size discovery, map/unmap/protect, shared/private mappings, file-backed mappings, anonymous mappings, fixed-address rules and failure behavior.
4. **File descriptors and VFS:** open/close/read/write, seek, metadata, directory iteration, descriptor duplication, pipes, polling, blocking, partial I/O and close-on-exec semantics.
5. **Process lifecycle:** process creation, exec, wait/reap, identifiers/handles, child status and orphan handling.
6. **Threads and TLS:** thread creation/exit, thread pointer setup, TLS image/layout, stack allocation contract, thread-local destructors and synchronization primitives.
7. **Synchronization and cancellation:** wait/wake semantics, timeout clock selection, robust mutex behavior, cancellation points and signal interaction.
8. **Signals:** action layout, mask operations, delivery, alternate stack, signal frame/return, interruption and restart semantics.
9. **Time:** realtime and monotonic clocks, resolution, timers, sleep, absolute/relative deadlines and interruption behavior.
10. **Networking:** socket creation, bind/connect/listen/accept, send/receive, socket options, shutdown, polling and error semantics.
11. **Executable and dynamic loader ABI:** ELF class/machine, load segments, relocations, interpreter path, initial stack, TLS relocations, shared-object loading and symbol resolution.

## 3. Architecture requirements

Initial architectures are x86-64 and AArch64. Both must expose the same logical syscall semantics and public C data layouts unless a documented architecture property requires otherwise. Architecture-specific entry registers and assembly remain private to Wlibc's backend.

## 4. POSIX mapping rule

POSIX functions are not one-to-one with syscalls. Wlibc may implement an interface using multiple syscalls, userspace algorithms, or a separate POSIX layer. Kernel ABI work must provide only mechanisms that require kernel participation. Do not add syscall entries solely to mirror libc function names.

## 5. Raw error convention

The existing ABI baseline leaves raw syscall error encoding undecided. This is a hard blocker for implementing `__syscall_ret`, errno conversion and cancellation-aware syscall wrappers. Select one convention and define the complete valid success range before Wlibc implements the raw-call layer.

## 6. Implementation order

1. Complete raw syscall stub and kernel entry/return implementation against the adopted convention.
2. Specify process entry and termination.
3. Specify VM and descriptor/VFS primitives.
4. Specify process/thread lifecycle, TLS and wait/wake.
5. Specify signal and cancellation semantics.
6. Specify clocks/timers and networking.
7. Specify executable/ELF and dynamic loader contracts.

## 7. Validation requirements

Every ABI entry must have explicit-width arguments, alignment, pointer direction and validation rules, permission checks, blocking/interruption behavior, partial-completion semantics, error mapping, stability label and kernel/userspace tests.

The ABI remains draft until these contracts are reviewed, implemented and tested. Wlibc must not introduce private syscall numbers or Linux fallback paths.
