#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>
#include <vector>
#include <numeric>
#include <iomanip>
#include <math.h>
#include <cstdlib>
#include <cstring>
#include <cmath>
#include <iterator>
#include <charconv>
#include <memory>

#include <map>
#include <utility>
#include "Serializer.h"
#include <fstream>

/**
    \brief Шаблонный класс предоставляющий поток для вывода текстовой информации

    \tparam T -  тип потока
*/
template <typename T>
class Tscreen{
   T& screen;
public:
   Tscreen(T& screen):screen(screen){}
   virtual T& get_screen(){return screen;}
};

/**
    \brief Класс текстового экрана для вывода крафических приметивов с требуемыми отступами иерархии
*/

class Tscreen_txt_stream:public Tscreen <std::ostream>{
   int needed_layer_number;
   int current_layer_number;
   public:
   Tscreen_txt_stream(std::ostream& xcout = std::cout, int layer  = 0):Tscreen<std::ostream>{xcout},needed_layer_number(layer),current_layer_number(0){}
   int tearing_layer(){
       auto answer = (needed_layer_number > current_layer_number)?(needed_layer_number - current_layer_number):(current_layer_number - needed_layer_number);
       current_layer_number = needed_layer_number;
       return answer;
   }
   void reset_tearing_layer(){
       current_layer_number =0;
       Tscreen<std::ostream>::get_screen() << "\n";
   }
   int current_layer(){return current_layer_number;}
   void up_needed_layer(){needed_layer_number++;}
   void down_needed_layer(){if(needed_layer_number)needed_layer_number--;}
   
/**
    \brief Функция возврощает текстовый поток с требуемым уровнем иерархии 
*/
    std::ostream& get_screen(){
        return Tscreen<std::ostream>::get_screen() << std::string(tearing_layer(),' ');
    }
};

/**
    \brief Класс инкремента и декремента уровня иерархии в парадигме RAII 
*/
struct Ttab_untab_screen_txt_stream{
    Tscreen_txt_stream &screen;
    Ttab_untab_screen_txt_stream(Tscreen_txt_stream &screen):screen(screen){screen.up_needed_layer();}
    ~Ttab_untab_screen_txt_stream(){screen.down_needed_layer();}
};

/**
    \brief  Интерфейс графического премитива 
*/
class Tprimetiv{
    public:
/**
    \brief  Метод вывода приметива на текстовый экран  
	 \param [in] xout текстовый экран на котором необходимо вывести приметив 
*/
    virtual void draw(Tscreen_txt_stream& xout) =0;
/**
    \brief  Метод сериализации приметива ( например сохранения в файл)  
	 \param [in] сериализатор
*/
    virtual void serialize(ISerializer&) =0;
/**
    \brief  Метод десериализации приметива ( например чтения из файла)  
	 \param [in] сериализатор
*/
    virtual void deserialize(ISerializer&) =0;
};

/**
    \brief  Класс графического премитива - координата 
*/
class Tcoordinate:public Tprimetiv{
    int x,y;
    public:
    static std::string const type;
    Tcoordinate(int x =0,int y=0):x(x),y(y){}
    virtual void draw(Tscreen_txt_stream& xout)override;
    virtual void serialize(ISerializer&)override;
    virtual void deserialize(ISerializer&)override;
};

/**
    \brief  Класс графического премитива - точка 
*/
class Tpoint:public Tprimetiv{
    Tcoordinate coordinate;
    public:
    static std::string const type;
    Tpoint(int x =0,int y=0):coordinate(x,y){}
    virtual void draw(Tscreen_txt_stream& xout)override;
    virtual void serialize(ISerializer&)override;
    virtual void deserialize(ISerializer&)override;
};
/**
    \brief  Класс графического премитива - линия. 
*/
class Tline:public Tprimetiv{
    Tcoordinate begin_line;
    Tcoordinate end_line;
    public:
    static std::string const type;
    Tline(Tcoordinate begin=Tcoordinate(0,0),Tcoordinate end=Tcoordinate(0,0)):begin_line(begin),end_line(end){}
    virtual void draw(Tscreen_txt_stream& xout)override;
    virtual void serialize(ISerializer&)override;
    virtual void deserialize(ISerializer&)override;
};

/**
    \brief  Класс графического премитива - дуга. 
*/
class Tarc:public Tprimetiv{
    int angle;
    int radius;
    Tcoordinate center;
    public:
    static std::string const type;
    Tarc(int angle = 0,int radius =0,Tcoordinate center = Tcoordinate{}):angle(angle),radius(radius),center(center){}
    virtual void draw(Tscreen_txt_stream& xout)override;
    virtual void serialize(ISerializer&)override;
    virtual void deserialize(ISerializer&)override;
};
/**
    \brief  Класс графического премитива - полигон - может содержать группу графических приметивов и полигонов.
*/
class Tpoligon:public Tprimetiv{
    int id_primetives;
    std::map<int,std::unique_ptr<Tprimetiv>> primetives;
    public:
    static std::string const type;
    Tpoligon():id_primetives(0){}
    Tpoligon(Tpoligon&& other){
        if(this == &other) return;
        std::swap(id_primetives,other.id_primetives);
        std::swap(primetives,other.primetives);
    }
/**
    \brief  Метод добавления графического премитива в полигон .
*/
    void add_primetives(std::unique_ptr<Tprimetiv> primetiv){
        primetives[id_primetives++] = std::move(primetiv);
    }
/**
    \brief  Метод удаления графического премитива из полигона .
*/
    void delete_primetives(int delet_id_primetives){
        if(delet_id_primetives>id_primetives) return;
        primetives.erase(delet_id_primetives);
    }
    virtual void draw(Tscreen_txt_stream& xout)override;
    virtual void serialize(ISerializer&)override;
    virtual void deserialize(ISerializer&)override;
};

/**
    \brief  Класс Документ - верхний в иерархии. Содержит все приметивы текущиго открытого файла.
*/
class Tdocument: public Tpoligon{
    std::string name;
    public:
    static std::string const type;
    Tdocument(std::string&& name=""):name(name){}
    std::string& get_name(){return name;};
    virtual void draw(Tscreen_txt_stream& xout)override;
    virtual void serialize(ISerializer&)override;
    virtual void deserialize(ISerializer&)override;
};
/**
    \brief  Класс Фабрика создания  std::unique_ptr по входному типу графичесгого приметива
*/
class TfactoryPrimetives{
    public:
    template<class T>
    static std::unique_ptr<Tprimetiv> create_primetive(T&& args){
        return std::unique_ptr<Tprimetiv> {static_cast<Tprimetiv*> (new T(std::forward<T>(args)))};
    }
};
template <typename T>
std::unique_ptr<Tprimetiv> create_primetive(T&& args){
    return TfactoryPrimetives::create_primetive(std::forward<T>(args));
}

