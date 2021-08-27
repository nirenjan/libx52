# inih (INI Not Invented Here)

**inih (INI Not Invented Here)** is a simple [.INI
file](http://en.wikipedia.org/wiki/INI_file) parser written in C. It's only a
couple of pages of code, and it was designed to be _small and simple_, so it's
good for embedded systems. It's also more or less compatible with Python's
[ConfigParser](http://docs.python.org/library/configparser.html) style of .INI
files, including RFC 822-style multi-line syntax and `name: value` entries.

The original project can be found [here](https://github.com/benhoyt/inih). I
have included the sources in this repository because it is not packaged for all
platforms (notably macOS), and the version here supercedes the version that is
distributed in most "stable" Linux distributions such as Debian.
