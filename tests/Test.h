#pragma once
#include <iostream>

#include <boost/test/data/test_case.hpp>

#include <test-config.h>

namespace std
{

template<typename T>
std::ostream& operator<<(std::ostream& out, const sf::Vector2<T> item)
{
    auto[x, y] = item;
    out << "{ x=" << x
        << " y=" << y
        << " }";

    return out;
}

std::ostream& operator<<(std::ostream& out, const sf::FloatRect& item)
{
    auto [left, top, width, height] = item;
    out << "{ left=" << left
        << " top=" << top
        << " width=" << width
        << " height=" << height
        << "}";

    return out;
}

} // namespace std

namespace tt
{

class NullWindow : public sf::RenderTarget
{
public:
    sf::Vector2u getSize() const override
    {
        return sf::Vector2u{4096,4096};
    }
};

void writeFile(const std::string& file, const std::string& data)
{
    boost::filesystem::path filepath{ file };

    if (!boost::filesystem::exists(filepath.parent_path()))
    {
        boost::filesystem::create_directories(filepath.parent_path());
    }

    std::ofstream out(file);
    out << data;
    out.close();
}


boost::filesystem::path tempFolder()
{
    auto temp = boost::filesystem::temp_directory_path();
    temp /= std::to_string(boost::unit_test::framework::current_test_case().p_id);
    boost::filesystem::create_directories(temp);
    return temp;
}

}