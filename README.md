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

A config file will be created at `~/.config/ds4led/ds4led.json` if it doesn't already exist. The newly created config will contain only the default DS4 led color `{"default": "#000040"}`. You can add your own presets in a similar fashion.

```bash
$ ds4led 000040 # use a valid RGB hex string

$ ds4led 0 0 64 # use a valid (0-255, 0-255, 0-255) R G B string

$ ds4led -l # will list presets

$ ds4led -p <preset name> # will apply the chosen preset
```

# TODO
1. Implement config
2. Implement presets
3. Implement daemoning