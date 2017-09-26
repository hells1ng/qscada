#ifndef MAIN_H
#define MAIN_H
/*Need to link library -lboost_system
 *  ln -s /usr/local/lib/libboost_system.so.1.62.0 /usr/lib/libboost_system.so.1.62.0
 *  ln -s /usr/local/lib/libboost_thread.so.1.62.0 /usr/lib/libboost_thread.so.1.62.0
 *  ln -s /usr/local/lib/libboost_date_time.so.1.62.0 /usr/lib/libboost_date_time.so.1.62.0
 */
int write_Owen_data(ModbusClass* modbus, vector<OwenClass*> ovector, GuidClass* guidMap , string guidforwrite, int valueforwrite)
{ //TODO реализовать запись в тредменегере
    int ret_ = 0;
    OwenClass *owenptr = ovector[0];

    vector<string> stringguidforwrite = owenptr->find_guid_parameters(guidMap, guidforwrite);//addr pin guid

    if (stringguidforwrite.size() == 0) {
        return ret_;
    }

    for (unsigned int i = 0; i < ovector.size(); i++) {
        owenptr = ovector[i];

        if (to_string(owenptr->address) == stringguidforwrite[1]) {
            int rc = owenptr->write_data(modbus, stringguidforwrite, valueforwrite);
            if (rc == 1) {
                ret_ = 1;
            }
        }
    }
    return ret_;
};

void FromPost()//TODO: interface func SqlDriver with Post table
{
    //cout << "       FROM_POST_THREAD " << endl;
    SqlDriver Sql_post;
    vector <string> arr;

//    arr = Sql_post.fromPost();// guid, value, status=0
    if ((arr.size() == 3)) {
//        bool res = (write_Owen_data(&Modbus, OwenVector, &Guid, arr[0], stoi(arr[1])) == 1);
        // true - success, false = error
//        Sql_post.toPost(arr[0], res); // guid, status
    }
}

#endif //MAIN_H
