#include <iostream>

struct partial_t {
	alignas(std::hardware_destructive_interference_size) // = 40
		unsigned value;
};

struct scalability_result {
	unsigned result; // ��� ������ ���� ����� ��� ������������ ���������� �������� �������
	double t, s, e; // ��������� � �������������
};