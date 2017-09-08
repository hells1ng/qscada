#ifndef _MSC_VER
#include <unistd.h>
#endif
#include <stdlib.h>
#include <vector>
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <inttypes.h>
#include <string>
#include <sstream>
#include <fstream>
#include "../../defines.h"
#include "id.h"
#include "init_sensorvector.h"

//vector<string>              GuidClass::elements;
//string                      GuidClass::firstofmap;
//std::vector<std::string>    GuidClass::Vec = init_sensorvector();

GuidClass::GuidClass()
{
    Vec = init_sensorvector();
    Map = init_map();
}

GuidMap GuidClass::init_map()
{
    GuidMap map_;
    for (size_t i = 0; i < Vec.size(); i++) {
        add_sensor(Vec[i], map_);
    }
	return map_;
}

 void GuidClass::add_sensor(const string &str, GuidMap& guidMap)
{
    elements = splitString(str, ",");

    firstofmap = elements[0] + ":" + elements[1] + ":" + elements[2];

    guidMap.insert(position(firstofmap ,elements[3]));

    elements.clear();
    firstofmap.clear();
}

string GuidClass::operator[](string key)
{
    string ret = "no_guid";
    try
    {
        ret = Map.left.at(key);
    }
    catch (std::out_of_range & e) {}
        //return Map.left.at(key);
    return ret;
}

string GuidClass::operator()(string value_)
{
    string ret = "no_id";
    try
    {
        ret = Map.right.at(value_);
    }
    catch (std::out_of_range & e) {}
        //return Map.left.at(key);
    return ret;
}

/* Функция возвращает из строки, состоящей из элементов,
 * соединенных через разделитель, вектор этих элементов
 */
vector<string> GuidClass::splitString(const string &fullstr, const string &delimiter)
{
    vector<string> elements;
    string::size_type lastpos = fullstr.find_first_not_of(delimiter, 0);
    string::size_type pos = fullstr.find_first_of(delimiter, lastpos);
    string substring("");

    while ((string::npos != pos) || (string::npos != lastpos)) {

        substring = fullstr.substr(lastpos, pos-lastpos);
        elements.push_back(substring);
        substring = "";

        lastpos = fullstr.find_first_not_of(delimiter, pos);
        pos = fullstr.find_first_of(delimiter, lastpos);
    }
    return elements;
}

/* Преобразуем строку в число
 */
double GuidClass::stringToDouble(const string &str)
{
    istringstream stm;
    double val = 0;
    stm.str(str);
    stm >> val;

    return val;
}

/* Создаем двумерный массив чисел из файла построчно
 */
void GuidClass::readDataDouble(const string &filename, const string &csvdelimiter,
                               vector< vector<double> > &sarr)
{
    ifstream fin(filename.c_str());
    string s;
    vector<string> selements;
    vector<double> delements;

    while (!fin.eof()) {
        getline(fin, s);

        if (!s.empty()){
            selements = splitString(s, csvdelimiter);

            for ( size_t i = 0; i < selements.size(); i++) {
                delements.push_back( stringToDouble(selements[i]) );
            }
            sarr.push_back(delements);
            selements.clear();
            delements.clear();
        }
    }
    fin.close();
}
/* Получаем 2 двумерных массива строк из файла
 */
int GuidClass::readDataString(const string &filename, const string &csvdelimiter,
                              vector< vector<string> > &sarr_rs485,
                              vector< vector<string> > &sarr_can)
{
    ifstream fin(filename.c_str());
    string s;
    vector<string> selements;

    if (!fin.is_open()) {
        cout << "Error opening CSV-file" << endl;
        return 0;
    }
    while (getline(fin, s)) {
        if (!s.empty()) {
            /*Делим строку на элеменыты*/
            selements = splitString(s, csvdelimiter);
            if (selements[0] == "RS485")
                sarr_rs485.push_back(selements);
            if (selements[0] == "CAN")
                sarr_can.push_back(selements);
            selements.clear();
        }
    }
    fin.close();
    return 0;
}

/*Выводим в консоль 1 строку файла
 */
void GuidClass::readFirstStr(const string &filename, const string &csvdelimiter)
{
    ifstream fin(filename.c_str());
    string s;
    vector<string> selements;

    getline(fin, s);

    if (!s.empty()) {
        selements = splitString(s, csvdelimiter);
        for ( size_t i = 0; i < selements.size(); i++ ) {
            cout << selements[i] << "\t";
        }
        cout << endl;
        selements.clear();
    }
    fin.close();
}

void GuidClass::WriteDataString(const string &filename, const string &csvdelimiter)
{
    ofstream fout(filename.c_str());
    uint16_t addr[4] = {1, 11, 41, 61};
    uint16_t number[4] = {32, 8, 8, 16};
    string s;
    vector<string> selements;
    fout << "Type" << csvdelimiter << "a1" << csvdelimiter << "a2" << csvdelimiter << "int_id" << csvdelimiter << "GUID" <<
    csvdelimiter << "Comment" << endl;
    for (int j = 0; j < 4; j++) {
        for (int  i = 1; i <= number[j]; i++) {
            fout << 0 << csvdelimiter << addr[j] << csvdelimiter << i << csvdelimiter << "m" << addr[j] << "." << i <<
            csvdelimiter << 0 << csvdelimiter << 0 << endl;
        }
    }
    fout.close();
}
/*
int GuidClass::guid_init(string (&guid_rs485)[ID_LINE][ID_COLUMNS], string (&guid_can)[ID_LINE][ID_COLUMNS],
                         const string& filename)
{
    vector< vector<string> > strarr_rs485, strarr_can;
    readDataString(filename, ",", strarr_rs485, strarr_can);

    /*Вывод в консоль всего CSV файла*/
/*
    readFirstStr(filename,",");
    cout << "-------------------------------------------------------------------------\n";
    for (size_t i = 0; i < strarr_rs485.size(); i++) {

        for (size_t j = 0; j < strarr_rs485[0].size(); j++) {
            cout << strarr_rs485[i][j] << "\t";
        }
        cout << "\n";
    }
    cout << "-------------------------------------------------------------------------\n";

    for (size_t i = 0; i < strarr_can.size(); i++) {

        for (size_t j = 0; j < strarr_can[0].size(); j++) {
            cout << strarr_can[i][j] << "\t";
        }
        cout << "\n";
    }
    cout << "-------------------------------------------------------------------------\n";

        //Заполняем таблицы GUID
    for (size_t i = 0; i < strarr_rs485.size(); i++) {
        guid_rs485[ atoi( strarr_rs485[i][1].c_str() ) ][ atoi( strarr_rs485[i][2].c_str() ) ] = strarr_rs485[i][3]; //STRING
    }
    for (size_t i = 0; i < strarr_can.size(); i++ ) {
        guid_can[ atoi( strarr_can[i][1].c_str() ) ][ atoi( strarr_can[i][2].c_str() ) ] = strarr_can[i][3]; //STRING
    }
    return 0;
}
*/




