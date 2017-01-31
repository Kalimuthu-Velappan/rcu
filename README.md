Validation of RCU
=================


This repository aims at validating the Linux kernel's
Read-Copy-Update (RCU) mechanism. The above process is available as a technical
report [here](rcupaper.pdf).

Author: Michalis Kokologiannakis.

* [License](#license)
* [Requirements](#requirements)
* [Directories](#directories)
* [Contact](#contact)

<a name="license">License</a>
-----------------------------

The code at this repository is distributed under the GPL, version 2 or (at your option) later.
Please see the LICENSE file for details.

<a name="requirements">Requirements</a>
--------------------------------------

In order to run the tests in this repository you need the stateless model checking
tool [Nidhugg](https://github.com/nidhugg/nidhugg).

More information about this tool can be found at
[this paper](https://arxiv.org/abs/1501.02069).

<a name="directories">Directories</a>
-------------------------------------

* The `valtiny` directory aims at validating the Tiny RCU flavour.
* The `valtree` directory aims at validating the Tree RCU flavour.

<a name="contact">Contact</a>
-----------------------------

Feel free to contact me at: `mixaskok at gmail dot com`.
