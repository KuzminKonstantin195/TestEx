#include <iostream>
#include <string>
#include <ctime>
#include <iomanip>
#include <fstream>
#include <vector>
//#include <Windows.h>
//#include <clocale>

#include "nlohmann/json.hpp"



using namespace std;
using namespace nlohmann;


struct PersonalData {
	string name = "null";
	string surname = "nullSur";
	int num = 0;
	tm startTime;
	tm finishTime;
	//int startOverPoint = 0;
	//int finishOverPoint = 0;
	int result_time = 0;
	//int resultOverPoint = 0;
};

void merge(vector<PersonalData>& vec, int start, int end, int mid);

void sort(vector<PersonalData>& vec, int start, int end) {

	if (start < end) {
		if (end - start == 1) { // значит в векторе два элемента
			if (vec[start].result_time > vec[end].result_time) {
				//std::cout << "Swap " << vec[start].result_time << "(" << start << ")" << " " << vec[end].result_time << "(" << end << ")" << " ";
				swap(vec[start], vec[end]);
				//std::cout << "to " << vec[start].result_time << "(" << start << ")" << " " << " " << vec[end].result_time << "(" << end << ")" << endl;
			}
		}
		else {
			int mid = (start + end) / 2;
			sort(vec, start, mid);
			sort(vec, mid + 1, end);
			merge(vec, start, end, mid);
		}
	}
}

void merge(vector<PersonalData>& vec, int start, int end, int mid) {
	int i_left(start), i_right(mid + 1);
	vector<PersonalData> res_vec;

	while (i_left <= mid && i_right <= end) {
		if (vec[i_left].result_time <= vec[i_right].result_time) {
			res_vec.push_back(vec[i_left]);
			i_left++;
		}
		else {
			res_vec.push_back(vec[i_right]);
			i_right++;
		}
	}
	while (i_left <= mid) {
		res_vec.push_back(vec[i_left]);
		i_left++;
	}

	while (i_right <= end) {
		res_vec.push_back(vec[i_right]);
		i_right++;
	}

	for (auto i = 0; i < res_vec.size(); i++) {
		vec[start + i] = res_vec[i];
	}

}


int main()
{
	//кодировка системы стоит на utf-8, поэтому использование setlocale и прочее не требуется

	vector <PersonalData> DataList;

	ifstream file_results;

	try
	{
		file_results.open("results_RUN.txt");
	}
	catch (const std::exception& ex)
	{
		std::cout << "Error with open file \"results_RUN.txt\"\n" << "exception: " << ex.what() << endl;

		std::cout << "Program has stoped" << endl;
		return 0;
	}

	string str_buf;
	// выгружаем данные из файла
	while (!file_results.eof() && file_results.is_open()) {
		PersonalData data;

		auto t = time(nullptr);
		data.startTime = *localtime(&t);
		data.finishTime = *localtime(&t);
		
		file_results >> data.num; // получаем имя

		file_results >> str_buf; // пропускаем "start"

		file_results >> get_time(&data.startTime, "%H:%M:%S"); // получили время старта		
		// пропускаем повтор имени и "finish", не забывая символ переноса строки
		file_results >> str_buf;
		file_results >> str_buf;
		file_results >> str_buf;

		file_results >> get_time(&data.finishTime, "%H:%M:%S"); // получили время старта

		data.result_time = difftime(mktime(&data.finishTime), mktime(&data.startTime));
		if (!file_results.eof()) {
			file_results >> str_buf; // пропускаем символ переноса строки
		}
		

		DataList.push_back(data);

		str_buf.clear();
	}
	file_results.close();

	//отсортируем список участников по наименьшему параметру result_time
	
	sort(DataList,0,DataList.size()-1);

	ifstream file_names("competitors2.json");
	json persons;
	json num_js;
	// дальше запишем имена первых 4-ех участников по идентефикатору (персональному номеру)
	try
	{
		file_names.open("competitors2.json");
	}
	catch (const std::exception& ex)
	{
		cout << "Error with open file \"competitors2.json\"\n" << "exception: " << ex.what() << endl;

		cout << "Program has stoped" << endl;
		return 0;
	}	
	
	
	file_names >> persons;
		
	for (int i = 0; i < 4; i++) {
		num_js = persons[to_string(DataList[i].num)];

		DataList[i].name = num_js["Name"];
		DataList[i].surname = num_js["Surname"];
	}
	file_names.close();
	
	//далее - блок записи в файл и вывода в консоль
	ofstream file_out("final_results.json");
	json output = {
		"1" , {
			{"Имя", DataList[0].name},
			{"Фамилия", DataList[0].surname},
			{"Результат", DataList[0].result_time} // временно так
		},
		"2" , {
			{"Имя", DataList[1].name},
			{"Фамилия", DataList[1].surname},
			{"Результат", DataList[1].result_time} // временно так
		},
		"3" , {
			{"Имя", DataList[2].name},
			{"Фамилия", DataList[2].surname},
			{"Результат", DataList[2].result_time} // временно так
		},
		"4" , {
			{"Имя", DataList[3].name},
			{"Фамилия", DataList[3].surname},
			{"Результат", DataList[3].result_time} // временно так
		}
	};
	file_out << output;

	// выводим в консоль результаты
	auto t = time(nullptr);
	
	cout << "| Занятое место | Нагрудный номер \t| Имя \t| Фамилия \t| Результат \t|" << endl;
	cout << "| --- \t\t| --- \t\t| --- \t\t| --- \t\t| --- \t\t|" << endl;
	tm time_null = *localtime(&t);
	
	time_null.tm_sec = (DataList[0].result_time%60);
	time_null.tm_min = DataList[0].result_time/60;

	cout << "| " << 1 << "\t\t| " << DataList[0].num << "\t\t| " << DataList[0].name << "\t| " << DataList[0].surname << "\t| " 
		<< put_time(&time_null, "%M:%S") << "\t|" << endl;

	time_null.tm_sec = (DataList[1].result_time % 60);
	time_null.tm_min = DataList[1].result_time / 60;

	cout << "| " << 2 << "\t\t| " << DataList[1].num << "\t\t| " << DataList[1].name << "\t| " << DataList[1].surname << "\t| " 
		<< put_time(&time_null, "%M:%S") << "\t|" << endl;

	time_null.tm_sec = (DataList[2].result_time % 60);
	time_null.tm_min = DataList[2].result_time / 60;

	cout << "| " << 3 << "\t\t| " << DataList[2].num << "\t\t| " << DataList[2].name << "\t| " << DataList[2].surname << "\t| " 
		<< put_time(&time_null, "%M:%S") << "\t|" << endl;
	time_null.tm_sec = (DataList[3].result_time % 60);
	time_null.tm_min = DataList[3].result_time / 60;
	cout << "| " << 4 << "\t\t| " << DataList[3].num << "\t\t| " << DataList[3].name << "\t| " << DataList[3].surname << "\t| " 
		<< put_time(&time_null, "%M:%S") << "\t|" << endl;

	file_out.close();
	return 0;
}
