# Sort

Demonstration of several sorting algorithms. Helps you to gain a better understanding of them.

Compile the program with `make`.

### main

The main program to play with sorting algorithms.

### benchmark

Measure and compare speed of algorithms with a given input size.

### measure

Generate an array consisting running time for different input sizes.

The output is compatible with GNU Octave and can be visualized later.

For example, with the output file (`sort.txt`) in the current directory of Octave:

```
load sort.txt;
plot(bubble_small(:,1),bubble_small(:,2),'r');
hold on;
plot(selection_small(:,1),selection_small(:,2),'b');
plot(insertion_small(:,1),insertion_small(:,2),'k');
legend('Bubble sort','Selection sort','Insertion sort');
print -dpng bubble-selection-insertion-1.png;
```
