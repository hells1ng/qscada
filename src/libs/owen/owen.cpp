#include "owen.h"
/* Static variables OwenClass
 */
int         OwenClass::counter = 0;
int         OwenClass::nb_fail = 0;
uint16_t    OwenClass::tab_rp_registers[] = {};
float       OwenClass::real[] = {};
char        OwenClass::str_data[STR_DATA_LINE][STR_DATA_COLUMNS] = {};
std::string OwenClass::data = "";
std::string OwenClass::str_id = "";

void OwenClass::print_results()
{
    printf("Test devices via RS485: ");
    if (nb_fail) {
        printf("%d FAILS\n", nb_fail);
        clear_fail();
    }
    else
        printf("SUCCESS\n");
}

std::string OwenClass::makeId(std::string line_, int addr_, int pin_)
{
    char a[4], p[4];
    sprintf(a, "%d",addr_);
    sprintf(p, "%d",pin_);
    return line_ + ":" + a + ":" + p;
}

void OwenClass::send_data()
{
    SqlDriver SQL;
//    SQL.toDataTable(SQL.prepare_data(QString::fromStdString(data))); // TODO
}

/* Функция возвращает из строки, состоящей из элементов,
 * соединенных через разделитель, вектор этих элементов
 */
vector<string> OwenClass::splitString(const string &fullstr,
                                      const string &delimiter)
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

vector<string> OwenClass::find_guid_parameters(GuidClass* guid, string lookingguid)
{
    vector<string> returnstring;

    if ((*guid)(lookingguid) != "no_id") {
        returnstring = splitString((*guid)(lookingguid), ":");//line:addr:pin
        returnstring.push_back(lookingguid);//guid
    }
    return returnstring;
};

int OwenClass::write_data(ModbusClass* modbus, vector<string> stringguidforwrite, int valueforwrite)
{
    return 0;
};

void OwenClass_16D::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND);
        int rc = modbus_read_input_registers(modbus->ctx, ADDRESS_START_DI, nb_DI, tab_rp_registers);
        if (rc != nb_DI) {
            printf("ERROR modbus_read_input_bits\n");
            printf("Address = %d, nb = %d\n", address, nb_DI);
            nb_fail++;
        }
        else {
            for (int j = 0; j < nb_DI; j++) {
                for (int i = 0; i < nb_BITS; i++) {
                    tab_rp_bits[i] = (tab_rp_registers[j] & (1 << i)) ? 1 : 0;

                    str_id = makeId(str_line, address, i+1);

                    if ((*guid)[str_id] != "no_guid") {
                        sprintf(str_data[1], "%d", tab_rp_bits[i]);
                        data = "";
                        data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";

                        if (PRINT_RESULTS)
                            std::cout << data << std::endl;
                        if (SENDING && (isFirstReading || (tab_rp_bits[i] != tab_rp_bits_prev[i])))
                            send_data();

                        tab_rp_bits_prev[i] = tab_rp_bits[i];
                        data_16D.bits[i] = tab_rp_bits_prev[i]; //TODO test
                    }
                }
            }
        }
        usleep(DELAY_PRE_SEND);

        rc = modbus_read_input_registers(modbus->ctx, ADDRESS_START_DI2, nb_DI2, tab_rp_registers);
        if (rc != nb_DI2) {
            printf("ERROR modbus_read_input_registers\n");
            printf("Address = %d, nb = %d\n", address, nb_DI2);
            nb_fail++;
        }
        else {
            for (int j = 0; j < nb_DI2; j++) {

                str_id = makeId(str_line, address, j+1+16);

                if ((*guid)[str_id] != "no_guid") {
                    sprintf(str_data[1], "%.2f", (float)tab_rp_registers[j]/100 + 0.75);	// 0.75 - для счетчика воды
                    data = "";
                    data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";
                    if (PRINT_RESULTS)
                        std::cout << data << std::endl;
                    if (SENDING && (isFirstReading || (tab_rp_registers[j] != tab_rp_registers_prev[j])))
                        send_data();
                    //if (SENDING&&(isFirstReading||(tab_rp_registers[j]!=tab_rp_registers_prev[j]))) send_data(mq_id);
                    tab_rp_registers_prev[j] = tab_rp_registers[j];
                    data_16D.regs[j] = tab_rp_registers_prev[j]; //TODO test
                }
            }
        }
        isFirstReading = false;
    }
    modbus->close();
};

void OwenClass_8A::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND * 2);
        int rc = modbus_read_input_registers(modbus->ctx, ADDRESS_START_RTD, nb_RTD, tab_rp_registers);
        if (rc != nb_RTD) {
            printf("ERROR modbus_read_input_registers\n");
            printf("Address = %d, nb = %d\n", address, nb_RTD);
            nb_fail++;
        }
        else {
            int k;

            for (int j = 0; j < nb_RTD; j++) {
                if (j == 1 || j == 7 || j == 13 || j == 19 || j == 25 || j == 31 || j == 37 || j == 43) {
                    if (tab_rp_registers[j] < 0b1000000000000000) {
                        k = j/6;
                        real[k] = static_cast<float>(tab_rp_registers[j])/(100);
                    }
                    else {
                        k = j/6;
                        real[k] = static_cast<float>(0xFFFF - tab_rp_registers[j])/(-100);
                    }
                }
            }

            for (int i = 0; i < 8; i++) {

                str_id = makeId(str_line, address, i+1);

                if ((*guid)[str_id] != "no_guid") {
                    sprintf(str_data[1], "%.2f", real[i]);
                    data = "";
                    data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";
                    if (PRINT_RESULTS)
                        std::cout << data << std::endl;
                    if (SENDING && (isFirstReading/*||(abs(real[i]-real_prev[i])>DELTA)*/))
                        send_data();
                    real_prev[i] = real[i];
                }
            }
        }
        isFirstReading = false;
    }
    modbus->close();
};

void OwenClass_8AC::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND);
        int rc = modbus_read_input_registers(modbus->ctx, ADDRESS_START_AI, nb_AI, tab_rp_registers);
        if (rc != nb_AI) {
            printf("ERROR modbus_read_registers (%d)\n", rc);
            printf("Address = %d, nb = %d\n", address, nb_AI);
            nb_fail++;
        }
        else {
            for (int j = 0; j < nb_AI; j++) {
                if (tab_rp_registers[j]<0b1000000000000000) {
                    real[j] = static_cast<float>(tab_rp_registers[j])/(100);
                }
                else
                    real[j] = static_cast<float>(0xFFFF - tab_rp_registers[j])/(-100);

                str_id = makeId(str_line, address, j+1);

                if ((*guid)[str_id] != "no_guid") {
                    sprintf(str_data[1], "%.2f", real[j]);
                    data = "";
                    data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";

                    if (PRINT_RESULTS)
                        std::cout << data << std::endl;

                    float deltax = real[j] - real_prev[j];
                    bool needupdate;

                    if (deltax >= 0) {
                        if (deltax - DELTA > 0)
                            needupdate = true;
                        else
                            needupdate = false;
                    }
                    else {
                        if (deltax + DELTA < 0)
                            needupdate = true;
                        else
                            needupdate = false;
                    }

                    if (SENDING && (isFirstReading || needupdate))
                        send_data();
                    if (isFirstReading) {
                        real_prev[j] = real[j];
                    }//update previous data in firstReading and deltax > DELTA
                    else if (needupdate) {
                        real_prev[j] = real[j];
                    }
                }
            }
        }
        isFirstReading = false;
    }
    modbus->close();
};

        //TODO: previous values and data for sending
void OwenClass_KM::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND);
        int rc = modbus_read_input_registers(modbus->ctx, ADDRESS_START_KM, nb_KM, tab_rp_registers);
        if (rc != nb_KM) {
            printf("ERROR modbus_read_input_registers\n");
            printf("Address = %d, nb = %d\n", address, nb_KM);
            nb_fail++;
        }
        else {
            //int addr=ADDRESS_START_KM;
            for (int j = 0; j < nb_KM; j++) {
                switch(j + ADDRESS_START_KM) {
                    case 19://KtU
                    {
                        real[0] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1]);
                        //printf("real= %.2f \n", real[0]);
                        break;
                    }
                    case 22://KtI
                    {
                        real[1] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1]);
                        //printf("real= %.2f \n", real[1]);
                        break;
                    }
                    case 25://U
                    {
                        real[2] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[2]);
                        break;
                    }
                    case 28://I
                    {
                        real[3] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[3]);
                        break;
                    }
                    case 31://N
                    {
                        real[4] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[4]);
                        break;
                    }
                    case 34://Na
                    {
                        real[5] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[5]);
                        break;
                    }
                    case 37://Nr
                    {
                        real[6] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[6]);
                        break;
                    }
                    case 40://Km
                    {
                        real[7] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[7]);
                        break;
                    }
                    case 43://f
                    {
                        real[8] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[8]);
                        break;
                    }
                    case 45://E
                    {
                        real[9] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[9]);
                        break;
                    }
                    case 47://Ea
                    {
                        real[10] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[10]);
                        break;
                    }
                    case 49://Er
                    {
                        real[11] = static_cast<float>(((tab_rp_registers[j]) << 16) + tab_rp_registers[j+1])/100;
                        //printf("real= %.2f \n", real[11]);
                        break;
                    }
                }
            }
            str_id = makeId(str_line, address, 0);
            data = "";
            data = data + (*guid)[str_id];
            for (int i = 0; i < 12; i++) {
                sprintf(str_data[i+1], "%.2f", real[i]);
                data = data + "|" + str_data[i+1];
            }
            data = data + "|";//+"\r\n";
            if (PRINT_RESULTS)
                std::cout << data << std::endl;
            if (SENDING)
                send_data(); //id/KtU/KtI/U/I/N/Na/Nr/Km/f/E/Ea/Er/
        }
    }
    modbus->close();
};

void OwenClass_SimDI::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND);
        int rc = modbus_read_input_bits(modbus->ctx, ADDRESS_START_SimDI, nb_SimDI, tab_rp_bits);
        if (rc != nb_SimDI) {
            printf("ERROR modbus_read_input_bits\n");
            printf("Address = %d, nb = %d\n", address, nb_SimDI);
            nb_fail++;
        }
        else {

            for (int j = 0; j < nb_SimDI; j++) {

                str_id = makeId(str_line, address, j+1);

                if ((*guid)[str_id] != "no_guid") {
                    sprintf(str_data[1], "%d", tab_rp_bits[j]);
                    data = "";
                    data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";
                    if (PRINT_RESULTS)
                        std::cout<<data<<std::endl;
                    if (SENDING)
                        send_data();
                }
            }
        }
    }
    modbus->close();
};

void OwenClass_SimAI::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND);
        int rc = modbus_read_input_registers(modbus->ctx, ADDRESS_START_SimAI, nb_SimAI, tab_rp_registers);
        if (rc != nb_SimAI) {
            printf("ERROR modbus_read_input_registerss\n");
            printf("Address = %d, nb = %d\n", address, nb_SimAI);
            nb_fail++;
        }
        else {
            for (int j = 0; j < nb_SimAI; j++) {

                str_id = makeId(str_line, address, j+1);

                if ((*guid)[str_id] != "no_guid") {
                    sprintf(str_data[1], "%d", tab_rp_registers[j]);
                    data = "";
                    data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";
                    if (PRINT_RESULTS)
                        std::cout << data << std::endl;
                    if (SENDING)
                        send_data();
                }
            }
        }
    }
    modbus->close();
};

void OwenClass_NL_8R::read_data(ModbusClass* modbus, GuidClass* guid)
{
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, address);
        usleep(DELAY_PRE_SEND);
        int rc = modbus_read_bits(modbus->ctx, ADDRESS_START_8R, nb_8R, tab_rp_bits);
        if (rc != nb_8R) {
            printf("ERROR modbus_read_bits\n");
            printf("Address = %d, nb = %d\n", address, nb_8R);
            nb_fail++;
        }
        else {
            for (int j = 0; j < nb_8R; j++) {

                str_id = makeId(str_line, address, j+1);

                if ((*guid)[str_id] != "no_guid") {
                    sprintf(str_data[1], "%d", tab_rp_bits[j]);
                    data = "";
                    data = data + (*guid)[str_id] + "|" + str_data[1] + "|0";//+"\r\n";
                    if (PRINT_RESULTS)
                        std::cout << data << std::endl;
                    if (SENDING && (isFirstReading || (tab_rp_bits[j] != tab_rp_bits_prev[j])))
                        send_data();
                    tab_rp_bits_prev[j] = tab_rp_bits[j];
                }
            }
        }
        isFirstReading = false;
    }
    modbus->close();
};

int OwenClass_NL_8R::write_data(ModbusClass* modbus, vector<string> stringguidforwrite, int valueforwrite)
{
    int ret = 0;
    if (modbus->isConnected()) {
        modbus_set_slave(modbus->ctx, atoi( stringguidforwrite[1].c_str() ));//addr
        usleep(DELAY_PRE_SEND);
        ret = modbus_write_bit(modbus->ctx, atoi( stringguidforwrite[2].c_str() ) - 1, valueforwrite);
    }
    modbus->close();
    return ret;
};
