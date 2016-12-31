# Zero-EPWING #

Zero-EPWING is a tool built to export easy to process JSON formatted UTF-8 data from dictionaries in
[EPWING](https://ja.wikipedia.org/wiki/EPWING) format. This is a terrible format because:

*   It is based on a closed and undocumented standard.
*   Not well supported as it isn't used anywhere else in the world.
*   The only library for parsing this format, `libeb`, is abandoned.
*   Data is stored in an inconsistent manner, with lots of duplication.
*   Text data is stored internally in the annoying EUC-JP encoding.
*   Characters which cannot be encoded are represented by image files.

Applications that parse EPWING formatted data traditionally use `libeb` to perform dictionary searches in place; dealing
with quirks in the format and `libeb` in the runtime. Zero-EPWING takes a different approach -- extract all the data and
output it an sane intermediate format, like JSON. As everyone knows how to parse JSON, it is trivial to take this
intermediate data and store it in a reasonable, industry standard representation.

![](https://foosoft.net/projects/zero-epwing/img/zero-wing.png)

## Requirements ##

*   [Autotools](https://www.gnu.org/software/automake/manual/html_node/Autotools-Introduction.html)
*   [CMake](https://cmake.org/)
*   [GCC](https://gcc.gnu.org/)
*   [Make](https://www.gnu.org/software/make/)
*   [MinGW](http://www.mingw.org/) (for Windows)

## Building ##

The instructions below are for building on Linux; creating a Windows executable is similar with the exception that
network support must be disabled on the modified `libeb` during configuration in order for it to compile and link.
Although I have not yet made MacOS X builds, it should also, in principle, work.

1.  Clone the `https://github.com/FooSoft/zero-epwing` repository.
2.  Initialize (`git submodule init`) and update (`git submodule update`) the project submodules.
3.  Switch to the `eb` directory then execute `./configure` and `make` to build the modified `libeb`.
4.  Switch to the `jansson` directory then execute `cmake .` and `make`.
5.  From the project root then execute `cmake .` and `make`.

## Usage ##

Zero-EPWING takes a single parameter, the directory of the EPWING dictionary to dump. It also supports the following
optional flags:

*   `--pretty-print` (`-p`): output pretty-printed JSON (useful for debugging).
*   `--markup` (`-m`): markup the output with as much metadata as possible.
*   `--positions` (`-s`): output *page* and *offset* data for each entry.

Upon loading and processing the provided EPWING data, Zero-EPWING will output a UTF-8 encoded JSON file to stdout.
Information about errors will be printed to stderr; serious errors will result in this application returning a non-zero
exit code. A sample of the JSON data output is pretty-printed below for reference:

```
{
    "charCode": "jisx0208",
    "discCode": "epwing",
    "subbooks": [
        {
            "title": "大辞泉",
            "copyright": "CD-ROM版大辞泉　1997年4月10日 第1版発行\n\n監　修　松村　明\n発行者　鈴木俊彦\n発行所...
            "entries": [
                {
                    "heading": "亜",
                    "text": "亜\n［音］ア\n［訓］つ‐ぐ\n［部首］二\n［総画数］７\n［コード］区点...
                },
                {
                    "heading": "あ",
                    "text": "あ\n{{w_50275}}\n{{w_50035}}五十音図ア行の第一音。五母音の一。後舌の開母音...
                },
                ...
```

You may have noticed the unusual-looking double curly bracket markers (such as `{{w_50035}}`). Remember what I mentioned
about certain characters being represented by image files? There are two graphical fonts in each dictionary, narrow and
wide. Whenever a character cannot be encoded as text, a glyph is used in its place. These font indices cannot be
converted directly to characters, differ from one dictionary to another, and short of performing OCR, you must build
tables to map these font indices to Unicode characters yourself. Zero-EWPING has no means to map these font glyphs to
Unicode by itself, and instead places inline markers in the form of `{{w_xxxx}}` and `{{n_xxxx}}` in the output,
specifying the referenced indices of the wide or narrow fonts respectively. The font glyphs can be dumped with the
`ebfont` sample application distributed with `libeb`, making the creation of these mappings tedious but possible.

## License ##

MIT
