# CLTSA

CLTSA is intended as tool for modeling, verifying and synthesizing reactive systems through the use of a mix of FSP and game structures LTL syntax.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes. 

### Prerequisites

In order to compile the source code you will need the following

```
make
gcc
lex
bison
```

### Installing

A clean version of the tool should be built by simply running
```
cd src
make all
```

To check that the compilation was succesful you can run this command:

```
./cltsa -h
```

It should print a help message indicating you to use the tool.

## Running the tests

If you want to run the tests execute the tool with the following parameters:
```
./cltsa --all-tests
```
Will run all the tests.
```
./cltsa --func-tests
```
Will run the functional tests.
```
./cltsa --load-tests
```
Will run the load tests.
### Running a particular specification

If you want to parse and verify/sinthesize a particular specification, suppose it is stored in a file named *spec.cfsp* you need to run:

```
./cltsa -r spec.cfsp
```


## Authors

* **Mariano Cerrutti* - *PhD Student at LaFHIS* - [LaFHIS Web](https://lafhis.dc.uba.ar/home)


