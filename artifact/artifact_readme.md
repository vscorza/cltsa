# Tool

The current tool is inteded for modeling, verifying, synthesizing and diagnosing automata-based specifications through the use of a mix of FSP and LTL syntax (CFSP).

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 

### Prerequisites

In order to compile the source code you will need the following

```
make
gcc
```

### Installing

A clean version of the tool should be built by simply running
```
make 
```

To check that the compilation was succesful you can run this command:

```
./artifact -h
```

It should print a help message indicating you to use the tool.

# Running the benchmark for CONFNAME

The set of tests to be run can be found in *tests*, to generate the results that were used in the presented publication run:

```
runt_test_cases.sh
```
This script will output its results in *results* where the following summaries can be found:

```
realizable_data_composite.csv
unrealizable_data_composite.csv
```

### Running a particular specification

If you want to parse and verify/sinthesize a particular specification, suppose it is stored in a file named *spec.cfsp* you need to run:

```
./artifact -r filename.fsp
```


## Authors

XXX


