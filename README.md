# DigitalIntegScheme

## Build

On linux, try:

```bash
make
```

which will also run some tests that you can find in `./examples`.
Otherwise:

```bash
g++ -g -o main main.cpp tokenizer.cpp parser.cpp interpreter.cpp
```

You should now have an executable called main.

## Run

You can run it with a filem like so:

```bash
./main ./examples/ic1.txt
```

It's possible to run it through stdin by not providing a file:

```bash
./main
```

If ran through stdin, you must escape with `ctr-d`, as `fopen` blocks.
When running a `FIND` command with a `.csv` file, this file must be placed in `./csvFiles`.
You can also `CLEAR` the program name space, making it possible to reuse function names.

## Implementation

The program is interpreted, using the following strategy:

1. Tokenize a command
2. Parse the command
3. Interpret the command

A command is a logical unit that starts with `DEFINE`, `RUN`, `CLEAR`, `FIND`, `ALL`.
There is a special unit `TRIVIAL` which does nothing.

The following `std` structures were also used `vector`, `string`, `optional`, `pair`.
