#include "Snapshot.h"

#include <iostream>

int main() {

	Snapshot snapshot = {};

	snapshot.put("test", 12);
	snapshot.put("answer", 42);

	snapshot.put("name", "test snapshot");

	auto item = snapshot.get("test");

	std::cout << "Test should be 12, it is: " << item.value.integer << std::endl;

	std::cout << snapshot;

	std::cin >> item.value.integer;


	return 0;
}