Security Test Benchmark for Computer Architectures
========================

This project aims to provide a generic test benchmark to
systematically evaluate the security features
supported by a processor hardware.

Copyright (c) 2017-2023 [Wei Song](mailto:wsong83@gmail.com) at
the Institute of Information Engineering,
Chinese Academy of Sciences.

The software can be distributed under GNU General Public License version 3.
See [LICENSE](LICENSE.md) for more details.

### Usage (Linux variants)

* Make the C++ `run-test` helper program

~~~bash
  make
~~~

* Run the test bench in exhaustive mode with a report

~~~bash
  ./run-test exhausted-run
~~~

* Run the test bench in fast mode with a report

~~~bash
  ./run-test fast-run
~~~

* Extra functions provided by the `run-test` helper

~~~bash
Usage: ./run-test [OPTION]
Run Script for the Security Test Benchmark.

Possible parameters:
  help          Show this help information.
  continue      Continue a previous test by reading the results.json file first.
  debug         Stop testing on the first unexpected exit status.
  make-only     Make the test cases without running them.
  no-make       Due to make the test cases as they are made aleady.
  corpus        Build-elsewhere mode: snapshot every build as test/<program>__<key> (with make) and run those prebuilt files (without make).
  fast-run      Only run the test case that their requirement runs successfully, and then generate a report.
  exhausted-run Run all tests until the total test case is exhausted, and then generate a report.
  print-trace   Print the trace log of the attacked target.
~~~

* Extra targets provided by the `Makefile` script

~~~text
dump        Generate a dissambly dump for all test cases.
prep        Generate the preprocessed C++ files (resolving all macros) for all test cases.
clean       Clean up the test environment for a fresh test.
~~~

### Running on NEMU (riscv64 Linux guest)

The suite also runs inside a booted Linux guest under XiangShan NEMU, for venues that have no
compiler: the corpus is cross-compiled on the host, baked into a bootable image together with
the riscv64 scheduler, and executed in the guest. The scheduler's `corpus` option is what splits
the two phases.

~~~bash
  make
  ./run-test corpus make-only exhausted-run     # build host: snapshot every build unit
  ./run-test no-make corpus exhausted-run       # run host: execute the prebuilt corpus
~~~

See [platform/riscv64-nemu-linux/README.md](platform/riscv64-nemu-linux/README.md) for the
pinned image recipe (`build-corpus.sh` → `build-image.sh` → `run.sh`), the host-native
reference runner, and the known caveats of that venue.

### Usage (Windows MSVC)

We assume MSVC 2019 or later is installed.
The test bench currently relies on GNU Make is made available through the winget:

~~~bash
  winget install gnuwin32.make
~~~

Assuming the test bench is run in a Windows PowerShell lunched by MSVC
(Suggested by Microsoft, a shell must be lunched through MSVC),
the GNU Make needs to be added to the PATH variable:

~~~bash
  $Env:PATH += ";C:\Program Files (x86)\GnuWin32\bin"
~~~

Once this is done, we try to make the Makefile works on Windows as well.
For the obvious reasons, some functions are unavailable but we are working on them.


### Content of the repo:
- **acc**: test cases related to memory access control.
- **cfi**: test cases related to control flow integrity.
- **cpi**: test cases related to code pointer integrity.
- **lib**: the architecture-specific support library.
- **mss**: test cases related to memory spatial safety.
- **mts**: test cases related to memory temporal safety.
- **platform**: venue scripts for running the suite outside a native host (the riscv64 Linux guest under NEMU).
- **doc**: documentation of the test suite.
- **plans**: archived implementation plans.
- **script**: helper scripts of the test suite.
- LICENSE.md: license declaration.
- Makefile: auto make script.
- README.md: this readme file.
- configure.json: the configuration parameters recorded in JSON.

### Documentation

Detailed documentation lives in the [doc](doc/content.html) directory:

- [Test catalog](doc/test-catalog.html) — every test case in the suite, by category.
- [Architecture](doc/architecture.html) — the scheduler, the configuration format and the
  result codes.
- [Porting](doc/porting.html) — the support library layout and how to add a new
  instruction set or operating system.

### Test results

The results of individual test cases are recorded in a generated file `results.json`.
The meaning of the result integer:

* 0: the attack succeeded — the vulnerability was exploited, so the platform does not provide
  the corresponding defense.
* 1: the attack was launched and its intended effect is absent, with no platform signal:
  silence. A `1` deliberately does not say whether a silent defense neutralized the attack or
  the attack's mechanism never landed — the argument sweeps, the `require` chains and the
  case-local values below (where a case can tell the two apart) carry that distinction, which
  no exit code can. The mss redzone family is the worked example: the out-of-bounds access
  simply did not land in the target buffer.
* 2-14: a case-local failure value, meaningful only for the case that delivers it; every such
  value is declared in its record's `expect-results` with its meaning.
* 15-31: the caught-exception and library band — the platform responded to the attack. The
  named values: `15` and `16` are the caught memory faults (mapping and access), `17`/`18` are
  in use by `cfi-call-wrong-vtable-released`, `19` is the handler's unmatched-fault
  *diagnostic* and is never declared as an expected result, `20` is a library configuration
  failure, and `21` is the fakefunc corruption's contained fault. The constants themselves
  live in `lib/include/result.hpp`.
* 32-63: a runtime parameter was discovered: the value is the code minus 32. The scheduler
  stores it in `variables.json` and treats the case as successful, so later cases can consume
  it through a `-v` argument. The producer bounds the value to `0..31` wherever the case
  declares no out-of-band value; that bound is not enforced at the consumer, and `251` is the
  shipped demonstration of a designed out-of-band delivery (`32 + 219`, the ASan frame
  offset).
* 64-65: the same discovery, but the parameter was written to a temp file instead of encoded
  in the status: `64` means the test case then passes, `65` means it fails with error `2`.
* 256-511: an exception is raised but uncaught, with the signal number as `n-256` — the
  process was terminated by a signal.
* 1024: the test case directly failed due to a failure in prerequisite cases.
* 1025: the case was not run: it is skipped on this platform, or it could not be built
  (scheduler-owned; never a record's own outcome, and never declared in `expect-results`).
* other: something is wrong and needs investigation.

The contract in full lives in [`lib/include/result.hpp`](lib/include/result.hpp) (the named
constants and the bands) and in [doc/architecture.html](doc/architecture.html) (the narrative,
the scheduler's own behaviour and the two named exceptions). Three rules complete it: `gvar`
is control-flow state and never becomes a verdict code; a caught fault *at* the target address
means the platform **denied** execution (a defense), while a fault *past* the entry proves the
injected code began executing (`0` — the attack succeeded); and `99` is the single *proactive*
declaration (the MTE/HWASan tag check), kept because no current lane can provoke it. `255` is
never declared anywhere: it is how POSIX renders a negative or out-of-range process status,
not a code.

### Contributors

In the alphabet order:

- Boya Li
- Ciyan Ouyang
- Da Xie
- Hao Ma
- Jiameng Ying
- Sihao Shen
- Zhuxin Yang
- Wei Song

### References:

1. 沈思豪, 解达, 宋威. [跨平台内存安全测试集设计](http://dx.doi.org/10.15888/j.cnki.csa.008840). 计算机系统应用, 2022, 31(10).
1. Wei Song, Jiameng Ying, Sihao Shen, Boya Li, Hao Ma, Peng Liu.
   _A comprehensive and cross-platform test suite for memory safety -- Towards an open framework for testing processor hardware supported security extensions_.
   [arXiv:2111.14072](https://arxiv.org/abs/2111.14072), November 2021.
1. Jianping Zhu, Wei Song, Ziyuan Zhu, Jiameng Ying, Boya Li, Bibo Tu, Gang Shi, Rui Hou, and Dan Meng.
   _[CPU security benchmark](https://wsong83.github.io/publication/comparch/secarch2018.pdf)_.
   In Proceedings of the 1st Workshop on Security-Oriented Designs of Computer Architectures and Processors,
   Toronto, ON, Canada, pp. 8–14, 2018.
