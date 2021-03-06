/*
 * this file demonstrates integrating ggformat with a string class
 *
 * compile me with "cl.exe string_examples.cpp ggformat.cpp"
 * or "g++ -std=c++11 string_examples.cpp ggformat.cpp"
 */

#include "ggformat.h"

template< typename T >
T min( T a, T b ) {
	return a < b ? a : b;
}

template< size_t N >
class str {
public:
	str() {
		clear();
	}

	template< typename... Rest >
		str( const char * fmt, const Rest & ... rest ) {
			sprintf( fmt, rest... );
		}

	void clear() {
		buf[ 0 ] = '\0';
		length = 0;
	}

	template< typename T >
	void operator+=( const T & x ) {
		appendf( "{}", x );
	}

	template< typename... Rest >
	void sprintf( const char * fmt, const Rest & ... rest ) {
		size_t copied = ggformat( buf, N, fmt, rest... );
		length = min( copied, N - 1 );
	}

	template< typename... Rest >
	void appendf( const char * fmt, const Rest & ... rest ) {
		size_t copied = ggformat( buf + length, N - length, fmt, rest... );
		length += min( copied, N - length - 1 );
	}

	const char * c_str() const {
		return buf;
	}

private:
	char buf[ N ];
	size_t length;
};

template< size_t N >
void format( FormatBuffer * fb, const str< N > & buf, const FormatOpts & opts ) {
	format( fb, buf.c_str(), opts );
}

int main() {
	str< 256 > a( "hello {-10}:", "world" );
	a += " ";
	a += 1;
	a += " ";
	a += 1.2345;
	a += " ";
	a += false;
	a.appendf( ". {} w{}rld", "goodbye", 0 );

	ggprint( "{}\n", a );
	return 0;
}
