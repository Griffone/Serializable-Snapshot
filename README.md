# C++ serializable snapshot object

A tiny exercise to essentially build a serializable hash map to multiple types of objects.
The key is to reduce unnecessary memory footprint of union structs to make it managable.
The serializable_type_t can be changed to suit your needs, for example the Key can be an enum, if all your snapshots have limited number of known fields.

## Features:
[x] Json-typed map (excluding arrays)
[x] stream serialization
[ ] stream deserialization
[ ] saving to JSON file
[ ] loading from JSON file

## Features that can potentially be implemented in the future:
[ ] Array type support
[ ] Correct cross-platform serialization-deserialization
[ ] Difference calculation
[ ] Delta compression 