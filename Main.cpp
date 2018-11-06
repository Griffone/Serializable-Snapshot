#include "Snapshot.h"

#include <sstream>
#include <iostream>

int main() {

	Snapshot snapshot = {};
	Snapshot b = {};
	b.put("nested", "object");

	snapshot.put("integer", 42);
	snapshot.put("float", 6.9f);
	snapshot.put("string", "bytes");
	snapshot.put("object", b);

	std::cout << "Before serialization:" << std::endl;
	std::cout << snapshot << std::endl;

	std::stringstream testStream;
	testStream << snapshot;

	// Just to make sure
	snapshot = Snapshot();
	std::cout << "After clearing:" << std::endl;
	std::cout << snapshot << std::endl;

	testStream >> snapshot;

	std::cout << "After deserialization:" << std::endl;
	std::cout << snapshot << std::endl;


	std::cout << "Try to make one yourself:" << std::endl;
	std::cin >> snapshot;

	std::cout << "User object:" << std::endl;
	std::cout << snapshot << std::endl;

	int i;
	std::cin >> i;


	return 0;
}