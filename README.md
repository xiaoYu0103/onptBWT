# onptBWT:

Author: Xinwu Yu

## Credits

This project is based on OnlineRLBWT (https://github.com/itomomoti/OnlineRlbwt.git) by Tomohiro I.

### Download

The source codes in 'module' directory are maintained in different repositories.
So, to download all the necessary source codes, do the following:

```sh
git clone https://github.com/xiaoYu0103/onptBWT.git
cd onptBWT
git submodule init
git submodule update
```

### Compile

Compilation may require cmake version no less than 3.1, and a compiler supporting some features of C++14.
It has been tested with "Apple LLVM version 7.3.0 (clang-703.0.31)" and "g++6.3.0".

The following commands creates the executable in the build directory (default build type is release).

```sh
mkdir build
cd build
cmake ..
make
```

### Usage

Executables (without option shows help).

```sh
./OnlineRlbwt
./OnlineRindex
./OnlineRindex_Demo
./OnlineLz77ViaRlbwt
./DecompressLz77
./onptBWT
```

### onptBWT



```sh
./onptBWT [options]  #   The input file must be in standard FASTA format.
```

| Option     | Short | Required | Default | Description                                                             |
| ---------- | ----- | -------- | ------- | ----------------------------------------------------------------------- |
| `--input`  | `-i`  | Yes      | -       | Input FASTA file path.                                                  |
| `--output` | `-o`  | No       | -       | Output BWT file path. If not specified, the BWT is not saved to a file. |
| `--check`  |       | No       | 0       | Check correctness (1=enabled, 0=disabled).                              |
| `--help`   |       | No       | -       | Show help message.                                                      |

### Examples

1. **Basic usage** (compute BWT and save to file):
   ```sh
   ./onptBWT -i genome.fasta -o output.bwt
   ```
2. **Enable correctness check**:
   ```sh
   ./onptBWT -i genome.fasta -o output.bwt --check=1
   ```
3. **Compute without saving to file**:
   ```sh
   ./onptBWT -i genome.fasta
   ```



