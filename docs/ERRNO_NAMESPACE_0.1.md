# Wicofe Errno Namespace 0.1

- **Status:** Accepted initial numeric assignments; namespace remains extensible.
- **Owner:** Wicofe-ABI.
- **Consumers:** Wicofe-Kernel, Wlibc, Wicofe-Lib, Wicofe-SDK.
- **Scope:** Raw syscall failure values and the userspace `errno` contract.

## 1. Rules

1. Errno values are positive, non-zero, Wicofe-owned 32-bit integers.
2. A raw syscall failure returns one of these values in RAX (x86-64) or X0 (AArch64), with the architecture carry flag set.
3. A successful syscall may return any value permitted by that syscall's result type, including negative signed values; carry is clear.
4. Wlibc sets the calling thread's `errno` to the raw positive value and returns the interface-defined failure sentinel.
5. The kernel never writes libc TLS or userspace `errno`.
6. Numeric values are identical on x86-64 and AArch64. They are not copied from Linux, BSD, or another host ABI.
7. Assigned values are permanent once published. Do not renumber or reuse them. New values are appended; gaps are reserved.
8. Aliases share the exact numeric value stated below. They are not separate errors.

## 2. Initial assignments

| Value | Symbol | Meaning |
|---:|---|---|
| 1 | EPERM | Operation not permitted |
| 2 | ENOENT | No such file or directory |
| 3 | ESRCH | No such process |
| 4 | EINTR | Interrupted system call |
| 5 | EIO | Input/output error |
| 6 | ENXIO | No such device or address |
| 7 | E2BIG | Argument list too long |
| 8 | ENOEXEC | Exec format error |
| 9 | EBADF | Bad file descriptor |
| 10 | ECHILD | No child processes |
| 11 | EAGAIN | Resource temporarily unavailable |
| 12 | ENOMEM | Not enough space |
| 13 | EACCES | Permission denied |
| 14 | EFAULT | Bad address |
| 15 | EBUSY | Device or resource busy |
| 16 | EEXIST | File exists |
| 17 | EXDEV | Cross-device link |
| 18 | ENODEV | No such device |
| 19 | ENOTDIR | Not a directory |
| 20 | EISDIR | Is a directory |
| 21 | EINVAL | Invalid argument |
| 22 | ENFILE | Too many open files in system |
| 23 | EMFILE | Too many open files |
| 24 | ENOTTY | Inappropriate I/O control operation |
| 25 | EFBIG | File too large |
| 26 | ENOSPC | No space left on device |
| 27 | ESPIPE | Invalid seek |
| 28 | EROFS | Read-only file system |
| 29 | EMLINK | Too many links |
| 30 | EPIPE | Broken pipe |
| 31 | EDOM | Mathematics argument out of domain |
| 32 | ERANGE | Result too large or too small |
| 33 | EDEADLK | Resource deadlock would occur |
| 34 | ENAMETOOLONG | Filename too long |
| 35 | ENOLCK | No locks available |
| 36 | ENOSYS | Function not implemented |
| 37 | ENOTEMPTY | Directory not empty |
| 38 | ELOOP | Too many levels of symbolic links |
| 39 | ENOMSG | No message of the desired type |
| 40 | EIDRM | Identifier removed |
| 41 | EOVERFLOW | Value too large for defined data type |
| 42 | ECANCELED | Operation canceled |
| 43 | EILSEQ | Illegal byte sequence |
| 44 | ENOTSOCK | Socket operation on non-socket |
| 45 | EDESTADDRREQ | Destination address required |
| 46 | EMSGSIZE | Message too large |
| 47 | EPROTOTYPE | Protocol wrong type for socket |
| 48 | ENOPROTOOPT | Protocol not available |
| 49 | EPROTONOSUPPORT | Protocol not supported |
| 50 | ESOCKTNOSUPPORT | Socket type not supported |
| 51 | EOPNOTSUPP | Operation not supported |
| 52 | EPFNOSUPPORT | Protocol family not supported |
| 53 | EAFNOSUPPORT | Address family not supported |
| 54 | EADDRINUSE | Address already in use |
| 55 | EADDRNOTAVAIL | Cannot assign requested address |
| 56 | ENETDOWN | Network is down |
| 57 | ENETUNREACH | Network is unreachable |
| 58 | ENETRESET | Network dropped connection on reset |
| 59 | ECONNABORTED | Connection aborted |
| 60 | ECONNRESET | Connection reset |
| 61 | ENOBUFS | No buffer space available |
| 62 | EISCONN | Socket is connected |
| 63 | ENOTCONN | Socket is not connected |
| 64 | ETIMEDOUT | Connection timed out |
| 65 | ECONNREFUSED | Connection refused |
| 66 | EHOSTUNREACH | Host is unreachable |
| 67 | EALREADY | Operation already in progress |
| 68 | EINPROGRESS | Operation in progress |
| 69 | ESTALE | Stale file handle |
| 70 | EDQUOT | Reserved for quota errors |

## 3. Aliases

- `EWOULDBLOCK` is an alias of `EAGAIN` (11).
- `ENOTSUP` is an alias of `EOPNOTSUPP` (51).

No other aliases are defined by this baseline. Architecture or subsystem code must not introduce private errno values.

## 4. Reserved range and extensions

- Values 71 through 255 are reserved for future common ABI assignments.
- Values 256 through 4095 are reserved for future subsystem-specific assignments only after review in this repository.
- Values 4096 and above are reserved for internal experiments and must never escape through a stable public ABI.
- An unrecognized raw error must be treated by Wlibc as an ABI/runtime defect; it must not be silently remapped to an unrelated standard errno.

## 5. POSIX mapping

The symbols and meanings are intended to support the Wicofe ISO C17 / POSIX.1-2024 profile. POSIX does not require these numeric values to match another operating system. The public C header must expose the required symbolic constants, while the kernel and libc share this canonical numeric table.

This initial table is not a claim that every POSIX-required errno has already been assigned. Before implementing a syscall or libc interface that can produce an additional required error, add its canonical assignment here and add cross-architecture tests.

## 6. Required tests

- Compile-time uniqueness check for all non-alias values.
- Compile-time alias equality checks.
- x86-64 and AArch64 agreement for every numeric assignment.
- Raw syscall error decoding sets thread-local `errno` and returns the interface failure sentinel.
- A successful negative signed result leaves `errno` unchanged and is not decoded as an error.
- Unknown raw error values are detected in debug/test builds.
