# souffle-ip-address

IP address functors for [souffle](https://souffle-lang.github.io/) datalog. For now IPv4 functionality only.

## Features

- parse symbol to Cidr
- print Cidr to symbol
- decide Cidr overlap

See `datalog/ffi/ipv4.dl`. For usage example, see `datalog/tests/ipv4.dl`.

## Usage

Run `make setup` the first time to build the docker container with dependencies.

Run `make prepare` to compile the functor library and run tests.

Run `make devenv` to get a shell within the docker with all deps present and current directory mounted.

