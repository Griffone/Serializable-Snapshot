#pragma once

#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

// If you use extractable types just set this to false end define snapshot_key_t to an extractable type and you're good
#define SNAPSHOT_KEY_IS_STRING	true
typedef std::string snapshot_key_t;

typedef int			snapshot_int_t;
typedef float		snapshot_float_t;
typedef std::string snapshot_string_t;

/*
A serializable object
Should hold no external references, ie be completely independently serializable.
To indicate that I call it a snapshot

Planned features:
* cpp streams serialization and deserialization
* save to and load from files

Possible features:
* support for arbitrary arrays of ValueTypes
*/
class Snapshot {
public:
	// Valute type of a stored item
	enum ValueType {
		SVT_INTEGER	= 0x01,	// a 32-bit integer number
		SVT_FLOAT	= 0x02,	// a 32-bit floating point number
		SVT_STRING	= 0x11,	// an snapshotString_t type string
		SVT_OBJECT	= 0x12	// another snapshot
	};
	// A returned item object
	struct Item {
		ValueType type;
		union Value {
			snapshot_int_t			integer;
			snapshot_float_t		real;
			snapshot_string_t const	*string;	// pointer to make sure it packs into small number of bits
			Snapshot const			*object;	// pointer to make sure it packs into 64-bits
		} value;

		Item(snapshot_int_t);
		Item(snapshot_float_t);
		Item(snapshot_string_t const &);
		Item(Snapshot const &);

		friend std::ostream& operator<<(std::ostream &os, ValueType const &type);
		friend std::istream& operator>>(std::istream &is, ValueType &type);
	};
	struct KeyNotFountException : public std::exception {
		KeyNotFountException(snapshot_key_t const &);

		char const * what() const throw();

		snapshot_key_t key;
	};

	// To avoid reallocation of the underlying memory, please consider providing counts for each of the neede object
	Snapshot(size_t reservedIntegerCount = 0, size_t reservedFloatCount = 0, size_t reservedStringCount = 0, size_t reservedObjectCount = 0);
	~Snapshot();

	// Please avoid having '"' in the key!, it will not serialize correctly!
	void put(snapshot_key_t const &key, snapshot_int_t const & value);
	void put(snapshot_key_t const &key, snapshot_float_t const & value);
	void put(snapshot_key_t const &key, snapshot_string_t const &value);
	void put(snapshot_key_t const &key, Snapshot const &value);

	/*
	Attempts to get the item associated with a given key.
	Throws KeyNotFountException if there is no association with a given key.
	*/
	Item get(snapshot_key_t const &key) const;

	friend std::ostream& operator<<(std::ostream &os, Snapshot const &snapshot);
	friend std::istream& operator>>(std::istream &is, Snapshot &snapshot);

private:
	std::unordered_map<snapshot_key_t, std::pair<ValueType, size_t>> header;

	std::vector<snapshot_int_t>		integers;
	std::vector<snapshot_float_t>	floats;
	std::vector<snapshot_string_t>	strings;
	std::vector<Snapshot>			objects;
};