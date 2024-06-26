#include "Serializer.h"
#include "main.h"

    void BoostSerializer::serializeInt(int data){
        if(!os) return;
        boost::archive::text_oarchive oa(*os);
        oa << data;
    }
    void BoostSerializer::deserializeInt(int& data){
        if(!is) return;
        boost::archive::text_iarchive ia(*is);
        ia >> data;
    }
    void BoostSerializer::serializeString(const std::string& data){
        if(!os) return;
        boost::archive::text_oarchive oa(*os);
        oa << data;
    }
    void BoostSerializer::deserializeString(std::string& data){
        if(!is) return;
        boost::archive::text_iarchive ia(*is);
        ia >> data;
    }

//-------------------------------------------------------------
std::string const Tcoordinate::type ="Tcoordinate";
void Tcoordinate::serialize(ISerializer& serializer){
    serializer.serializeString(type);
    serializer.serializeInt(x);
    serializer.serializeInt(y);
}
void Tcoordinate::deserialize(ISerializer&serializer){
    serializer.deserializeInt(x);
    serializer.deserializeInt(y);
}
std::string const Tpoint::type ="Tpoint";
void Tpoint::serialize(ISerializer&serializer){
    serializer.serializeString(type);
    coordinate.serialize(serializer);
}
void Tpoint::deserialize(ISerializer&serializer){
    std::string type;
    serializer.deserializeString(type);
    coordinate.deserialize(serializer);
}
std::string const Tline::type ="Tline";
void Tline::serialize(ISerializer&serializer){
    serializer.serializeString(type);
    begin_line.serialize(serializer);
    end_line.serialize(serializer);
}
void Tline::deserialize(ISerializer&serializer){
    std::string type;
    serializer.deserializeString(type);
    begin_line.deserialize(serializer);
    serializer.deserializeString(type);
    end_line.deserialize(serializer);
}
std::string const Tarc::type ="Tarc";
void Tarc::serialize(ISerializer&serializer){
    serializer.serializeString(type);
    center.serialize(serializer);
    serializer.serializeInt(angle);
    serializer.serializeInt(radius);

}
void Tarc::deserialize(ISerializer&serializer){
    std::string type;
    serializer.deserializeString(type);
    center.deserialize(serializer);
    serializer.deserializeInt(angle);
    serializer.deserializeInt(radius);
}
std::string const Tpoligon::type ="Tpoligon";
void Tpoligon::serialize(ISerializer&serializer){
    serializer.serializeString(type);
    serializer.serializeInt(primetives.size());
    for(auto& primetiv: primetives){
        primetiv.second->serialize(serializer);
    }
}

using Tcreator_clear_primetiv =std::unique_ptr<Tprimetiv> (*)();
const std::map<const std::string,Tcreator_clear_primetiv> map_creator_primrtives{
    {Tcoordinate::type,[](){return std::unique_ptr<Tprimetiv> ( static_cast<Tprimetiv*> (new Tcoordinate()));}},
    {Tpoint::type,[](){return std::unique_ptr<Tprimetiv> ( static_cast<Tprimetiv*> (new Tpoint()));}},
    {Tline::type,[](){return std::unique_ptr<Tprimetiv> ( static_cast<Tprimetiv*> (new Tline()));}},
    {Tarc::type,[](){return std::unique_ptr<Tprimetiv> ( static_cast<Tprimetiv*> (new Tarc()));}},
    {Tpoligon::type,[](){return std::unique_ptr<Tprimetiv> ( static_cast<Tprimetiv*> (new Tpoligon()));}},
};

std::unique_ptr<Tprimetiv> creator_clear_primetiv(const std::string key_for_type){
    return map_creator_primrtives.at(key_for_type)();
}

void Tpoligon::deserialize(ISerializer&serializer){
    int size;
    serializer.deserializeInt(size);
    Tscreen_txt_stream screen{std::cout,1};
    while(size--){
        std::string type_primetive;
        serializer.deserializeString(type_primetive);
        std::unique_ptr<Tprimetiv> primetive  = creator_clear_primetiv(type_primetive);
        primetive->deserialize(serializer);
        add_primetives(std::move(primetive));
    }
}
std::string const Tdocument::type ="Tdocument";
void Tdocument::serialize(ISerializer&serializer){
    serializer.serializeString(type);
    serializer.serializeString(name);
    Tpoligon::serialize(serializer);

}
void Tdocument::deserialize(ISerializer&serializer){
    std::string type;
    serializer.deserializeString(type);
    serializer.deserializeString(name);
    std::string type_poligon;
    serializer.deserializeString(type_poligon);
    Tpoligon::deserialize(serializer);
}
