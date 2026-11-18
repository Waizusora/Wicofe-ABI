# Wicofe Syscall Calling Convention 0.1

- **Status:** Accepted design decision; implementation and validation pending.
- **Owner:** Wicofe-ABI.
- **Consumers:** Wicofe-Kernel, Wlibc, Wicofe-SDK, Wicofe-Lib.
- **Initial architectures:** x86-64 and AArch64.
- **Decision:** Use the architecture carry condition as the out-of-band syscall error discriminator on both initial architectures.

## 1. Goals

Define a small, architecture-aware syscall entry contract for Wicofe that:
- supports the native Wicofe syscall namespace and data ABI;
- gives libc an unambiguous success/error result without reserving negative return values;
- supports up to six machine-word arguments;
- has explicit register clobber and preservation rules;
- does not claim Linux syscall-number, structure, semantic, or binary compatibility.

The register choices use the architectures' conventional trap mechanisms. This does not make the Wicofe syscall ABI Linux-compatible: syscall identifiers, error signaling, public structures, semantics, executable ABI and versioning remain Wicofe-defined.

## 2. x86-64

Use the `SYSCALL` instruction.

| Purpose | Register |
|---|---|
| Syscall identifier | RAX |
| Argument 1 | RDI |
| Argument 2 | RSI |
| Argument 3 | RDX |
| Argument 4 | R10 |
| Argument 5 | R8 |
| Argument 6 | R9 |
| Return value / error number | RAX |
| Error indicator | RFLAGS.CF |
| Hardware-clobbered return state | RCX, R11 |

Rules:
- On success, the kernel returns the operation's result in RAX and clears CF.
- On failure, the kernel returns a positive Wicofe errno value in RAX and sets CF.
- The kernel returns with `SYSRETQ` or a validated equivalent path that restores the defined CF state.
- RCX and R11 are clobbered by the architectural instruction and must be declared as such by the userspace stub.
- The syscall stub must capture CF in the same compiler-visible assembly operation as `SYSCALL`.
- Other general-purpose register preservation and user condition-code clobber details must match the kernel's published entry-frame contract; they must not be assumed by libc until verified.
- The syscall identifier namespace is assigned only by Wicofe-ABI. Linux syscall numbers must not be copied or treated as aliases.

## 3. AArch64

Use `SVC #0`.

| Purpose | Register |
|---|---|
| Syscall identifier | X8 |
| Arguments 1–6 | X0–X5 |
| Return value / error number | X0 |
| Error indicator | PSTATE.C |
| Trap instruction | `svc #0` |

Rules:
- On success, the kernel returns the operation's result in X0 and clears PSTATE.C.
- On failure, the kernel returns a positive Wicofe errno value in X0 and sets PSTATE.C in the saved program status restored by `ERET`.
- The userspace stub captures C in the same assembly block as `SVC`, before compiler-generated code can change condition flags.
- X8 is the syscall identifier register and must be treated as clobbered by the stub. X0 is input/output.
- Preservation of X1–X5 and all other general-purpose registers must match the kernel's published exception-frame contract; it must not be assumed by libc until verified.
- The syscall identifier namespace is assigned only by Wicofe-ABI. Linux syscall numbers must not be copied or treated as aliases.

## 4. Common result semantics

- Success and failure are distinguished by the architecture's carry flag, not by the sign or numeric range of the result.
- On success, the primary result register contains the operation result and carry is clear.
- On failure, the primary result register contains a positive errno value from the Wicofe errno namespace and carry is set.
- Wlibc converts raw failure to the interface-specific POSIX failure value and sets thread-local `errno`.
- The kernel must not write userspace TLS or a libc `errno` variable.
- The canonical errno namespace, aliases, and values must be defined in this repository before syscall implementation.
- Restart, interruption, partial completion, blocking and cancellation behavior are specified per syscall; carry signaling does not define those semantics by itself.

This convention permits successful signed values, including negative timestamps where an interface permits them, without colliding with an error range.

## 5. Syscall identifier policy

- Use a Wicofe-owned 32-bit identifier namespace carried in the architecture's syscall-number register.
- Assign identifiers centrally in this repository only after each interface specification is reviewed.
- Do not use Linux `__NR_*` tables or expose Linux syscall identifiers in public Wicofe headers.
- Reserve documented ranges for core ABI, architecture-specific operations and future extension; exact ranges remain undecided.
- Every identifier has an ABI introduction version and stability state.

## 6. Blocking and cancellation

The calling convention only defines entry and result transport. Individual syscall specifications must additionally define whether the operation can block, which clock governs timeouts, signal interruption/restart, partial progress, and cancellation-point behavior. Do not copy Linux futex, clone, restart-block or signal-frame semantics by assumption.

## 7. Mandatory validation before implementation is considered complete

1. Assembly-level x86-64 and AArch64 tests for syscall entry, result, carry state and register preservation.
2. Kernel return-path tests for success and error cases.
3. Tests proving negative successful return values remain distinguishable from errors.
4. Interrupt and signal-return tests proving carry survives the return path as specified.
5. Compiler/ABI tests for supported GCC and Clang versions, including optimization and LTO configurations where supported.
6. Review against the Wicofe executable ABI, signal ABI, and kernel entry implementation.
7. QEMU runtime tests on both architectures; build-only compilation is insufficient.

## 8. Remaining specification work

The carry-flag decision is accepted. The following are still open and must be specified before the ABI is declared stable:
- canonical errno values and public header generation;
- signal-frame representation and restoration of user condition state;
- syscall identifier ranges and ABI version/capability discovery;
- signal-frame representation and restoration of user condition state;
- per-syscall interruption, restart, blocking and cancellation behavior.

No syscall numbers are implied or assigned by this calling-convention decision.
