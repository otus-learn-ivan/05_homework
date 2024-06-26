
#include "main.h"

#include <thread>
#include <chrono>

using namespace std;

 template <typename T >
 std::vector<T> get_vector_value_from_in(){
   string line{};
   getline(cin,line);
   std::istringstream iss{line};
   std::vector<T> val{};
   std::copy(std::istream_iterator<T>(iss), std::istream_iterator<T>(),std::back_inserter(val));
   return val;
 }
//-------------------------------------------------------------
void Tcoordinate::draw(Tscreen_txt_stream& xout){
    xout.get_screen() << "("<< x <<","<< y<<")";
}
void Tpoint::draw(Tscreen_txt_stream& xout){
    xout.get_screen() << "point";coordinate.draw(xout);xout.reset_tearing_layer();
}
void Tline::draw(Tscreen_txt_stream& xout){
    xout.get_screen() << "line (begin: "; begin_line.draw(xout); xout.get_screen() << "  end: ";end_line.draw(xout); xout.get_screen() << ")";xout.reset_tearing_layer();
}
void Tarc::draw(Tscreen_txt_stream& xout){
      xout.get_screen()<< "arc (angle: " << angle <<",radius: " << radius << ",center: ";center.draw(xout); xout.get_screen() << ")";xout.reset_tearing_layer();
}
void Tpoligon::draw(Tscreen_txt_stream& xout){
    xout.get_screen() << "poligon";xout.reset_tearing_layer();
    Ttab_untab_screen_txt_stream to_right(xout);
    for(auto& primetiv: primetives){
        xout.get_screen() << "id:" << primetiv.first << " - "; primetiv.second->draw(xout) ;//xout.reset_tearing_layer();
    }
}
void Tdocument::draw(Tscreen_txt_stream& xout){
    xout.get_screen() << "Name: " << name << " type: " ;
    Tpoligon::draw(xout);
}
//---------------------------------------------------------------
struct MenuItem {
    std::string title;
    std::function <std::unique_ptr<Tdocument>(std::unique_ptr<Tdocument>)> handler;
};
struct Tcreate_new_document{
    std::unique_ptr<Tdocument> operator()(std::unique_ptr<Tdocument> drawing){
        std::cout << "Tcreate_new_document\n";
        std::cout << "Введите имя документа: ";
        std::string name{};
        getline(cin,name);
        drawing = std::make_unique<Tdocument>(std::move(name));
        return drawing;
    };
};
struct Timport_document_from_file{
    std::unique_ptr<Tdocument> operator()(std::unique_ptr<Tdocument> drawing){
        std::cout << "Timport_document_from_file\n";
        std::cout << "Введите имя файла: ";
        std::string file{};
        getline(cin,file);
        std::ifstream file_in{file};
        drawing = std::move(std::make_unique<Tdocument>());
        drawing->deserialize(*std::make_unique<BoostSerializer>(nullptr,&file_in));
        return drawing;
    };
};
struct Texport_document_to_file{
    std::unique_ptr<Tdocument> operator()(std::unique_ptr<Tdocument> drawing){
        std::cout << "Texport_document_to_file\n";
        if(!drawing){
             std::cout << "Документ не создан\n";
            return drawing;
        }
        std::cout << "Введите имя файла: ";
        std::string file{};
        getline(cin,file);
        std::ofstream file_out{file};
        drawing->serialize(*std::make_unique<BoostSerializer>(&file_out,nullptr));
        return drawing;
    };
};

struct Tcreate_gprimetivew{
    std::unique_ptr<Tdocument> operator()(std::unique_ptr<Tdocument> drawing){
        if(!drawing){
             std::cout << "Документ не создан\n";
            return drawing;
        }
        std::cout << "Tcreate_gprimetivew\n";
        std::cout << "Введите номер типа графического примитива (1 - точка, 2 - линия, 3 - дуга) :";
        std::size_t index = get_vector_value_from_in<int>()[0]-1;
        std::vector<std::function<std::unique_ptr<Tprimetiv>()>> create_primrtive_handler
        {
           [](){
               std::cout << "Введите параметры точки через пробел(int int):";
               auto prm_point = get_vector_value_from_in<int>();
               return create_primetive(Tpoint{prm_point[0],prm_point[1]});
           },
           [](){
               std::cout << "Введите параметры линии: x1 y1 x2 y2, через пробел(int int int int):";
               auto prm_point = get_vector_value_from_in<int>();
               return create_primetive(Tline{{prm_point[0],prm_point[1]},{prm_point[2],prm_point[3]}});
           },
           [](){
               std::cout << "Введите параметры дуги: angle radius x y, через пробел(int int int int):";
               auto prm_point = get_vector_value_from_in<int>();
               return create_primetive(Tarc{prm_point[0],prm_point[1],{prm_point[2],prm_point[3]}});
           }
        };
        drawing->add_primetives(create_primrtive_handler[index]());
        return drawing;
    };
};
struct Tdelete_gprimetivew{
    std::unique_ptr<Tdocument> operator()(std::unique_ptr<Tdocument> drawing){
        std::cout << "Tdelete_gprimetivew\n";
        if(!drawing){
             std::cout << "Документ не создан\n";
            return drawing;
        }
        std::cout << "Введите id удаляемого примитива :";
        int index = get_vector_value_from_in<int>()[0];
        drawing->delete_primetives(index);
        return drawing;
    };
};

struct Tquit_program{
    std::unique_ptr<Tdocument> operator()(std::unique_ptr<Tdocument> drawing){
        std::cout << "Tquit_program\n";
        std::this_thread::sleep_for(std::chrono::seconds(1));
        if(drawing){
            std::cout << "Сохранить документ (0 - нет >0 - да ) :";
            int index = get_vector_value_from_in<int>()[0];
            if(index){
                std::make_unique<Texport_document_to_file>()->operator()(std::move(drawing));
            }
        }
        exit(0);
        return nullptr;
    };
};

int main()
{

//#if 0
    system("clear");
    std::unique_ptr<Tdocument> drawing;
    Tscreen_txt_stream screen{std::cout,2};
    std::vector<MenuItem> MenuItems{
        {"1 - Создание нового документа ",Tcreate_new_document()},
        {"2 - Импорт документа из файла ",Timport_document_from_file()},
        {"3 - Экспорт документа в файл ",Texport_document_to_file()},
        {"4 - Создание графического примитива ",Tcreate_gprimetivew()},
        {"5 - Удаление графического примитива ",Tdelete_gprimetivew()},
        {"6 - Выход из программы ",Tquit_program()}
    };
    while(1){
        for(auto&[key,handler]:MenuItems){
            std::cout << key << "\n";
        }
        std::size_t index = get_vector_value_from_in<int>()[0]-1;
        if(index> MenuItems.size()-1){
            std::cout << "Выбери верный пункт меню \n";
        }else{
            drawing=MenuItems[index].handler(std::move(drawing));
            system("clear");
            drawing->draw(screen);
        }
    }
//#endif // 0
}
