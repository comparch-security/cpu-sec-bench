Security Test Benchmark for Computer Architectures
========================

This project aims to provide a generic test benchmark to
systematically evaluate the security features
supported by a processor hardware.

Copyright (c) 2017-2021 [Wei Song](mailto:wsong83@gmail.com) at
the State Key Laboratory of Information Security,
the Institute of Information Engineering,
Chinese Academy of Sciences.

The software can be distributed under GNU General Public License version 3.
See [LICENSE](LICENSE.md) for more details.

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
- run-test.py: script to run the test suite.
