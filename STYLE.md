# stark Style Guide

This guide defines how code in this repository is written. Where a rule is
defined here, it overrides any global style configuration; anything not
covered here falls back to those global rules.

## Control flow whitespace

Control flow gets room to breathe:

- One blank line before an opening `if`, `else`, or loop header — except
  directly after another opener.
- One blank line after its closing brace, except between stacked closers,
  which stay line after line.
- Early returns sit on their own line, separated from the work above them:

```c
if (STARK_EXPECT_FALSE(opts == NULL)) {
  cli_opts_error(NULL, 0, "stark_cli_opts_parse", "cli_opts cannot be NULL");

  return false;
}
```

- Plain statements get no added whitespace.

`clang-format` preserves this spacing but never inserts it — apply it by
hand, especially around preprocessor boundaries. Wrap hand-tuned regions
such as argv tables in tests in `// clang-format off` /
`// clang-format on`.

## Branch depth

Maximum nesting depth is 4, counting the function body as level one; aim
for 3. Flatten with early exits and goto labels before reaching for helper
functions (`cli_opts_assign_opt_skip_val`, `hash_table_probe_retry`) — and
extract a helper only when that code is genuinely used in more than one
place (`cli_opts_error`, `cli_opts_probe`). Depth beyond 4 is acceptable
only when no other representation of the logic exists and flattening would
change the semantic structure of the code.

## Function ordering

Functions are laid out callees-first: nothing references a function defined
below it in the file. This makes every file read top-down as descending
call-graph layers — `internal/cli_opts.h` runs from `cli_opts_error` at the
top (reachable from nearly everything) down to `cli_opts_init`, with the
public implementations consuming them after. Reading bottom-up walks from
the entry point to the primitives.

When two functions must recurse into each other (`hash_table_probe` /
`hash_table_insert` during resize), break the cycle with one minimal
forward declaration instead of reordering.

## Variable declarations

Local variables are declared largest-first: pointers before non-pointers,
and among pointers, sorted by pointed-to type size. Declarators sharing a
base type may share a line:

```c
char *str = NULL, *delim = NULL, *rcp;
```

Struct member layout follows different rules — size, alignment, and cache
behavior take priority there. `stark_cli_opt` packs bitfields and places
its `_pad` / `_pad1` members explicitly where alignment demands rather than
following declaration ordering.

## Types

Every custom type pairs a tag with a `_t` typedef:
`struct stark_cli_opt` / `stark_cli_opt_t`. In declarations, use the tagged
form — parameters, casts, and compound literals all spell out
`struct stark_hash_table` or `enum stark_cli_opts_err`. The typedef name
belongs at the type's definition, not scattered through call sites.

## Naming

- Public symbols carry the library prefix: functions and type tags in
  snake_case under `stark_` (`stark_cli_opts_parse`), macros and enum
  constants in ALL_CAPS under `STARK_` (`STARK_CLI_OPTS_ENABLE_ENV`).
- Internal helpers are static, drop the library prefix, but keep the module
  prefix (`cli_opts_error`, `hash_table_probe`). Transient internal macros
  are ALL_CAPS without the prefix (`FLAG_VAL_TOKEN`, `LUT_TYPE_LH`) and are
  undefined again before the translation unit ends.
- Private struct members are underscore-prefixed (`_flags`,
  `_token_pool`); public members are bare.
- Names are short mnemonics: `htp`, `rcp`, `bkt`, single-letter loop
  counters. Goto labels are named `<function>_<phase>`
  (`cli_opts_init_loope`, `stark_cli_opts_parse_uopt`) so a jump reads as a
  state transition.

## Conditional compilation

- Feature-dependent code is always guarded — never leave a conditional
  branch bare.
- Conjunction guards state every condition explicitly:
  `#if defined(STARK_CLI_OPTS_ENABLE_HEAP) && defined(STARK_CLI_OPTS_ENABLE_ENV)`.
- Every `#endif` carries a trailing comment naming its opening condition;
  dual guards name both. `#else` branches do the same, naming the positive
  macro: `#else // STARK_CLI_OPTS_ENABLE_HEAP`.
- Configuration macros get `#ifndef` defaults documented in the header
  preamble (`STARK_CLI_OPTS_LH_LUT_SIZE`) so build systems can override
  them.
- Toggles self-clean: settings forced around an include are saved and
  restored (`DEF_SHTEH` / `UNDEF_SHTDEP`), and transient macros are removed
  through the `INTERNAL_*_UNDEF` tail blocks so nothing leaks past an
  inclusion.
- Include guards are named from the file path, uppercased
  (`STARK_INTERNAL_CLI_OPTS_H`).

## Comments

Comments serve exactly three purposes:

1. **Documentation** — module-level documentation lives in the header
   preamble, next to the configuration macros it describes.
2. **Semantic ordering** — category labels above macro groups
   (`// Optimization hints`), divider blocks around major regions, and
   condition names on guards. Labels sit above what they label; trailing
   labels are reserved for enum members.
3. **Explaining why** — reasoning not inferable from the code itself:
   layout guarantees (`// Fits into one 64-bit L1 cache line...`),
   lifetime requirements, deliberate omissions.

Everything else is narration of what the code already says and does not
belong. Use single-line `//` comments throughout, including multi-line
banners; reserve block comments for inline enum member labels.

## Error handling

Public operations return success or failure directly — a `bool`, or a
pointer that is null on failure. Details travel through an optional
out-parameter callers may pass NULL to ignore
(`enum stark_hash_table_err *rcp`). Diagnostics flow through exactly one
cold-path reporter per module (`cli_opts_error`, `hash_table_error`),
which formats into stack storage and either invokes the user's callback or
prints. Allocation failures are checked at the point of allocation and undo
any partial allocation made within the same operation. Conversions check
their error signals where they happen (`errno == ERANGE`).

## Formatting

Two-space indentation. Function braces on their own line; control-flow
braces attach to their header line; `else if` chains attach to the closing
brace. Case labels sit at their `switch`'s indentation. Empty loop bodies
are a lone semicolon on the next indented line. Designated initializers
list one member per line with trailing commas. Line continuations are
backslash-padded so continued arguments align vertically.

## Includes

Project headers first, then system headers — alphabetical within each tier,
tiers separated by a blank line. Feature-test macros (`#define _GNU_SOURCE`)
precede all includes. Every file includes what it uses, even when another
included header already provides it; system headers are exempt. Every
include must earn its place: nothing is included that the file does not
use.

## Headers and implementation structure

Library headers open with the standard banner: SPDX identifier, module
description, and copyright as `//` lines. Public API lives in
`include/stark/<module>.h`; the implementation compiles inside the public
header under `<MODULE>_IMPL`, defined in exactly one translation unit, and
pulls in `include/stark/internal/<module>.h`. An internal header may force
settings for its own includes by saving, overriding, and restoring the
caller's macros around the inclusion.

## Contributing and review

Unconventional constructs here are deliberate until proven otherwise —
goto-labeled state machines, hand-packed bitfields, comma-operator state
handling. If you believe something is a bug, verify it empirically through
the test suite before asserting it; otherwise report it as a potential
issue with your reasoning. Do not restructure unusual code into
conventional forms without demonstrating a real defect.
