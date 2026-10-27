# CS4023 Operating Systems Threading

<p align="center">
  <img src=".github/logo.png" alt="lm051-cs4023-operating-systems-threading logo" width="128" />
</p>

C exercises on processes and threads for the CS4023 Operating Systems
module, from the LM051 Computer Systems degree at the University of Limerick.

## Contents

- `example1a.c` to `example4.c`: create child threads with their own
  stacks, wait for them, share a counter and run a producer and consumer.
- `copy.c`: copies a file using low-level read and write calls.
- `common`: shared helpers used by the examples.

## Tech

- C on Linux

## Building

Compile each example with GCC, for example:

```sh
gcc -o example1a example1a.c
./example1a
```

## License

Licensed under the Apache License 2.0. See [LICENSE](LICENSE).
