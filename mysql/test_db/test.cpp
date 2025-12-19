#include<iostream>
#include<unistd.h>
#include<mysql/mysql.h>
#include<string>
const std::string host="127.0.0.1";
const std::string user="connector";
const std::string passwd="xdh07309318.";
const std::string db="conn";
const unsigned int port=4579;
int main(){
    // std::cout<<"mysql client version: "<<mysql_get_client_info()<<std::endl;
    MYSQL *my=mysql_init(nullptr);
    if(nullptr==my){
        std::cerr<<"init MySQL error"<<std::endl;
        return 1;
    }
    if(mysql_real_connect(my,host.c_str(),user.c_str(),passwd.c_str(),db.c_str(),port,nullptr,0)==nullptr){
        std::cerr<<"connect db error"<<std::endl;
        return 2;
    }
    mysql_set_character_set(my,"utf8");
    // std::string sql="insert into user(name,age,telphone) values('张三',19,'66571189')";
    // std::string sql="delete from user where id=3";
    std::string sql="select * from user";
    int n=mysql_query(my,sql.c_str());
    if(n==0){
        std::cout<<"success"<<std::endl;
    }
    MYSQL_RES *res=mysql_store_result(my);
    my_ulonglong rows=mysql_num_rows(res);
    my_ulonglong fields=mysql_num_fields(res);
    std::cout<<"行："<<rows<<std::endl;
    std::cout<<"列："<<fields<<std::endl;
    MYSQL_FIELD *field_arr=mysql_fetch_fields(res);
    for(int i=0;i<fields;i++){
        std::cout<<field_arr[i].name<<'\t';
    }
    std::cout<<"\n";
    for(int i=0;i<rows;i++){
        MYSQL_ROW row=mysql_fetch_row(res);
        for(int j=0;j<fields;j++){
            std::cout<<row[j]<<'\t';
        }
        std::cout<<'\n';
    }
    std::cout<<field_arr->db<<std::endl;
    std::cout<<field_arr->table<<std::endl;
    // while(true){
        
    //     std::cout<<"mysql>>>";
    //     if(!std::getline(std::cin,sql)) break;
    //     if(sql=="quit") break;
    //     int n=mysql_query(my,sql.c_str());
    //     if(n==0){
    //         std::cout<<sql<<" success: "<<std::endl;
    //     }
    //     else{
    //         std::cerr<<sql<<" fail "<<std::endl;
    //     }
    // }
    // std::cout<<"bye bye"<<std::endl;
    mysql_free_result(res);
    mysql_close(my);
    return 0;
}