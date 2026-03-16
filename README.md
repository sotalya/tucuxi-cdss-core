# Tucuxi-Cdss-Core
[![License: AGPL v3](https://img.shields.io/badge/license-AGPL--3.0--or--later-blue.svg)](https://www.gnu.org/licenses/agpl-3.0.html)


This project exploits submodules, so do not forget to clone with the --recursive flag.

## Compilation

### Linux

First, execute the setup.sh file, in the root directory.

	./setup.sh

Then use cmake to compile:

	mkdir build
	cd build
	cmake ..
	make -j$(nproc)

To build the tests:

	mkdir build_tests
	cd build_tests
	cmake ../test/gtest/
	make -j$(nproc)

### Windows


First, execute the setup.bat file, in the root directory.

	./setup.bat

Then use cmake to compile:

	mkdir build
	cd build
	cmake -GNinja ..
	cmake --build .

To build the tests:

	mkdir build_tests
	cd build_tests
	cmake -GNinja ../test
	cmake --build .


## License

This project is licensed under the GNU Affero General Public License v3.0 or later (AGPL-3.0-or-later).

See the [LICENSE](LICENSE) file for the full text.

## Third-Party Licenses

This project includes third-party components.
See the NOTICE file for details.
