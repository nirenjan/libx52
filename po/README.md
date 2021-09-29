Notes for translators
=====================

libx52 is slowly being migrated to use the GNU gettext library to support
internationalization (i18n). Contributions are welcome to both update the
existing code to use `gettext`, and to add new translations.

# xx_PL

xx_PL is a translation of English strings into [Pig
Latin](https://en.wikipedia.org/wiki/Pig_Latin) that is used in lieu of any real
translations. This file is used to test that the translation functionality is
working as expected.

# Adding new code to i18n (maintainers)

Most code should be using the `_("...")` format to refer to a translatable
string. Some strings can be left untranslated, but these are mostly just
whitespace, such as line breaks.

Once you add new strings to be translated, update `po/POTFILES.in` to include
any new files that have to be translated. The file path is relative to the root
of the project.

The next step is to rebuild the translation template `libx52.pot`. To do
this, run `make -C po update-po` from your build directory (where you ran
`configure`). This also updates any translation files (with `.po` extension) to
include the new source strings.

# Adding new languages (translators)

Run `msginit -l <language-code> libx52.pot` to generate a new `.po` file
for that language. Language code is a 2 letter
[ISO-639-1](https://en.wikipedia.org/wiki/List_of_ISO_639-1_codes) code with an
optional [ISO-3166-1](https://en.wikipedia.org/wiki/ISO_3166-1_alpha-2) region
suffix. Edit this file to add your translation, and add the new language to
`po/LINGUAS`.

`po/LINGUAS` must be in alphabetical order.

# Testing the i18n functionality

TODO

