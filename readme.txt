Timing Analyzer

Description:
    Given a gate-level netlist and a cell library, the timing analyzer perform the three steps below to show 
    the longest delay time and their corresponding paths under a given input pattern.
    
Delay Calculation:
    Step1: Build the delay graph according to the given netlist
    Step2: Calculate the delay and value of each node in the graph following topological order
    Step3: Output the longest and delay and their corresponding paths

Compile: (This will create executable "0866015.o")
    $ make

Usage:
    $ ./086615.o [path_to_netlist_file] [-p path_to_pattern_file] [-l path_to_cell_library]
      path_to_netlist_file: path to netlist file, e.g. ../benchmarks/c17.v
                        -p: path to pattern file, e.g. -p ../patterns/c17.pat
                        -l: path to cell library, e.g. -l ../lib/test_lib.lib

Implementation steps:
    1. Parse cell library and verilog circuit files. Each gate, cells, nets will be built with
       corresponding class, and recorded in a circuit class and a library class.
    2. Read input patterns and run each pattern.
    3. Start from input net, run through all gates connected to the net. And add the nets that are 
       connected to the gate into a queue. By first-in-first-out principal, we can assure that all
       signal required for afterwards gates are pre-generated.
    4. With the records of connections between gates and nets, we can decide the preceding and successing
       gates with the gate's input and output net. And calculate the input transition time and output 
       capacitance.
    5. With the rising/falling table in cell library we parsed, we can use extraploation and interpolation
       to calculate the cell delay and output transition time.
    6. When reaching the output nets, trace back the preceding gate and calculate the longest delay path as
       we did in step 4 until reach input nets. Finally, we compare all path to get the longest delay path.

