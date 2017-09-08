vector<string> splitStr(const string &fullstr,
                                        const string &delimiter)
{
    vector<string> elements;
    string::size_type lastpos = fullstr.find_first_not_of(delimiter, 0);
    string::size_type pos = fullstr.find_first_of(delimiter, lastpos);
    string substring("");

    while ( (string::npos != pos) || (string::npos != lastpos) ) {

        substring = fullstr.substr(lastpos, pos-lastpos);
        elements.push_back(substring);
        substring = "";
        lastpos = fullstr.find_first_not_of(delimiter, pos);
        pos = fullstr.find_first_of(delimiter, lastpos);
    }
    return elements;
}
void addsensor(const string &str, GuidMap& guidMap)
{
    cout<<"Add sensor: "<<str<<endl;
    vector<string> elements = splitStr(str, ",");

    for ( size_t i=0; i<elements.size(); i++ ) {

        cout << elements[i] << "\t";
    }
    string firstofmap = elements[0] + ":" + elements[1] + ":" + elements[2];
    //guidMap.insert(std::pair<string,string>(firstofmap, elements[3]));
    guidMap[firstofmap] = elements[3];
    guidMap["1"]="first";
    guidMap["2"]="second";
    elements.clear();


}

void init_map(GuidMap& guidmap)
{
    addsensor("RS485,1,1,aaecfc5f2ffa4b43aecfd0bf18210d61,9,-,-,-,Nasos 1", guidmap);
 /*   addsensor("RS485,1,2,cc305ace7b6946c19b25986b7186bfe0,8,-,-,-,Datchik protechki 1", guidmap);
    addsensor("RS485,1,3,e06ef2480bca4b00a6bc718afc800a99,10,-,-,-,Yroven davleniya 1", guidmap);
    addsensor("RS485,1,4,647612cf692a4a288697d072182d8b93,7,-,-,-,Vvodno1 avtomat 1", guidmap);
    addsensor("RS485,1,21,b0ac1d32e60e42f9a4beb99100e4b98a,1,-,-,-,C4etchik vodi 1", guidmap);
    addsensor("RS485,41,1,a951c38acdd944aea73a4ec59832ff26,5,-,-,-,Dat4ik temperaturi 3", guidmap);
    addsensor("RS485,41,2,39a51cf4048840afb4e74bf0aa0a3358,5,-,-,-,Dat4ik vlajnosti 4", guidmap);
    addsensor("CAN,44,0,1e146e46623044b1972078ba22ea6579,4,-,-,-,S4et4ik elektri4eskii", guidmap);
    addsensor("RS485,2,5,5f1b3f0977c94b8fbb5811d5baecdb5a,0,-,-,-,0", guidmap);
*/
	//myMap.insert(std::pair<string,string>("one","first") );
	//myMap.insert(std::pair<string,string>("two","second") );
	//myMap[1]=0;
}
