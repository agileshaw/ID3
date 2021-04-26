# ID3
An implementation of ID3 decision tree algorithm in C. The program is implemented and tested under Ubuntu 18.04.

## Usage
### Compile with Makefile
To compile the scripts, run:
```bash
make all
```
To remove the intermediate files created during compiling process, run:
```bash
make clean
```

### Run Program
After compilation, an executable program namely `classifier` will be created. This program can take in 3 arguments: seed number, treefile folder path, dataset path. 

By default, if running the program without any arguments, the program will use a randomly generated seed number, read in `adult+stretch.csv` dataset in `data/` folder, and create a file with inferred decision tree in the current directory. 
```bash
./classifier
```

To specify a folder to save the decision tree, run with `-t` flag. Example:
```bash
./classifier -t /home/Documents/tree
```
**Note**: Please do not add an extra `/` at the end of the path.

To input a desired seed number (usually used when trying to reproduce a previous result), run with `-s` flag. Example: 
```bash
./classifier -s 3509273525
```

To use a dataset as training set, run with `-d` flag. Example: 
```bash
./classifier -d data/breast-cancer.csv
```
**Note**: The dataset is required to have the first row specifying feature names and to use `,` as delimiter.

Here is an example when passing in all three arguments:
```bash
./classifier -s 3509273525 -d data/breast-cancer.csv -t /home/Documents/tree 
```