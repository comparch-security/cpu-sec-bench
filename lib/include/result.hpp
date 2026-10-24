// Process-status (return-code) vocabulary of the test suite.
//
// Every value a test case or the support library delivers as the process status
// belongs to one of the families below.  This header is their single home, so that a
// case, the caught-exception library and the registry cannot drift apart.  The
// published tables in README.md and doc/architecture.html document the same families.
//
//   0          RT_CODE_SUCCESS -- the attack succeeded.
//
//   1          The shared not-demonstrated constant: the attack was launched, the run
//              ended with the attack's intended effect absent, and the platform
//              delivered no signal -- no caught fault, no signal death, no execution of
//              the attacker's target.  Silence.  Whether a silent defense acted or the
//              mechanism never landed is deliberately not encoded here; the defender's
//              identity is not read off the exit code.
//
//   2-14       The case-local verdict space.  Any other failure value is local to the
//              case that delivers it and MUST be declared, with its meaning, in that
//              case's record in configure.json.  A precondition-failed case -- it could
//              not stage or measure the attack at all -- delivers its case-local value
//              here and never the shared 1.  Two carve-outs: the scheduler renders a 65
//              temp-file failure as 2, so a 2 in a set-var record's results may be
//              scheduler-generated; and a staging/calibration producer's failure
//              constant stays UNDECLARED by design, because declaring it would satisfy a
//              dependent's require gate and run the dependent on a failed input.
//
//   15-31      The caught-exception / support-library band, the named constants below.
//
//   32-63      Runtime-parameter delivery, 32 + value.  The scheduler consumes the code
//              into variables.json (set-var) and re-reads it as a -v argument.
//              Contract-mandated, never a verdict.  0..31 is the designed value range;
//              it is enforced at the producer only where the case declares no
//              out-of-band value (acc/get-ra-offset.cpp).  The two same-call-site
//              carriers deliver their measured frame offset as-is, which is how the
//              declared 251 exception (32 + 219) arises; the consumer side is not
//              range-enforced.
//
//   64-65      The temp-file variant of the same channel: 64 = the parameter was
//              written, 65 = it could not be.
//
//   256+signo  Signal deaths, encoded by the scheduler.  These are not codes.
//
//   1024/1025  Scheduler-owned: prerequisite unsatisfied (1024) and not run -- skipped
//              for the platform or could not be built (1025).
//
// EXCEPTIONS.  Two values sit outside 0-31 by documentation, not by accident:
// RT_CODE_MTE_TAGCHECK = 99 (hwasan's own tag-mismatch process value, declared on the
// cfi-code-injection-backed records) and 251 (the shipped ASan frame-shape value on the
// two return-to-parent-same-call-site records; registry-only, deliberately not a
// constant here).  255 is NOT a code: it is only the POSIX rendering of a negative or
// out-of-range process status, and it may never be declared in expect-results.
//
// COLLISION RULE.  Never reuse 15/16/17/18.  No status constant outside 0-31 except the
// named exceptions above.  No shared value may be minted for a case-local meaning: a new
// per-case verdict is a declaration in configure.json, not a constant here.
//
// `gvar` is control-flow state.  It may gate a branch and carry a calibration datum into
// the parameter channel, but it never becomes a verdict code.

#ifndef RESULT_HPP_INCLUDED
#define RESULT_HPP_INCLUDED

// the attack succeeded
const int RT_CODE_SUCCESS = 0;

// the caught-exception / support-library band
const int RT_CODE_MAPERR           = 15;  // the target address is not accessible
const int RT_CODE_ACCERR           = 16;  // no permission to access the target address
const int RT_CODE_MISMATCH         = 19;  // a fault matched no registration (diagnostic,
                                          // declared only when a designed scenario reaches it)
const int RT_CODE_LIBFAIL          = 20;  // the library's own install/reset/pool guard failed
const int RT_CODE_FAULT_CONTAINED  = 21;  // a fault from a corrupted frame was contained

// A match predicate, never a status: the kernel's MTE tag-check si_code.  The name is
// deliberately not SEGV_MTESERR -- glibc declares that enumerator in bits/siginfo-consts.h.
const int RT_SICODE_MTESERR = 9;

// Named out-of-band exception: hwasan's own process value for a tag mismatch, paired
// with the RT_SICODE_MTESERR predicate above.
const int RT_CODE_MTE_TAGCHECK = 99;

#endif
