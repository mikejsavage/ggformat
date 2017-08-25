# ggformat

ggformat is a liberally licensed string formatting library for C++ that
supports user defined types without blowing up your compile times. It is
meant to be used as a replacement for printf and friends.

ggformat requires C++11 (variadic templates), and supports VS2015, GCC
and clang out of the box. It should also work with VS2013 and VS2017 but
I don't test against them.

[tinyformat]: https://github.com/c42f/tinyformat

I wrote ggformat because the existing string formatting options for C++
either do not support user defined types or bloat compile times too
much. printf doesn't support user defined types. Streams bloat compile
times and IO manipulators are unreadable. [tinyformat](tinyformat) uses
streams under the hood and also bloats compile times.


## Usage

```cpp
size_t ggformat( char * buf, size_t len, const char * fmt, ... );
bool ggprint_to_file( FILE * file, const char * fmt, ... );
bool ggprint( const char * fmt, ... );
```

In short, `ggformat` replaces s(n)printf, `ggprint_to_file` replaces
fprintf, and `ggprint` replaces printf.

`ggformat` writes at most `len` bytes to `buf`, and that always includes
a null terminator. It returns the number of bytes that would have been
written if `buf` were large enough, _not including the null terminator_,
and can be larger than `len` (just like sprintf).

`ggprint_to_file` does what you would expect, and `ggprint` writes to
standard output. Both return `true` on success, or `false` if the write
fails.

`ggformat` does not allocate memory, the other functions allocate if you
print lots of data. All functions abort if you mess up the formatting
string or don't pass the right number of arguments. You should not pass
user defined strings as format strings, and I believe it's more helpful
to fail hard on programmer typos.
	
Basic usage looks like this:

```cpp
#include "ggformat.h"

int main() {
	ggprint( "hello {}\n", 1.23 ); // hello 1.23000
	return 0;
}
```

and you can see more examples in basic_examples.cc and
string_examples.cc.


## Format options

You can add format specifiers between the braces to change how things
are printed. The following options are supported:

- Plus sign (`{+}`): Prints a leading + for positive numeric types.
- Width (`{x}`): left pads the output with spaces to be `x` characters
  wide. When used on floats, it left pads the output so the _left side
  of the decimal point_ is `x` characters wide (I chose this because I
  think it makes `{x.y}` more intuitive). If the output is already wider
  than `x` characters, it doesn't do anything.
- Width with zero padding (`{0x}`): as above, but pads with zeroes
  instead of spaces.
- Width with left alignment (`{-x}` or `{-0x}`): same again but puts the
  spaces/zeroes on the right.
- Precision (`{.x}`): specifies the number of digits that appear after
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
see `Thing` in basic_examples.cc.


## Dynamic allocation (std::string, asprintf, etc)

`ggformat( NULL, 0, ... );` returns the number of bytes required to hold
the formatted string. With that it's easy to integrate ggformat with
your favourite dynamic string solution. For example, ggformat with
std::string:

```cpp
template< typename... Rest >
std::string ggformat_to_string( const char * fmt, Rest... rest ) {
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
string_examples.cc.

ggformat uses sprintf under the hood. It compiles slightly slower than
sprintf and quite a bit faster than tinyformat. Runtime performance is
not important, but ggformat shouldn't be much slower than sprintf.

In general ggformat is short enough that you can easily modify it to fit
your needs, and will be updated infrequently enough that doing so isn't
a huge pain. For example, it's very easy to replace malloc/free with
your own allocators, and if you don't like aborting on errors it's
pretty easy to change that too.
