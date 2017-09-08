#ifndef INIT_SENSORSVECTOR_H_INCLUDED
#define INIT_SENSORSVECTOR_H_INCLUDED

std::vector<std::string> GuidClass::init_sensorvector()
{
    std::vector<std::string> vec_;
    vec_.push_back("RS485,1,1,aaecfc5f2ffa4b43aecfd0bf18210d61,9,-,-,-,Nasos 1");
    vec_.push_back("RS485,1,2,cc305ace7b6946c19b25986b7186bfe0,8,-,-,-,Datchik protechki 1");
    vec_.push_back("RS485,1,3,e06ef2480bca4b00a6bc718afc800a99,10,-,-,-,Yroven davleniya 1");
    vec_.push_back("RS485,1,4,647612cf692a4a288697d072182d8b93,7,-,-,-,Vvodno1 avtomat 1");
    vec_.push_back("RS485,1,21,b0ac1d32e60e42f9a4beb99100e4b98a,1,-,-,-,C4etchik vodi 1");
    vec_.push_back("RS485,41,1,a951c38acdd944aea73a4ec59832ff26,5,-,-,-,Dat4ik temperaturi 3");
    vec_.push_back("RS485,41,2,39a51cf4048840afb4e74bf0aa0a3358,5,-,-,-,Dat4ik vlajnosti 4");
    vec_.push_back("CAN,44,0,1e146e46623044b1972078ba22ea6579,4,-,-,-,S4et4ik elektri4eskii");
    vec_.push_back("RS485,2,5,5f1b3f0977c94b8fbb5811d5baecdb5a,0,-,-,-,0");
	return vec_;
}
#endif //INCLUDES_H_INCLUDED
