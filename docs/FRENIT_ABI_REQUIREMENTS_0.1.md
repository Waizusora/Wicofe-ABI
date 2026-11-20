# Frenit Userspace ABI Requirements 0.1

- **Status:** Proposed capability requirements; no syscall numbers or layouts assigned.
- **Owner:** Wicofe-ABI.
- **Consumer:** [Frenit — Free Init](https://github.com/Waizusora/Frenit).
- **Kernel implementation:** [Wicofe-Kernel](https://github.com/Waizusora/Wicofe-Kernel).
- **Cross-component contract:** [WIC-SPEC-BOOT-FRENIT-001](https://github.com/Waizusora/Wicofe-Spec/blob/main/boot/FRENIT_INTEGRATION_CONTRACT_0.1.md).
- **Date:** 2026-11-18

## 1. Purpose and boundary

Frenit is the Wicofe userspace init system and planned PID 1 service supervisor. This document identifies ABI capability domains it will need. It does not create a parallel ABI, assign syscall numbers, select register conventions or freeze structure layouts.

The canonical syscall contract remains the [Wicofe Userspace ABI Specification Baseline](../SPECIFICATION_BASELINE.md). Wicofe-Kernel implements that contract. Frenit consumes the published userspace ABI and must not define private syscall numbers.

## 2. Required capability domains

The ABI design must cover these domains with explicit semantics before native PID 1 integration:

- Process creation/exec, process identifiers, exit status, wait/reap and orphan adoption visibility.
- Signal delivery, signal masks/handling where supported, and interruption/restart semantics for wait and I/O.
- File descriptors, pipes, filesystem access and process standard streams.
- A local IPC mechanism with a versioned, bounded message contract suitable for init/service control; transport selection remains open until separately specified.
- Monotonic clocks and timeout-safe waiting.
- Privilege and peer-credential inspection sufficient to authorize privileged service-management operations.
- System information and runtime capability discovery.
- Authorized system shutdown, reboot, poweroff and halt requests.

## 3. Design constraints

1. Use explicit-width types and C-compatible layouts for public ABI structures.
2. Define blocking, interruption, partial-completion, error and permission behavior per operation.
3. Distinguish process identifiers from stable process handles; define PID reuse safety.
4. Do not expose Rust compiler-private layouts.
5. Keep optional capabilities discoverable and independently versioned.
6. Do not copy Linux syscall numbers or claim Linux semantic/binary compatibility without a reviewed Wicofe contract.
7. Do not add an ABI entry solely because Frenit has a CLI command with a similar name; CLI commands are userspace interfaces, not kernel syscalls.
8. Do not assume Unix-domain sockets or any specific IPC wire format until the relevant ABI specification accepts it.

## 4. Optional capabilities

Cgroups, pidfd-equivalent handles, advanced namespaces, resource controls, socket activation primitives, watchdog interfaces and security sandboxing are optional until separately accepted and specified. Their absence must not block mandatory init operation if an equivalent required primitive is available.

## 5. Conformance

Each ABI operation consumed by Frenit must link to:

- an ABI specification and stability label;
- kernel implementation and supported target matrix;
- positive, negative, boundary, interruption and permission tests;
- userspace wrapper mapping where applicable.

Frenit host tests do not establish Wicofe ABI conformance. Native PID 1 support remains blocked until the relevant ABI entries are specified, reviewed, implemented and tested with Wicofe-Kernel.
