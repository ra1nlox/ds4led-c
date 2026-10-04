# ds4led

The same ds4led, but rewritten in C, because I wanted to.

Works only on linux

# Build

## Main way

1. Build nob:
   ```bash
    $ cc nob.c -o nob
   ```
2. Run nob:
   ```bash
   $ ./nob
   ```

## Alternative way

Build directly:
```bash
$ cc -lm main.c -o ds4led
```

# Usage
```bash
$ ds4led 000040 # use a valid RGB hex string

$ ds4led 0 0 64 # use a valid (0-255, 0-255, 0-255) R G B string
```

# TODO
1. Implement config
2. Implement presets
3. Implement daemoning