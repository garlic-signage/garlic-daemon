# Code Conventions

Short and simple rules for the garlic-daemon source code.

## Language

- **C++23**, without GNU extensions (set in `CMakeLists.txt`).
- Minimum compiler is **GCC 14**. Only use features GCC 14 supports.
  Clang is not supported as compiler.
- Minimum tools: CMake 3.20, clang-format 19.
- Fine to use, e.g.: `std::print`, `std::format`, `std::expected`, `std::jthread`,
  `std::ranges::to`, `starts_with`/`ends_with`.
- Not available or only experimental on GCC 14, so do not use:
  `import std` / modules, `std::flat_map`, `std::mdspan`, `std::stacktrace`.
- Prefer the C++ standard library over C functions where it offers the same,
  e.g. `std::filesystem` instead of `opendir`, `std::ifstream` instead of `fopen`.
- System calls without a C++ equivalent (`socket`, `reboot`, `mount`, `fork`, ...) are fine.
- Linux only, no other platforms.

## Simplicity

- The simplest solution that works. No abstraction layers or patterns without a concrete need.
- Interfaces or virtual methods only where tests need to replace something
  (e.g. `SystemActions`, so tests do not reboot the machine).

## Naming

| Element                  | Style             | Example                     |
|--------------------------|-------------------|-----------------------------|
| Classes                  | `PascalCase`      | `MessageDispatcher`         |
| Methods, functions       | `camelCase`       | `isPlayerRunning()`         |
| Private/protected members | `snake_case_`    | `player_name_`              |
| Public members of structs | `snake_case`     | `Client::fd`                |
| Local variables, params  | `snake_case`      | `config_file`               |
| Constants                | `UPPER_CASE`      | `WATCHDOG_TIMER`            |
| Files                    | `snake_case`      | `message_dispatcher.cpp`    |

The trailing `_` of members avoids that a parameter hides a member (`-Wshadow`):

```cpp
Watchdog::Watchdog(const Configuration &configuration) : player_name_(configuration.getPlayerName()) {}
void Watchdog::setActivity(bool is_active) { is_active_ = is_active; }
```

Test fixtures in `tests/` are exempt, their members have no trailing `_`.

## Rules

- No `using namespace` in header files.
- `nullptr` instead of `NULL`.
- Pass strings as `const std::string&` when no copy is needed. Return them by value,
  that is simpler and safe (e.g. the getters of `Configuration`).
- Every class gets a short doc comment (`/** ... */`) in its header explaining what it does.
- Every source file starts with the AGPL license header.

## Warnings

The code builds **without any warning** with GCC 14 and these flags (set in `CMakeLists.txt`):
`-Wall -Wextra -Wpedantic -Wshadow -Wconversion`

- Fix a warning instead of switching it off.
- `-DWARNINGS_AS_ERRORS=ON` turns warnings into errors, e.g. in CI.

## Formatting

Formatting is defined in `.clang-format`:

- 4 spaces indentation, no tabs
- Braces on their own line (Allman style)
- `public:` / `private:` indented inside the class, members one level deeper

Format a file with `clang-format -i <file>`.

## Tests

- GoogleTest, one test file per class: `tests/test_<class_file>.cpp`.
- Run with `cmake -S . -B build && cmake --build build && ctest --test-dir build`.
