Security Test Benchmark for Computer Architectures
========================

This project aims to provide a generic test benchmark to
systematically evaluate the security features
supported by a processor hardware.

Copyright (c) 2017-2022 [Wei Song](mailto:wsong83@gmail.com) at
the State Key Laboratory of Information Security,
the Institute of Information Engineering,
Chinese Academy of Sciences.

The software can be distributed under GNU General Public License version 3.
See [LICENSE](LICENSE.md) for more details.

#### Usage

* Make the C++ `run-test` helper program

~~~bash
  make
~~~

* Run the test bench with a report

~~~bash
  ./run-test report
~~~

* Extra functions provided by the `run-test` helper

~~~text
Usage: ./run-test [OPTION]
Run Script for the Security Test Benchmark.

Possible parameters:
  help        Show this help information.
  continue    Continue a previous test by reading the results.json file first.
  debug       Stop testing on the first unexpected exit status.
  make-only   Make the test cases without running them.
  no-make     Due to make the test cases as they are made aleady.
  report      Generate a report after finishing all test cases.
~~~

* Extra targets provided by the `Makefile` script

~~~text
dump        Generate a dissambly dump for all test cases.
prep        Generate the preprocessed C++ files (resolving all macros) for all test cases.
clean       Clean up the test environment for a fresh test.
~~~

#### Content of the repo:
- **acc**: test cases related to memory access control.
- **cfi**: test cases related to control flow integrity.
- **cpi**: test cases related to code pointer integrity.
- **lib**: the architecture-specific support library.
- **mss**: test cases related to memory spatial safety.
- **mts**: test cases related to memory temporal safety.
- **script**: helper scripts of the test suite.
- LICENSE.md: license declaration.
- Makefile: auto make script.
- README.md: this readme file.
- configure.json: the configuration parameters recorded in JSON.

#### Contributors

In the alphabet order:

- Boya Li
- Da Xie
- Hao Ma
- Jiameng Ying
- Sihao Shen
- Wei Song

#### References:

1. Wei Song, Jiameng Ying, Sihao Shen, Boya Li, Hao Ma, Peng Liu.
   _A comprehensive and cross-platform test suite for memory safety -- Towards an open framework for testing processor hardware supported security extensions_.
   [arXiv:2111.14072](https://arxiv.org/abs/2111.14072), November 2021.
2. Jianping Zhu, Wei Song, Ziyuan Zhu, Jiameng Ying, Boya Li, Bibo Tu, Gang Shi, Rui Hou, and Dan Meng.
   _[CPU security benchmark](https://wsong83.github.io/publication/comparch/secarch2018.pdf)_.
   In Proceedings of the 1st Workshop on Security-Oriented Designs of Computer Architectures and Processors,
   Toronto, ON, Canada, pp. 8–14, 2018.
