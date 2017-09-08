#ifndef MAIN_H
#define MAIN_H
/*Need to link library -lboost_system
 *  ln -s /usr/local/lib/libboost_system.so.1.62.0 /usr/lib/libboost_system.so.1.62.0
 *  ln -s /usr/local/lib/libboost_thread.so.1.62.0 /usr/lib/libboost_thread.so.1.62.0
 *  ln -s /usr/local/lib/libboost_date_time.so.1.62.0 /usr/lib/libboost_date_time.so.1.62.0
 */
int write_Owen_data(ModbusClass* modbus, vector<OwenClass*> ovector, GuidClass* guidMap , string guidforwrite, int valueforwrite)
{
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


void owen_thread()
{
    //cout << "   OWEN_THREAD " << endl;
    for (unsigned int i = 0; i < OwenVector.size(); i++) {
        Owen_ptr = OwenVector[i];
        Owen_ptr->read_data(&Modbus, &Guid);
    }

    /*
    Owen_16D_1.read_data(&Modbus, &Guid);
    Owen_8AC_41.read_data(&Modbus, &Guid);
    NL_8R_2.read_data(&Modbus, &Guid);
    */

    //Owen_ptr->print_results();

    //system("free -m");
}

void mercury_thread()
{
    //cout << "           MERCURY_THREAD " << endl;
    Mercury_44.read_data(&Can, &Guid);

    //Net1.write();
}

void FromPost()
{
    //cout << "       FROM_POST_THREAD " << endl;
    Sql_driver Sql_post;
    vector <string> arr;

    arr = Sql_post.fromPost();// guid, value, status=0
    if ((arr.size() == 3)) {
        bool res = (write_Owen_data(&Modbus, OwenVector, &Guid, arr[0], stoi(arr[1])) == 1);
        // true - success, false = error
        Sql_post.toPost(arr[0], res); // guid, status
    }
}

void sendToServer()
{
   // cout << "sendToServer_THREAD " << endl;

    Sql_driver SQL_data;
    HttpsDriver   httpsDriver;
    //SQL_data.fromData(4);

    httpsDriver.Send( SQL_data.fromData(10) );
}

void getSensorIntervalFromServer()
{
    HttpsDriver   httpsDriver;
    httpsDriver.Send( HttpsDriver::GET_SENSOR_PERIOD );
}

void initOwenVector()
{
    if (SIM_MODE) {
        for (int i = 100;i <= 202; i++) {
            if (i <= 135) {
                OwenClass_SimDI* SimDi = new OwenClass_SimDI(i, 16, "RS485");
                OwenVector.push_back(SimDi);
            }
            else {
                OwenClass_SimAI* SimAi = new OwenClass_SimAI(i, 16, "RS485");
                OwenVector.push_back(SimAi);
            }
        }
    }
    else {
        OwenVector.push_back(&Owen_16D_1);
        //OwenVector.push_back(&Owen_8A_11);
        //OwenVector.push_back(&Owen_8AC_41); // 8AC pogib
        //OwenVector.push_back(&Owen_KM_56);
        //OwenVector.push_back(&UNO_61);
        //OwenVector.push_back(&NL_8R_2);
    }
}

void print1()
{
    cout << " print1" << endl;
}

void scripts() //TODO test
{
    DI = Owen_16D_1.data_16D.bits;
    cout << "DI.0 = " << (int)DI[0] << endl;
    if ((int)DI[0] == 1)
        write_Owen_data(&Modbus, OwenVector, &Guid, "5f1b3f0977c94b8fbb5811d5baecdb5a", 1);
    else if ((int)DI[0] == 0)
        write_Owen_data(&Modbus, OwenVector, &Guid, "5f1b3f0977c94b8fbb5811d5baecdb5a", 0);
    //cout << "DI.1 = " << (int)DI[1] << endl;
}
#endif //MAIN_H
