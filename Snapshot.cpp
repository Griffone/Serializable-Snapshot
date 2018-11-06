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

Snapshot::Item Snapshot::get(snapshot_key_t const &key) {
	auto keyval = header.find(key);

	if (keyval == header.end())
		throw KeyNotFountException(key);

	Item item = {
		keyval->second.first,
		0
	};

	switch (keyval->second.first) {
	case SVT_INTEGER:
		item.value.integer = integers[keyval->second.second];
		break;

	case SVT_FLOAT:
		item.value.real = floats[keyval->second.second];
		break;

	case SVT_STRING:
		item.value.string = &strings[keyval->second.second];
		break;

	case SVT_OBJECT:
		item.value.object = &objects[keyval->second.second];
		break;
	}

	return item;
}

std::ostream & operator<<(std::ostream & os, Snapshot const & snapshot) {
	os << '{';
	bool first = true;
	for (auto item : snapshot.header) {
		if (first)
			first = false;
		else
			os << ", ";

		os << "(\"" << item.first << "\"," << item.second.first << "):";

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
