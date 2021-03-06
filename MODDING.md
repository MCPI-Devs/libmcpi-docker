# Modding
Modding Minecraft: Pi Edition is possible by patching the binary at runtime. To make this easier ``minecraft-pi-dcoker`` includes a libary called ``libcore.so`` which provides several functions to help you patch the game.

## Hex Addresses
Minecraft: Pi Edition has no symbols so you must patch the hex address of an instruction instead of using a function name. Hex addresses can be found using tools like [Ghidra](https://ghidra-sre.org) or [RetDec](https://retdec.com). To find out what a function does, you can find its equivalent in Minecraft: Pocket Edition 0.6.1 and use its name for reference because Minecraft: Pocket Edition 0.6.1 includes symbols.

## Loading Directories
``minecraft-pi-docker`` loads mods from two locations, ``/app/minecraft-pi/mods``, and ``~/.minecraft/mods``. The first location only exists in the Docker container and is immutable, so you should place your mods in ``~/.minecraft/mods`` which is mounted on the host as ``~/.minecraft-pi/mods``.

## ``libcore.so`` API
Header files and the shared library can be download from [Jenkins](https://jenkins.thebrokenrail.com/job/minecraft-pi-docker/job/master/lastSuccessfulBuild/artifact/out/lib).

### ``void *overwrite(void *start, void *target)``
This method replaces a function with another function.

#### Parameters
- **start:** The function you are replacing.
- **target:** The function you are replacing it with.

#### Return Value
The original contents of the function.

#### Warning
This should never be used on functions that are only 1 byte long because it overwrites 2 bytes.

#### Example
```c
static int func_injection(int a, int b) {
    return a + 4;
}

__attribute__((constructor)) static void init() {
    overwrite((void *) 0xabcde, func_injection);
}
```

### ``void revert_overwrite(void *start, void *original)``
This allows you to revert ``overwrite()``. This can be used to call the original version of a function.

#### Parameters
- **start:** The function that was overwritten.
- **original:** The return value of ``overwrite()``.

#### Return Value
None

#### Example
```c
typedef int (*func_t)(int a, int b);
static func_t func = (func_t) 0xabcde;
static void *func_original = NULL;

static int func_injection(int a, int b) {
    revert_overwrite((void *) func, func_original);
    (*func)(a, b);
    revert_overwrite((void *) func, func_original);

    return a + 4;
}

__attribute__((constructor)) static void init() {
    func_original = overwrite((void *) func, func_injection);
}
```

### ``void patch(void *start, unsigned char patch[])``
This allows you to replace a specific instruction.

#### Parameters
- **start:** The target instruction.
- **patch:** The new instruction (array length must be 4).

#### Return Value
None

#### Example
```c
__attribute__((constructor)) static void init() {
    unsigned char patch_data[4] = {0x00, 0x00, 0x00, 0x00};
    patch((void *) 0xabcde, patch_data);
}
```
