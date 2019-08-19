# About
This is a naive attempt at creating an application that can differentiate between "yes" and "no" by comparing STE and ZCR. 

# Usage
1. Compile main.cpp using the command
    g++ main.cpp
2. Run the code using ./a.out along with the 3 files(yes file, no file, test file)
    ./a.out yes_1 no_1 yes_2

# About files
The Data directory has a subdirectory - input which has input files of yes and no recorded from CoolEdit.
The subdirectory "raw" has STE and ZCR before removal of noise.
The subdirectory "normalised" has STE and ZCR after removal of noise. This directory is used for processing.