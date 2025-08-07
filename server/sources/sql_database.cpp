#include "sql_database.h"


void SqliteError(int op_code, std::string_view message){
    if(op_code != SQLITE_OK){
       std::cerr << "SQLite error : " << message << std::endl;
       // handle with some other situation 
    }
}

std::string Trim(const std::string & tables){
    const std::string whitespace = "\t\n\r";
    const auto strBegin = tables.find_first_not_of(whitespace);
    if(std::string::npos == strBegin){
        return "";
    }
    const auto strEnd = tables.find_last_not_of(whitespace);
    return tables.substr(strBegin, strEnd);
}

std::vector<std::string> S_TableSQL(std::string && tables){
    std::string start_delimieter = "START TABLE";
    std::string end_delimieter = "END TABLE";
    std::vector<std::string> tables_sql;
    for(auto && part : tables | std::ranges::views::split(start_delimieter)){
        std::string s_part(part.begin(),part.end());
        for(auto && inner_part : s_part | std::ranges::views::split(end_delimieter)){
            std::string create_sql(inner_part.begin(), inner_part.end());
            std::string final_create_sql = Trim(create_sql);
            if(!final_create_sql.empty()){
                tables_sql.push_back(final_create_sql);
            }
        }
    }
    return tables_sql;
}

std::vector<std::string> SplitSQL(std::string && tables, int type = 1){
    //1 -- sql :: create table
    switch (type)
    {
    case 1:
        return S_TableSQL(std::move(tables));
    default:
        std::cerr << "Split nothing because wrong type." << std::endl;
        return {};
    }
}






SQLiteDatabase::SQLiteDatabase(std::string && db_filename)
: sql_db_filename(std::move(db_filename)),
sql_operator_code(SQLITE_OK),
sql_db_ptr(nullptr)
{
    CreateDatabase();
}

bool SQLiteDatabase::CreateDatabase(){
    if(sql_db_ptr){
        std::cout << "database has already created." << std::endl;
        return false;
    }else{
        sql_operator_code = sqlite3_open(sql_db_filename.data(), &sql_db_ptr);
        if(sql_operator_code != SQLITE_OK){
            SqliteError(sql_operator_code, "Can't open database");
            sqlite3_close(sql_db_ptr);
            sql_db_ptr = nullptr;
        }else{
            std::cout << "Database opened successfully." << std::endl;
        }
    }
}

bool SQLiteDatabase::CreateTable(std::string_view create_table_sql){
    sql_operator_code = sqlite3_exec(sql_db_ptr, create_table_sql.data(),0,0,0);
    SqliteError(sql_operator_code, "it's failed to create table.");
}

void SQLiteDatabase::CloseDatabase(){
    if(sql_db_ptr){
        sqlite3_close(sql_db_ptr);
        std::cout << "Database closed" << std::endl;
    }
}


std::vector<std::string> SQLiteDatabase::ImportCreateSQL(std::string_view tables_file_path){
    std::ifstream sql_file(tables_file_path.data(), std::ios_base::beg);
    if(!sql_file.is_open()){
        // throw std::runtime_error("tables_file can't open.\n");
        std::cerr << "tables_file can't open." << std::endl;
        return {}; 
    }
    std::string tables(
        std::istreambuf_iterator<char>(sql_file),
        std::istream_iterator<char>()
    );
    return SplitSQL(std::move(tables));
}

SQLiteDatabase::~SQLiteDatabase(){
    CloseDatabase();
}