#include "Snapshot.h"

#include <sstream>
#include <iostream>

int main() {

	Snapshot snapshot = {};

	snapshot.put("test", 12);
	snapshot.put("answer", 42);

	snapshot.put("name", "test snapshot");

	auto item = snapshot.get("test");

	std::cout << "Test should be 12, it is: " << item.value.integer << std::endl;

	std::cout << "Before serialization: " << std::endl;
	std::cout << snapshot << std::endl;

	std::stringstream testStream;
	testStream << snapshot;

	// Just to make sure
	snapshot = Snapshot();
	std::cout << "Make sure the snapshot is empty:" << std::endl;
	std::cout << snapshot << std::endl;

	testStream >> snapshot;

	std::cout << "After deserialization: " << std::endl;
	std::cout << snapshot << std::endl;

	std::cin >> item.value.integer;


	return 0;
}