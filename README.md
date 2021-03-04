# ggformat v1.1

ggformat is a liberally licensed string formatting library for C++ that
supports user defined types and sub-second compile times. It's meant to
be used as a replacement for printf and friends.

ggformat saves you time by reducing the amount of tedious boilerplate
code you have to write, without paying it back at compile time. ggformat
has a trivial API and is easy to incrementally add to an existing
codebase. ggformat integrates nicely with custom strings and allocators,
making it appropriate for use in long-lived applications and games where
memory fragmentation is a concern.

ggformat supports VS2015 onwards, GCC and clang out of the box. It
should also work with VS2013 but consider it unsupported.


## Version history

As of March 2021, ggformat has been used in shipped code for three and a
half years with only a single minor bugfix. I consider it complete.

- __v1.1 3rd Mar 2021__: fix undefined behaviour in binary printer, and
  some potential compilation issues.
- __v1.0 29th Oct 2017__: variadic arguments are now passed by const
  reference. You can now use ggformat with types that have deleted copy
  constructors/assignment operators.


## Usage

```cpp
size_t ggformat( char * buf, size_t len, const char * fmt, ... );
bool ggprint_to_file( FILE * file, const char * fmt, ... );
bool ggprint( const char * fmt, ... );
```

In short, `ggformat` replaces `snprintf`, `ggprint_to_file` replaces
`fprintf`, and `ggprint` replaces `printf`.

`ggformat` writes at most `len` bytes to `buf`, and includes a null
terminator unless `len` is zero. It returns the number of bytes that
would have been written if `buf` were arbitrarily large, _not including
the null terminator_. You may use the return value to allocate a new
buffer large enough to hold the full string, see the dynamic allocation
section below for an example.

`ggprint_to_file` does what you would expect, and `ggprint` writes to
standard output. Both return `true` on success, or `false` if the write
fails. Both use `fwrite` internally and set `errno` accordingly.

`ggformat` does not allocate memory. `ggprint_to_file` and `ggprint`
allocate if you print lots of data. All functions abort if you mess up
the formatting string or don't pass the right number of arguments. You
should not pass user defined strings as format strings, and I believe
it's more helpful to fail hard on programmer typos.

Basic usage looks like this:

```cpp
#include "ggformat.h"

int main() {
	ggprint( "hello {}\n", 1.23 ); // hello 1.23000
	return 0;
}
```

and you can see more examples in basic_examples.cpp and
string_examples.cpp.


## Format options

You can add format specifiers between the braces to change how things
are printed. The following options are supported:

- Plus sign (`{+}`): Prints a leading + for positive numeric types.
- Width (`{#}`): left pads the output with spaces to be `#` characters
  wide. When used on floats, it left pads the output so the _left side
  of the decimal point_ is `x` characters wide. Note that this behaviour
  doesn't match printf, but it does make format strings like `{#.#}`
  behave like you would expect.. If the output is already wider than `#`
  characters, it doesn't do anything.
- Width with zero padding (`{0#}`): as above, but pads with zeroes
  instead of spaces.
- Width with left alignment (`{-#}` or `{-0#}`): same again but puts the
  spaces/zeroes on the right.
- Precision (`{.#}`): specifies the number of digits that appear after
  the decimal point when printing floats.
- Number format (`{x}` or `{b}`): prints integers in hexadecimal/binary.

These can all be combined, but should be kept in the order they were
just listed in.

If you want to print a literal { or }, use {{ and }}.


## User defined types

If you want to print your own types with ggformat, you need to define
`void format( FormatBuffer * fb, T x, const FormatOpts & opts );`.
`FormatBuffer` is a wrapper around a `char *` and length and its exact
definition is not important. `FormatOpts` holds parsed format options
and is defined as:

```cpp
struct FormatOpts {
        enum NumberFormat { DECIMAL, HEX, BINARY };

        int width = -1;
        int precision = -1;
        bool plus_sign = false;
        bool left_align = false;
        bool zero_pad = false;
        NumberFormat number_format = DECIMAL;
};
```

`format` implementations are typically quite simple:

```cpp
#include "ggformat.h"

struct v3 {
	explicit v3( float x_, float y_, float z_ ) { x = x_; y = y_; z = z_; }
	float x, y, z;
};

v3 operator+( const v3 & lhs, const v3 & rhs ) {
	return v3( lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z );
}

void format( FormatBuffer * fb, const v3 & v, const FormatOpts & opts ) {
	format( fb, "v3(" );
	format( fb, v.x, opts );
	format( fb, ", " );
	format( fb, v.y, opts );
	format( fb, ", " );
	format( fb, v.z, opts );
	format( fb, ")" );
}

int main() {
        v3 a = v3( 1, 2, 3 );
        v3 b = v3( 4, 5, 6 );
	// a = v3(1.00000, 2.00000, 3.00000). b = v3( 4.00,  5.00,  6.00).
	// a + b = v3(+5.00000, +7.00000, +9.00000)
        ggprint( "a = {}. b = {2.2}.\na + b = {+}\n", a, b, a + b );

        return 0;
}
```

If you have a huge type and don't feel like writing a wall of `format`,
see `Thing` in basic_examples.cpp.


## Dynamic allocation (std::string, asprintf, etc)

`ggformat` returns the number of bytes needed to store the formatted
string, which you can then use to allocate a buffer large enough to hold
the formatted string. With that it's easy to integrate ggformat with
your favourite dynamic string solution. For example, ggformat with
std::string:

```cpp
template< typename... Rest >
std::string ggformat_to_string( const char * fmt, const Rest & ... rest ) {
	size_t space_required = ggformat( nullptr, 0, fmt, rest... );

	if( space_required + 1 < space_required )
		throw std::overflow_error( "formatted string is too long" );

	std::string result;
	result.resize( space_required + 1 ); // + 1 so there's space for the null terminator...
	ggformat( &result[ 0 ], space_required + 1, fmt, rest... );
	result.resize( space_required ); // ...and then trim it off

	return result;
}
```


## Other stuff

Since this is C++ you can and should wrap `ggformat` in a string class
to make it more convenient to use. You can see an example in
string_examples.cpp.

ggformat uses sprintf under the hood. sprintf can be pretty slow at
runtime, but compiles quickly.

ggformat has a small codebase and will probably never receive another
update. If you don't like some of my decisions you can easily create a
local fork.
