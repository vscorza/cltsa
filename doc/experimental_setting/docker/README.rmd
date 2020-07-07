# Behavior minimization experimental data

This docker image is intended to provide support to:

1. [Compile] the CLTSA tool which allows the user to model, verify and synthetize reactive models using a mix of ltl and fsp syntax 
2. [Run] the set of test cases constructed to validate the Behavior minimization technique for unrealizable gr(1) specifications
3. [Create] the `csv` and `tex` files from the aforementioned test cases

# Folder structure

Source code, test cases and results are distributed as following:

1. [Source files] required to compile the CLTS tool are located at `/src`, they are already built for the running container, but a corresponding Make file is provided in order to re-create the binaries if needed
2. [Test cases] and related scripts are located at `/r_sh`, if the user wants to re-run the test cases, the following command should be executed: `/r_sh/run_all_reports.sh "/src/tests/" "/tmp-results/" "/src/" 1 "docker"`
3. [Results] are stored in `/tmp-results`


