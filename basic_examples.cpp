/*
 * this file demonstrates basic ggformat usage
 *
 * compile me with "cl.exe basic_examples.cpp ggformat.cpp"
 * or "g++ -std=c++11 basic_examples.cpp ggformat.cpp"
 */

#include <stdint.h>
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

struct Thing {
	// pretend this is more complicated
	int a;
	float b;
};

void format( FormatBuffer * fb, const Thing & thing, const FormatOpts & opts ) {
	// this is a bit of a hack but is occasionally useful
	// note that opts are ignored, rather than forwarded to a and b
	ggformat_impl( fb, "a = {}. b = {}", thing.a, thing.b );
}

int main() {
	// basic types
	ggprint( "ints: {-5} {04} {+} {}\n", 1, 1, 1, 1 );
	ggprint( "hex: 0x{04x}\n", 123 );
	ggprint( "bin: 0b{b} 0b{b} 0b{b} 0b{b}\n", uint64_t( 123 ), int32_t( -123 ), uint16_t( 123 ), uint8_t( 123 ) );
	ggprint( "floats: {-10} {4.2} {+} {}\n", 1.23, 1.23, 1.23, 1.23 );
	ggprint( "bools: {} {}\n", true, false );
	ggprint( "strings: {-10} {} {{ }}\n", "hello", "world" );

	ggprint( "mins : {} {} {} {}\n", int64_t( INT64_MIN ), int32_t( INT32_MIN ), int16_t( INT16_MIN ), int8_t( INT8_MIN ) );
	ggprint( "maxs : {} {} {} {}\n", int64_t( INT64_MAX ), int32_t( INT32_MAX ), int16_t( INT16_MAX ), int8_t( INT8_MAX ) );
	ggprint( "umaxs: {} {} {} {}\n", uint64_t( UINT64_MAX ), uint32_t( UINT32_MAX ), uint16_t( UINT16_MAX ), uint8_t( UINT8_MAX ) );

	ggprint( "mins : {x} {x} {x} {x}\n", int64_t( INT64_MIN ), int32_t( INT32_MIN ), int16_t( INT16_MIN ), int8_t( INT8_MIN ) );
	ggprint( "maxs : {x} {x} {x} {x}\n", int64_t( INT64_MAX ), int32_t( INT32_MAX ), int16_t( INT16_MAX ), int8_t( INT8_MAX ) );
	ggprint( "umaxs: {x} {x} {x} {x}\n", uint64_t( UINT64_MAX ), uint32_t( UINT32_MAX ), uint16_t( UINT16_MAX ), uint8_t( UINT8_MAX ) );

	ggprint( "mins : {b} {b} {b} {b}\n", int64_t( INT64_MIN ), int32_t( INT32_MIN ), int16_t( INT16_MIN ), int8_t( INT8_MIN ) );
	ggprint( "maxs : {b} {b} {b} {b}\n", int64_t( INT64_MAX ), int32_t( INT32_MAX ), int16_t( INT16_MAX ), int8_t( INT8_MAX ) );
	ggprint( "umaxs: {b} {b} {b} {b}\n", uint64_t( UINT64_MAX ), uint32_t( UINT32_MAX ), uint16_t( UINT16_MAX ), uint8_t( UINT8_MAX ) );

	// user defined type
	v3 a = v3( 1, 2, 3 );
	v3 b = v3( 4, 5, 6 );
	ggprint( "a = {}. b = {02.2}.\na + b = {+}\n", a, b, a + b );

	// more complicated user defined type
	Thing thing;
	thing.a = 12345;
	thing.b = 67890;
	ggprint( "{}\n", thing );

	return 0;
}

