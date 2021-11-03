// fastbuffers.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>
#include "../../include/fastbuffers.hpp"

using namespace fastbuffers;

struct Person
{
	REFLECT_DEFINE(
		int age;
		float sex;
		double nice;
	);
};


int main()
{
	stream ss;
	ss << 1 << 2 << 3 << 4 << 5;

	constexpr Person ps{ 1,1.0f,1.2 };

	constexpr auto name = fastbuffers::reflect::rf_elem_name<Person, 0>();

	static_assert(name == "age", "name error");

	constexpr auto name1 = fastbuffers::rf_name<Person>();

	constexpr auto value1 = fastbuffers::rf_element<1>(ps);

	//static_assert(rf_name<Person>() == "Person", "error");

	//static_assert(rf_element<0>(ps) == 1, "re_element error");

	static_assert(std::is_same_v<rf_elemet_t<Person, 1>, float>,"elemnt type error");

	std::cout << "Hello World!\n";
	std::cin.get();
}

// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
