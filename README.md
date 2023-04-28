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

## Rationale

Datalog can express rules to derive transitive information about facts.
For example, starting from single-hop network routes, one could compute reachability information between all nodes.
This can then be utilized in various ways, such as compliance checks or troubleshooting.

But one typically needs to deal with IP addresses, CIDR network ranges, and decide if they are overlapping in order to evaluate if a given routing action applies or not. Those operations (and the required data types) are missing from Souffle, but this repository aims to add them.

Here are some resources that discuss network route discovery using datalog:

- [Reachability analysis for AWS-based networks](http://www0.cs.ucl.ac.uk/staff/b.cook/CAV19.pdf) - AWS seems to have considered implementing some of their network checking services via datalog (they compare Souffle with some more specific representations). They mention ~200 rules to cover the cases. Mentions Souffle scales to 10K nodes, and sometimes beyonds.

- [Some more slides about networking and datalog](https://boonloo.cis.upenn.edu/research/talks/fmin-loo.pdf)

### Batfish hat-tip

On a related note, while not datalog, but [Batfish](https://www.batfish.org/) is an excellent tool to simulate network reachability on multiple levels, taking real-world routing tables as input. See [bf-aws-snapshot](https://github.com/ratulm/bf-aws-snapshot) which can extract routing tables from AWS networking components.

### Some other datalogs

- [differential-datalog](https://github.com/vmware/differential-datalog) is a variant which can take incremental updates of facts and output incremental changes in response.

- [ichiban/prolog](https://github.com/ichiban/prolog) is a golang-embedded prolog interpreter. Not datalog, but a notable mention.

- [Rego](https://www.openpolicyagent.org/docs/latest/policy-language/) is a datalog-inspired policy language that deals well with structured data like JSON. But its focus is near-linear evaluation speed, so it can't compute transitive closures, which makes it less suitable for mass-deriving data.
