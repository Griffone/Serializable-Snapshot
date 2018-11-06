#include "Snapshot.h"

Snapshot::Snapshot(size_t reservedIntegerCount, size_t reservedFloatCount, size_t reservedStringCount, size_t reservedObjectCount) {
	integers.reserve(reservedIntegerCount);
	floats.reserve(reservedFloatCount);
	strings.reserve(reservedStringCount);
	objects.reserve(reservedObjectCount);

	header.reserve(reservedIntegerCount + reservedFloatCount + reservedStringCount + reservedObjectCount);
}

Snapshot::~Snapshot() {
	// use default destructors of collections
}

Snapshot::KeyNotFountException::KeyNotFountException(snapshot_key_t const &key) : key(key) {}

char const * Snapshot::KeyNotFountException::what() const throw() {
	return (key + " was not found in snapshot").c_str();
}

void Snapshot::put(snapshot_key_t const &key, snapshot_int_t const &value) {
	auto keyval = header.find(key);
	if (keyval == header.end()) {
		header[key] = std::pair<ValueType, size_t>(SVT_INTEGER, integers.size());
		integers.push_back(value);
	} else {
		integers[keyval->second.second] = value;
	}
}

void Snapshot::put(snapshot_key_t const &key, snapshot_float_t const &value) {
	auto keyval = header.find(key);
	if (keyval == header.end()) {
		header[key] = std::pair<ValueType, size_t>(SVT_FLOAT, floats.size());
		floats.push_back(value);
	} else {
		floats[keyval->second.second] = value;
	}
}

void Snapshot::put(snapshot_key_t const &key, snapshot_string_t const &value) {
	auto keyval = header.find(key);
	if (keyval == header.end()) {
		header[key] = std::pair<ValueType, size_t>(SVT_STRING, strings.size());
		strings.push_back(value);
	} else {
		strings[keyval->second.second] = value;
	}
}

void Snapshot::put(snapshot_key_t const &key, Snapshot const &value) {
	auto keyval = header.find(key);
	if (keyval == header.end()) {
		header[key] = std::pair<ValueType, size_t>(SVT_OBJECT, objects.size());
		objects.push_back(value);
	} else {
		objects[keyval->second.second] = value;
	}
}

Snapshot::Item Snapshot::get(snapshot_key_t const &key) const {
	auto keyval = header.find(key);

	if (keyval == header.end())
		throw KeyNotFountException(key);

	switch (keyval->second.first) {
	case SVT_INTEGER:
		return Item(integers[keyval->second.second]);

	case SVT_FLOAT:
		return Item(floats[keyval->second.second]);

	case SVT_STRING:
		return Item(strings[keyval->second.second]);

	case SVT_OBJECT:
		return Item(objects[keyval->second.second]);
	}
}

#define VTC_INTEGER	'I'
#define VTC_FLOAT	'F'
#define VTC_STRING	'S'
#define VTC_OBJECT	'O'

std::ostream & operator<<(std::ostream & os, Snapshot::ValueType const & type) {
	switch (type) {
	case Snapshot::SVT_INTEGER:
		os << VTC_INTEGER;
		break;

	case Snapshot::SVT_FLOAT:
		os << VTC_FLOAT;
		break;

	case Snapshot::SVT_STRING:
		os << VTC_STRING;
		break;

	case Snapshot::SVT_OBJECT:
		os << VTC_OBJECT;
		break;
	}

	return os;
}

std::istream & operator>>(std::istream & is, Snapshot::ValueType &type) {
	std::istream::sentry s(is);

	if (!s) return is;

	char c;
	if (!is.get(c)) return is;

	switch (c) {
	case VTC_INTEGER:
		type = Snapshot::SVT_INTEGER;
		break;

	case VTC_FLOAT:
		type = Snapshot::SVT_FLOAT;
		break;

	case VTC_STRING:
		type = Snapshot::SVT_STRING;
		break;

	case VTC_OBJECT:
		type = Snapshot::SVT_OBJECT;
		break;

	default:
		is.setstate(std::ios::badbit);
	}

	return is;
}

std::ostream & operator<<(std::ostream & os, Snapshot const & snapshot) {
	os << "{("
		<< snapshot.integers.size() << ','
		<< snapshot.floats.size() << ','
		<< snapshot.strings.size() << ','
		<< snapshot.objects.size() << ')';

	for (auto item : snapshot.header) {
		os << ", (\"" << item.first << "\"," << item.second.first << "):";

		switch (item.second.first) {
		case Snapshot::SVT_INTEGER:
			os << snapshot.integers[item.second.second];
			break;

		case Snapshot::SVT_FLOAT:
			os << snapshot.floats[item.second.second];
			break;

		case Snapshot::SVT_STRING:
			os << '"' << snapshot.strings[item.second.second] << '"';
			break;

		case Snapshot::SVT_OBJECT:
			os << snapshot.objects[item.second.second];
			break;
		}
	}
	os << '}';
	return os;
}

#define pullChar(stream, ch)			\
	if (is.get() != ch) {				\
		is.setstate(std::ios::badbit);	\
		return stream;					\
	}

inline std::istream& parseCounts(std::istream &is, size_t &integerCount, size_t &floatCount, size_t &stringCount, size_t &objectCount) {
	char c;
	if (is.bad()) return is;

	pullChar(is, '(');

	is >> integerCount;
	if (is.bad()) return is;
	pullChar(is, ',');

	is >> floatCount;
	if (is.bad()) return is;
	pullChar(is, ',');
	
	is >> stringCount;
	if (is.bad()) return is;
	pullChar(is, ',');
	
	is >> objectCount;
	if (is.bad()) return is;
	pullChar(is, ')');
}

inline snapshot_string_t readString(std::istream &is) {
	snapshot_string_t string;

	if (is.get() != '"') {
		is.setstate(std::ios::badbit);
		return string;
	}

	char c;
	while (is.get(c)) {
		if (c == '"')
			break;

		string.push_back(c);
	}
	return string;
}

inline snapshot_key_t getKey(std::istream &is) {
#if SNAPSHOT_KEY_IS_STRING
	return readString(is);
#else
	snapshot_key_t key;
	is >> key;
	return key;
#endif
}

std::istream & operator>>(std::istream &is, Snapshot &snapshot) {
	std::istream::sentry s(is);

	if (!s) return is;

	pullChar(is, '{');

	size_t intCount, floatCount, stringCount, objectCount;
	parseCounts(is, intCount, floatCount, stringCount, objectCount);

	if (is.bad()) return is;

	snapshot = Snapshot(intCount, floatCount, stringCount, objectCount);
	size_t totalCounts = intCount + floatCount + stringCount + objectCount;

	for (size_t i = 0; i < totalCounts && is.good(); ++i) {
		pullChar(is, ',');
		pullChar(is, ' ');

		pullChar(is, '(');
		
		std::string key = getKey(is);
		if (is.bad()) break;

		pullChar(is, ',');
		Snapshot::ValueType type;
		is >> type;

		if (is.bad()) break;
		pullChar(is, ')');
		pullChar(is, ':');

		snapshot_int_t integer;
		snapshot_float_t real;
		snapshot_string_t string;
		Snapshot object;

		switch (type) {
		case Snapshot::SVT_INTEGER:
			is >> integer;
			snapshot.put(key, integer);
			break;

		case Snapshot::SVT_FLOAT:
			is >> real;
			snapshot.put(key, real);
			break;

		case Snapshot::SVT_STRING:
			string = readString(is);
			snapshot.put(key, string);
			break;

		case Snapshot::SVT_OBJECT:
			is >> object;
			snapshot.put(key, object);
			break;
		}

		if (is.bad()) break;
	}

	pullChar(is, '}');

	return is;
}

Snapshot::Item::Item(snapshot_int_t _value) : type(SVT_INTEGER) {
	value.integer = _value;
}

Snapshot::Item::Item(snapshot_float_t _value) : type(SVT_FLOAT) {
	value.real = _value;
}

Snapshot::Item::Item(snapshot_string_t const &_value) : type(SVT_INTEGER) {
	value.string = &_value;
}

Snapshot::Item::Item(Snapshot const &_value) : type(SVT_INTEGER) {
	value.object = &_value;
}
