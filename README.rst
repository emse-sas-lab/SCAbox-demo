SCAbox Acquisition Demo
***************************************************************

.. image:: https://img.shields.io/github/license/emse-sas-lab/SBAbox-demo
    :target: https://choosealicense.com/licenses/mit/
    :alt: license

`Website <https://emse-sas-lab.github.io/SCAbox/>`_

Overview
===============================================================

This repository contains a demo design and application for side-channel acquisition. 
It is part of the `SCAbox <https://emse-sas-lab.github.io/SCAbox/>`_ project.

Features
===============================================================

Block Design
---------------------------------------------------------------

- Time-to-Digital Converter based sensor (TDC)
- Advanced Encryption Standard (AES)
- Manual placement for sensors

Embedded
---------------------------------------------------------------

- Debug and acquisition CLI application
- Serialization of acquisition data
- `Tiny-AES <https://github.com/kokke/tiny-AES-c>`_
- `OpenSSL AES <https://www.openssl.org/>`_
- `Dhuertas AES <https://github.com/dhuertas/AES>`_

Install
===============================================================

In order to install this demo you must follow the `installation tutorial <https://emse-sas-lab.github.io/SCAbox/tuto/installation.html>`_.

Usage
===============================================================

The demo contains an example FPGA design that implements a sensor and a crypto-core.

It is is driven by a C application allowing the user to input commands to run synchronous acquisition.

The data is printed into a serial port in order to be retrieve in your computer.

If you want to launch this demo follow the `acquisition tutorial <https://emse-sas-lab.github.io/SCAbox/tuto/installation.html>`_.

More
===============================================================

SCAbox is a project on the topic of side-channel analysis.
The goal of SCAbox is to provide a cheap and efficient test-bench for side-channel analysis.

To learn more about SCAbox please visit our `website <https://emse-sas-lab.github.io/SCAbox/>`_.
It provides a tutorials and a wiki about side-channel analysis.

SCAbox is an open-source project, all the sources are hosted on GitHub

- `IP repository <https://github.com/emse-sas-lab/SCAbox-ip/>`_
- `Acquisition demo <https://github.com/emse-sas-lab/SCAbox-demo/>`_
- `Attack demo <https://github.com/emse-sas-lab/SCAbox-automation/>`_
- `SCAbox website  <https://github.com/emse-sas-lab/SCAbox/>`_

Contributing
---------------------------------------------------------------

Please feel free to take part into SCAbox project, all kind of contributions are welcomed.

The project aims at gathering a significant number of IP cores, crypto-algorithms and attack models 
in order to provide an exhaustive view of today's remote SCA threat.

Software and embedded improvements are also greatly welcomed. Since the project is quite vast and invovles
a very heterogeneous technical stack, it is difficult to maintain the quality with a reduced size team.  

License
---------------------------------------------------------------

All the contents of the SCAbox project are licensed under the `MIT license <https://choosealicense.com/licenses/mit/>`_ provided in each GitHub repository.

Copyright (c) 2020 Dahoux Sami
