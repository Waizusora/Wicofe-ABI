# Syscall Calling Convention Technical Review 0.1

- **Status:** Review complete; proposal remains Proposed and is not an adopted ABI.
- **Reviewed proposal:** [Syscall calling convention proposal 0.1](SYSCALL_CALLING_CONVENTION_PROPOSAL_0.1.md)
- **Architectures:** x86-64 and AArch64.
- **Decision:** Carry-flag error signaling is architecturally feasible on both targets, subject to the mandatory kernel and compiler constraints below. This review does not freeze the ABI.

## 1. Executive finding

The proposed result encoding is viable:

- Success: operation result in the primary result register; carry clear.
- Failure: positive Wicofe errno in the primary result register; carry set.
- The carry bit is an out-of-band discriminator, so a successful negative signed result does not collide with an error.

The architecture manuals support preserving the relevant condition state across the respective return mechanisms. This is not sufficient by itself to approve the ABI: the kernel return path, signal context behavior, compiler stubs, and interrupt/restart semantics must all be tested.

## 2. x86-64 findings

SYSCALL saves the post-instruction RIP in RCX and RFLAGS in R11, then applies the OS-configured IA32_FMASK mask to the live kernel RFLAGS. SYSRETQ restores the user return RIP from RCX and user RFLAGS from R11. Therefore CF can be used as the result discriminator if the kernel deliberately writes the result CF into the saved return RFLAGS image.

Mandatory constraints:

1. The syscall entry path must save the incoming user RFLAGS before using arithmetic or compare instructions that modify flags.
2. The return path must set or clear CF in the saved user RFLAGS image after all kernel work that can modify condition flags.
3. The return path must not assume that every return can safely use SYSRETQ. Validate the user RIP and use a fully specified safe IRETQ fallback for non-canonical or otherwise invalid return state.
4. The IA32_FMASK configuration must be documented. It controls kernel-entry flags; it must not accidentally erase the saved user CF result.
5. RCX and R11 are clobbered. The syscall stub must expose those clobbers to the compiler.
6. The raw-call stub must capture CF as an output of the same compiler-visible assembly operation as SYSCALL. A separate C comparison after returning is invalid because compiler-generated instructions may overwrite flags.
7. The syscall ABI must explicitly state the user condition-code clobber/result contract. Do not promise preservation of user arithmetic flags unless the kernel restores them by a separate mechanism.

SYSRETQ flag restoration and its architectural constraints are described by the Intel SDM. See [Intel instruction reference](https://kib.kiev.ua/x86docs/Intel/SDMs/252046-012.pdf) and [SYSRET instruction summary](https://x86-64.net/insn/sysret/).

## 3. AArch64 findings

On exception entry to EL1, PSTATE is captured in SPSR_EL1. ERET restores PSTATE from SPSR_EL1, including the condition flags. Consequently, the kernel can return the syscall result in X0 and the error discriminator in PSTATE.C.

Mandatory constraints:

1. The exception entry code must retain the original user SPSR and must not confuse the saved entry C bit with the syscall result C bit.
2. Immediately before ERET, the kernel must set or clear SPSR_EL1.C according to the syscall result.
3. The userspace stub must read C in the same assembly block as SVC (for example, with a conditional-set instruction) before compiler-generated code can change condition flags.
4. X8 is the syscall identifier register and must be treated as clobbered by the stub. X0 is input/output. The preservation contract for X1-X5 and all other GPRs must be explicit and consistent with the actual kernel save/restore frame.
5. Signal delivery and signal return must preserve the defined user register and condition-state contract; signal-frame semantics cannot be inferred from ERET alone.

Arm documents that exception entry stores PSTATE in SPSR_ELx and exception return restores it; SPSR_EL1.C specifically carries PSTATE.C. See [Arm AArch64 Exception Model](https://documentation-service.arm.com/static/67ac57fb091bfc3e0a9479cc) and [SPSR_EL1 register description](https://www.df.lth.se/~getz/ARM/SysReg/AArch64-spsr_el1.html).

## 4. Compiler and assembly constraints

The syscall entry must be implemented as a small architecture-specific assembly boundary, not as ordinary C that assumes flags survive arbitrary compiler output.

Required implementation properties:

- One indivisible compiler-visible asm operation performs the trap and captures the carry result.
- Declare every register input, output, and clobber explicitly.
- Do not mark the operation as memory-free: syscall operations can read or write memory through user pointers. Use the appropriate compiler memory clobber or equivalent wrapper contract.
- Prevent the compiler from moving memory accesses across the syscall boundary when the syscall semantics require ordering.
- Keep architecture-specific assembly behind a single internal raw-syscall API.
- Verify GCC and Clang output at the supported optimization levels, including LTO if enabled.

No inline-assembly implementation is approved by this document. Exact constraints must be validated with the selected compiler versions and target triples.

## 5. ABI semantics that remain open

Before adoption, Wicofe-ABI must define:

- Whether condition flags other than the carry result are unspecified or preserved.
- Whether all general-purpose registers except the documented result/identifier/clobber set are preserved.
- Whether syscall entry is permitted while user interrupts/debug state are in particular configurations.
- How signal frames represent and restore user condition state.
- Whether kernel preemption, interrupts, and nested exceptions can alter the saved result flags.
- Exact error namespace, valid errno range, and mapping to ISO C/POSIX errno.
- Syscall restart and cancellation behavior per operation.
- ABI version/capability discovery and reserved syscall-number ranges.

## 6. Required acceptance test matrix

| Test | x86-64 | AArch64 |
|---|---:|---:|
| Successful syscall returns zero with carry clear | Required | Required |
| Successful syscall returns positive value with carry clear | Required | Required |
| Successful syscall returns negative signed value with carry clear | Required | Required |
| Error returns positive errno with carry set | Required | Required |
| Entry and return preserve documented GPRs | Required | Required |
| Return flags survive interrupt/preemption path | Required | Required |
| Invalid user return address takes safe return path | Required | Architecture-specific equivalent |
| Signal delivery/return preserves specified context | Required | Required |
| GCC and Clang optimized stub tests | Required | Required |
| Kernel and userspace agree on errno constants | Required | Required |

Tests must run under QEMU for both architectures before the convention can be accepted. Build-only compilation is not a substitute for runtime flag verification.

## 7. Recommendation

Retain carry-flag signaling as the leading candidate. It is preferable to reserving a negative return range when the ABI needs to represent the full signed result domain. Do not mark the proposal Accepted until:

1. The kernel entry/return implementation demonstrates the required flag behavior.
2. Compiler-specific raw syscall stubs pass the test matrix.
3. Signal-context and safe-return behavior are specified.
4. The canonical errno namespace and register preservation rules are agreed.
5. An explicit architecture decision records adoption.

Until then, Wlibc must not implement a production raw syscall backend against this proposal.
