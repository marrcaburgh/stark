# stark Roadmap

stark is an umbrella project growing in two directions: a C99+ utility
library today, and a programming language with its supporting toolchain
over time.

## Utility library

- further utilities added over time 

### stark_config

- config file parsing (broadly defined, implementation-specifics will be added here as it gets implemented)

### stark_cli_opts

This is all planned to be implemented by v1.0.0:

- help generation
- (done) option aliases for longhand/env/subcommands
- (done) option dependencies (via groups)
- dynamic array growth (heap mode only)
- validator arrays with ANY (OR) modifier, which includes callback arrays (ANY does not apply)
- (done) global options with optional disable toggle for subcommands

### stark_hash_table

This is all planned to be implemented by v1.0.0:

- tombstones and deletion
- (done) index tracking

## Toolchain

- stark programming language compiler
- LSP server
- related developer tooling
