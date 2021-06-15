#include "variable.h"

variable::variable(value_type typ, value val)
{
    this->type_ = typ;
    this->value_ = val;
}

variable::variable(const variable &var)
{
    if(var.type_ == value_type::STRING)
    {
        this->str = var.str;
    }
    else if (var.type_ == value_type::DATA)
    {
        this->data = var.data;
    }
    else
    {
        this->value_ = var.value_;
    }
    this->type_ = var.type_;
}

variable::variable(std::string str)
{
    this->type_ = value_type::STRING;
    this->str = str;
}

variable::variable(std::vector<unsigned char> data)
{
    this->type_ = value_type::DATA;
    this->data = data;
}

bool variable::operator==(const variable &other)
{
    if(this->type_ != other.type_)
    {
        return false;
    }

    if(other.type_ == value_type::STRING)
    {
        return (this->str.compare(other.str) == 0);
    }
    else if (other.type_ == value_type::DATA)
    {
        return (this->data == other.data);
    }
    else
    {
        return (this->value_.uint64 == other.value_.uint64);
    }

}
