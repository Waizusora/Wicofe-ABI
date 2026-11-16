# OSynit Userspace ABI Requirements 0.1

- **Status:** Proposed requirements; no syscall numbers or layouts assigned
- **Owner:** Wicofe-ABI
- **Consumer:** OSynit
- **Date:** 2026-11-16

## 1. Purpose and boundary

OSynit is the Wicofe userspace init system and PID 1 supervisor. This document identifies ABI capabilities OSynit needs. It does not create a parallel ABI, assign syscall numbers, select register conventions, or freeze structure layouts.

The canonical syscall contract remains [Wicofe Userspace ABI Specification Baseline](../SPECIFICATION_BASELINE.md). Kernel mechanisms are implemented by [Wicofe-Kernel](https://github.com/Waizusora/Wicofe-Kernel).

## 2. Required capability domains

The ABI design must cover these domains with explicit semantics:

- Process creation/exec, process identifiers, exit status, wait/reap, and orphan adoption visibility.
- Signal delivery, signal masks/handling where supported, and interruption/restart semantics for wait and I/O.
- File descriptors, pipes, filesystem access, and process standard streams.
- Unix-domain local IPC or an explicitly versioned equivalent suitable for a local manager socket.
- Monotonic clocks and timeout-safe waiting.
- Privilege and peer-credential inspection for authenticating local OSynit IPC clients.
- System information and runtime capability discovery.
- Authorized system reboot, poweroff, and halt requests.

## 3. Design constraints

1. Use explicit-width types and C-compatible layouts for public ABI structures.
2. Define blocking, interruption, partial-completion, error, and permission behavior per operation.
3. Distinguish process identifiers from stable process handles; define PID reuse safety.
4. Do not expose Rust compiler-private layouts.
5. Keep optional capabilities discoverable and independently versioned.
6. Do not copy Linux syscall numbers or claim Linux semantic compatibility without a reviewed Wicofe contract.
7. Do not add an ABI entry solely because OSynit has a command with a similar name; CLI commands are userspace protocol, not kernel syscalls.

## 4. Optional capabilities

Cgroups, pidfd-equivalent handles, advanced namespaces, resource controls, socket activation primitives, watchdog interfaces, and security sandboxing are optional until separately accepted and specified. Their absence must not prevent the mandatory init core from operating if an equivalent required primitive is available.

## 5. Conformance

Each ABI operation consumed by OSynit must link to:
- an ABI specification and stability label;
- kernel implementation and supported target matrix;
- positive, negative, boundary, interruption, and permission tests;
- userspace wrapper mapping where applicable.

OSynit host tests do not establish Wicofe ABI conformance.
