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
- System calls without a C++ equivalent (`mq_open`, `reboot`, `mount`, `fork`, ...) are fine.

## Simplicity

- The simplest solution that works. No abstraction layers or patterns without a concrete need.
- Interfaces or virtual methods only where tests need to replace something
  (e.g. `SystemActions`, so tests do not reboot the machine).

## Naming

| Element                  | Style             | Example                     |
|--------------------------|-------------------|-----------------------------|
| Classes                  | `PascalCase`      | `MessageDispatcher`         |
| Methods, functions       | `camelCase`       | `isPlayerRunning()`         |
| Member variables         | `snake_case`      | `player_name`               |
| Local variables, params  | `snake_case`      | `config_file`               |
| Constants                | `UPPER_CASE`      | `WATCHDOG_TIMER`            |
| Files                    | `snake_case`      | `message_dispatcher.cpp`    |

If a parameter has the same name as a member, assign with `this->`.

## Rules

- No `using namespace` in header files.
- `nullptr` instead of `NULL`.
- Pass and return strings as `const std::string&` when no copy is needed.
- Every class gets a short doc comment (`/** ... */`) in its header explaining what it does.
- Every source file starts with the AGPL license header.

## Formatting

Formatting is defined in `.clang-format`:

- 4 spaces indentation, no tabs
- Braces on their own line (Allman style)
- `public:` / `private:` indented inside the class, members one level deeper

Format a file with `clang-format -i <file>`.

## Tests

- GoogleTest, one test file per class: `tests/test_<class_file>.cpp`.
- Run with `cmake -S . -B build && cmake --build build && ctest --test-dir build`.
