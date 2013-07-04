X52 MFD pages
=============

The Saitek SDK for Windows allows the programmer to create multiple
"pages" on the MFD display which can be toggled using the PgUp and PgDn
controls below the MFD. In addition to the pages, the software also
allows the programmer to write more than 16 characters to each line and
will automatically scroll those lines.

# Features

* Create multiple pages, each page containing up to 3 lines of text and
each line containing upto 256 characters.
* Automatically shift the text by 1 character every 250 ms, iff the text
exceeds 16 characters.
* Support callbacks on a per-page basis for the Up, Down and Select
buttons, to allow the programmer to create custom handlers.
* Support a summary page listing all the active pages.

# API

```c
x52_mfd_create_page(uint8_t *page_id,
                    char *description,
                    x52_page_callback *handler)

x52_mfd_write_line(uint8_t page_id,
                   uint8_t line,
                   char *text,
                   uint8_t length)

x52_mfd_update_page(uint8_t page_id,
                    uint8_t activate)

x52_mfd_delete_page(uint8_t page_id)

typedef void (*x52_page_callback)(uint8_t page_id,
                                  uint8_t event,
                                  uint8_t state)
```

## Steps

1. Create a page
2. Write the individual lines of text to the page
3. Update the page with activate parameter set to 1
4. (Optional) Deactivate the page with activate parameter set to 0
5. Delete the page

## Callback

The callback API is a function that takes three arguments as follows:

1. A page ID (8 bits) which identifies the current page. This allows
   a single callback handler to handle multiple pages.
2. An 8-bit event identifier. Currently, this is defined to be one of:
   * `X52_MFD_EVENT_UP`
   * `X52_MFD_EVENT_DN`
   * `X52_MFD_EVENT_SEL`
3. An 8-bit state identifier. Currently, this is defined to be one of:
   * `X52_MFD_BUTTON_STATE_UP`
   * `X52_MFD_BUTTON_STATE_DN`


