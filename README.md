# Quine-McCluskey-gui
GUI implementation of the Quine-McCluskey algorithm for minimizing Boolean functions. To minimize, the algorithm combines minterms into implicants and further combines implicants, giving us a reduced boolean function. Further details can be found in at [Wikipedia](http://en.wikipedia.org/wiki/Quine-McCluskey_algorithm "Quine-McCluskey in detail").  

## Examples
The gif below demostrates the minterm and table entries for an EXCLUSIVE-OR (XOR) Boolean function, complimenting the table entries, and entering multiple functions (AND, NOR, NAND, XOR ). 
![alt tag](https://raw.github.com/svtanthony/Quine-McCluskey-gui/master/demo.gif)  

Example of usefulness with 4 variable function; both functions below are functionally equivalent.  

| Boolean Function  | Minimized Boolean Function |
|:------:|:-----------:|
| A'BC'D' + AB'C'D' + AB'C'D + AB'CD' + AB'CD +  ABC'D' + ABCD' + ABCD | BC'D' + AB' + AC |

## Usage
Key features include:  
* Tabular entries  
* Minterm/Maxterm entries  
* Sum-of-Products outputs  
* Products-of-Sum outputs  
* Compliment output

## Installation
Install `qt4-default` if not already installed.  
Type the following at the command prompt to compile:  
```
$ qmake  
$ make
```
