#ifndef ID_H_INCLUDED
#define ID_H_INCLUDED
using std::string;
using std::vector;
using std::ifstream;
using std::istringstream;
using std::cout;
using std::endl;
using std::ofstream;

#include <boost/config.hpp>
#include <boost/bimap.hpp>

//TODO Обернуть функцию в Класс GuidMap
template<class MapType>
void print_map(const MapType & map,
               const std::string & separator,
               std::ostream & os)
{
    typedef typename MapType::const_iterator const_iterator;

    for( const_iterator i = map.begin(), iend = map.end(); i != iend; ++i )
    {
        os << i->first << separator << i->second << std::endl;
    }
}

typedef boost::bimap<std::string, std::string> GuidMap;
typedef GuidMap::value_type position;

class GuidClass
{
public:
     vector<string> elements;
     string firstofmap;
    GuidMap Map;
    //static std::vector<std::string> Vec;
    std::vector<std::string> Vec;
    GuidClass();
    ~GuidClass () {
    };
    vector<string> splitString(const string &fullstr, const string &delimiter);
    double  stringToDouble(const string &str);
    void    readDataDouble(const string &filename, const string &csvdelimiter,
                           vector<vector<double> > &sarr);
    int     readDataString(const string &filename, const string &csvdelimiter,
                       vector< vector<string> > &sarr_rs485, vector< vector<string> > &sarr_can) ;//Read CSV
    void    readFirstStr(const string &filename, const string &csvdelimiter);
    void    WriteDataString(const string &filename,
                         const string &csvdelimiter);
 /*   int     guid_init(string (&guid_rs485)[ID_LINE][ID_COLUMNS],
                  string (&guid_can)[ID_LINE][ID_COLUMNS],
                  const string& filename);//Init guid table
 */
    void    add_sensor(const string &str, GuidMap& guidMap);

    static std::vector<std::string> init_sensorvector();
    GuidMap init_map();
    string operator[](string key);
    string operator()(string value_);
};

#endif
