Pinelog - a lightweight logging API
===================================

Pinelog is a lightweight logging API for C programs that's designed to be
included in your program source code. Parameters for Pinelog are configured at
build time by means of preprocessor flags.

# Usage
## Logging macros

Pinelog uses `printf` style formatting, using the following list of macros. The
macro indicates the level at which the message is logged.

* `PINELOG_FATAL`
* `PINELOG_ERROR`
* `PINELOG_WARN`
* `PINELOG_INFO`
* `PINELOG_DEBUG`
* `PINELOG_TRACE`

**Note:** `PINELOG_FATAL` is used when the program encounters a fatal condition
and needs to abort. This will log the fatal message and terminate the program
with an exit code of 1.

### Example

```C
PINELOG_INFO("configuration file %s not found, using defaults", config_file);
```

## Logging levels

The default logging level is `ERROR`, and this can be controlled by the
preprocessor flag `PINELOG_DEFAULT_LEVEL`.

The program can control the level at which messages can be logged at runtime,
by using the `pinelog_set_level` function. This function takes in the level
definition, which is one of the following, in increasing order of priority.

* `PINELOG_LVL_TRACE`
* `PINELOG_LVL_DEBUG`
* `PINELOG_LVL_INFO`
* `PINELOG_LVL_WARNING`
* `PINELOG_LVL_ERROR`
* `PINELOG_LVL_FATAL`
* `PINELOG_LVL_NONE`

Setting the level to a given priority suppresses all log messages of lower
priority, i.e., if the level is set to `PINELOG_LVL_ERROR`, messages at
`WARNING` level and below will be suppressed, but `ERROR` and `FATAL` messages
will be logged.

**Note:** `PINELOG_LVL_NONE` suppresses all log messages, but `PINELOG_FATAL`
will still terminate the program, even though nothing is logged.

### Example

```C
pinelog_set_level(PINELOG_LVL_WARNING);
```

```
-DPINELOG_DEFAULT_LEVEL=PINELOG_LVL_WARNING
```

## Output redirection

Pinelog defaults to writing the log messages to standard output, and this can
be controlled by the preprocessor flag `PINELOG_DEFAULT_STREAM`.

However, the application can redirect log messages at runtime to a different
`FILE *` stream, or to a file by using one of the following two methods:

```C
FILE *out = fopen("/run/app.fifo", "w");
pinelog_set_output_stream(out);
pinelog_set_output_file("/var/log/app.log");
```

```
-DPINELOG_DEFAULT_STREAM=stderr
```

## Logging format

Pinelog uses an opinionated logging format that is fixed as follows. Fields
within `[]` are optional and controlled by build time flags.

    [2021-07-14 11:08:04 ][ERROR: ][./test_pinelog.c:108 ]formatted message.

The program can be controlled by the following preprocessor flags, all of which
default to `0` (disabled). Set the flag to `1` to enable it.

* `PINELOG_SHOW_DATE` - Display the ISO 8601 date and time when the message is
  logged.
* `PINELOG_SHOW_LEVEL` - Display the level at which the message is logged.
* `PINELOG_SHOW_BACKTRACE` - Display the file and line where the message is
  logged.

Set these flags by using the `-D` compiler argument, .e.g.
`-DPINELOG_SHOW_LEVEL=1 -DPINELOG_SHOW_DATE=1`

### Level strings

The application can control the level strings displayed by means of preprocessor
flags, if the application wishes to display the log messages in a language other
than English. This can be achieved by means of the following preprocessor
definitions.

* `PINELOG_FATAL_STR`
* `PINELOG_ERROR_STR`
* `PINELOG_WARNING_STR`
* `PINELOG_INFO_STR`
* `PINELOG_DEBUG_STR`
* `PINELOG_TRACE_STR`

### Example

```
-DPINELOG_ERROR_STR=\"E\" -DPINELOG_FATAL_STR=\"F\"
```

# Integrating Pinelog

Pinelog is intended to be integrated into your application source tree, either
by means of including the sources directly, or by including the repository as
a Git submodule or subtree.

The default build of Pinelog uses an autotools generated `config.h` file, which
includes checks for the following GCC attributes. If you don't care about these,
then either create a dummy config.h which includes the macros
`HAVE_FUNC_ATTRIBUTE_CONSTRUCTOR` and `HAVE_FUNC_ATTRIBUTE_FORMAT`, or use the
`AX_GCC_FUNC_ATTRIBUTE` macro to check for the `constructor` and `format`
attributes in your application's `configure.ac` file.
