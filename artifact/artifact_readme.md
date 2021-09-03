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

The script will synthesize realizable specifications and then compile all the metrics in *realizable_data_composite.csv*, then it will minimize unrealizable specificactions and save their results in *unrealizable_data_composite.csv".

### Getting more output for a particular specification

If the reader wants to inspect the output of a specific file, a plain text representation of an automaton can be generated by adding this line to the *fsp* specification file:

```
STRAT >> "path/to/folder/strat.rep".
```

Where *STRAT* is the name of the automaton to be exported and *path/to/folder/strat.rep* the full path to the destination file.
A similar syntax can be used to generate a file with some metrics related to the automaton:

```
STRAT >>_m "path/to/folder/strat.met".
```

This command will also create a friendlier *html* file that can be open with a browser.



### Running a particular specification

If you want to parse and verify/sinthesize a particular specification, suppose it is stored in a file named *spec.cfsp* you need to run:

```
./artifact -r filename.fsp
```




