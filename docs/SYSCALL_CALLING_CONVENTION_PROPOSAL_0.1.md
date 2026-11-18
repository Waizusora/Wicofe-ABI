# Wicofe Syscall Calling Convention Proposal 0.1

- **Status:** Proposed; not frozen or implemented.
- **Owner:** Wicofe-ABI.
- **Consumers:** Wicofe-Kernel, Wlibc, Wicofe-SDK, Wicofe-Lib.
- **Initial architectures:** x86-64 and AArch64.

## 1. Goals

Define a small, architecture-aware syscall entry contract for Wicofe that:
- supports the native Wicofe syscall namespace and data ABI;
- gives libc an unambiguous success/error result without reserving negative return values;
- supports up to six machine-word arguments;
- has explicit register clobber and preservation rules;
- does not claim Linux syscall-number, structure, semantic, or binary compatibility.

The register choices below use the architectures' conventional trap mechanisms. This does not make the Wicofe syscall ABI Linux-compatible: syscall identifiers, error signaling, public structures, semantics, executable ABI and versioning remain Wicofe-defined.

## 2. x86-64 proposal

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
- Other general-purpose registers are preserved unless a future ABI revision explicitly changes this rule.
- The syscall identifier namespace is assigned only by Wicofe-ABI. Linux syscall numbers must not be copied or treated as aliases.

## 3. AArch64 proposal

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
- The userspace stub reads the restored condition flags after `SVC` to distinguish success from failure.
- X1–X5 and all other general-purpose registers are preserved unless a future ABI revision explicitly changes this rule.
- The syscall identifier namespace is assigned only by Wicofe-ABI. Linux syscall numbers must not be copied or treated as aliases.

## 4. Common result semantics

- Success and failure are distinguished by the architecture's carry flag, not by the sign or numeric range of the result.
- On failure, the result register contains a positive errno value from the Wicofe errno namespace.
- Wlibc converts the raw failure to the POSIX convention: return the interface-specific failure value and set thread-local `errno`.
- The kernel must not write userspace TLS or a libc `errno` variable.
- The complete errno namespace, aliases, and values must be defined in a canonical ABI header before implementation.
- Restart, interruption, partial completion, blocking and cancellation behavior are specified per syscall; carry-flag signaling does not define those semantics by itself.

This convention permits successful signed values, including negative timestamps where an interface permits them, without colliding with an error range.

## 5. Syscall identifier policy

- Use a Wicofe-owned 32-bit identifier namespace carried in the architecture's syscall-number register.
- Assign identifiers centrally in this repository only after each interface specification is reviewed.
- Do not use Linux `__NR_*` tables or expose Linux syscall identifiers in public Wicofe headers.
- Reserve documented ranges for core ABI, architecture-specific operations and future extension; exact ranges remain undecided.
- Every identifier has an ABI introduction version and stability state.

## 6. Blocking and cancellation

The calling convention only defines entry and result transport. Individual syscall specifications must additionally define:
- whether the operation can block;
- which clock governs timeouts;
- whether and how signals interrupt it;
- whether partial progress is returned;
- whether the kernel may transparently restart it;
- how Wlibc cancellation points can interrupt a blocked operation without violating POSIX behavior.

Do not copy Linux futex, clone, restart-block or signal-frame semantics by assumption. Specify Wicofe equivalents based on the required POSIX behavior.

## 7. Required validation before adoption

1. Assembly-level x86-64 and AArch64 tests for syscall entry, result, carry state and register preservation.
2. Kernel return-path tests for both success and error cases.
3. Tests proving negative successful return values remain distinguishable from errors.
4. Interrupt and signal-return tests proving the carry flag survives the return path as specified.
5. Compiler/ABI tests for inline assembly constraints under supported GCC and Clang versions.
6. Review against the Wicofe executable ABI, signal ABI, and kernel entry implementation.

## 8. Open decisions

- Confirm whether CF/PSTATE.C error signaling is accepted.
- Define the canonical errno values and public header generation.
- Confirm exact preservation/clobber rules with the kernel entry implementation.
- Define syscall identifier ranges and ABI version negotiation.
- Define per-operation interruption/restart/cancellation behavior.

No implementation may hardcode this proposal until its status is changed to Accepted by an explicit architecture decision.
