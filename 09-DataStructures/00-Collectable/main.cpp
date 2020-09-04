#include <iostream>
using namespace std;

class Collectable {
private:
    type_info m_type;

public:
    

};

class dynamic_set {

public:
    dynamic_set();
    ~dynamic_set();

    virtual int insert(Collectable *) = 0;
    virtual int remove(Collectable *) = 0;
    virtual bool find(Collectable *) = 0;
};

class linked_list: public dynamic_set
{

};

class Student: public Collectable
{

};


int main()
{
    dynamic_set pDynamicSet = linked_list();

}

