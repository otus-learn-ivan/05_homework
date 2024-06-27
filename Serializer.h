#ifndef SERIALIZER_H
#define SERIALIZER_H

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

/**
    \brief  Интерфейс  сериализатора графического премитива 
*/
class ISerializer
{
    public:
        virtual void serializeInt(int data) = 0;
        virtual void deserializeInt(int& data) = 0;
        virtual void serializeString(const std::string& data) = 0;
        virtual void deserializeString(std::string& data) = 0;
};
/**
    \brief  Класс реализации сериализатора графического премитива  с помощью boost::serialization
*/
class BoostSerializer : public ISerializer {
    std::ostream* os;
    std::istream* is;
public:
    BoostSerializer (std::ostream* os,std::istream* is):os(os),is(is){}
    void serializeInt(int data) override;
    void deserializeInt(int& data) override;
    void serializeString(const std::string& data)override;
    void deserializeString(std::string& data) override;
};

#endif // SERIALIZER_H
