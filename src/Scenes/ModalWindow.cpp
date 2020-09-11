#include <map>

#include <fmt/core.h>

#include "../Player.h"

#include "ModalWindow.h"

namespace tt
{

ModalWindow::ModalWindow(ResourceManager& resmgr, sf::RenderTarget& target)
    : Screen(resmgr, target)
{
    if (auto temp = _resources.load<sf::Font>("fonts/mono_bold.ttf");
        temp.has_value())
    {
        _font = *(temp);
    }
    else
    {
        // TODO: logging?!?!
    }

    _border = std::make_shared<sf::RectangleShape>();
    _border->setFillColor(sf::Color::Transparent);
    _border->setOutlineThickness(7.5);
    _border->setOutlineColor(sf::Color::Cyan);

    _background = std::make_shared<sf::RectangleShape>();
    _background->setFillColor(sf::Color{ 0, 0, 0 });

    float width = _window.getSize().x - (_window.getSize().x * 0.35f);
    float height = 175.f;
    _background->setSize(sf::Vector2f{ width, height });

    _text = std::make_shared<sf::Text>("", _font);
    _text->setFillColor(sf::Color::White);
    _text->setCharacterSize(25);

    setAlignment(Alignment::BOTTOM);

    // the order of these calls matters
    addDrawable(_background);
    addDrawable(_border);
    addDrawable(_text);
}

PollResult ModalWindow::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;
            
            case sf::Keyboard::Space:
            case sf::Keyboard::Escape:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, {} }};
            }
            break;
        }
    }

    return PollResult{};
}

void ModalWindow::setText(const std::string & text)
{
    _text->setString(text);
}

void ModalWindow::setHeight(float height)
{
    _background->setSize(sf::Vector2f{ _background->getSize().x, height });
    setAlignment(_alignment);
}

void ModalWindow::setWidth(float width)
{
    _background->setSize(sf::Vector2f{ width, _background->getSize().x });
    setAlignment(_alignment);
}

void ModalWindow::setAlignment(ModalWindow::Alignment al)
{
    _alignment = al;
    auto [width, height] = _background->getSize();

    switch (_alignment)
    {
        case ModalWindow::Alignment::TOP:
        {
            float xpos = (_window.getSize().x / 2) - (width / 2);
            float ypos = 25.f;
            _background->setPosition(xpos, ypos);
        }
        break;

        case ModalWindow::Alignment::CENTER:
        {
            float xpos = (_window.getSize().x / 2) - (width / 2);
            float ypos = (_window.getSize().y / 2) - (height / 2);
            _background->setPosition(xpos, ypos);
        }
        break;

        case ModalWindow::Alignment::BOTTOM:
        {
            float xpos = (_window.getSize().x / 2) - (width / 2);
            float ypos = _window.getSize().y - (height + 25.f);
            _background->setPosition(xpos, ypos);
        }
        break;
    }

    auto [xpos, ypos] = _background->getPosition();
    _text->setPosition(xpos + 10, ypos + 5);

    _border->setSize(_background->getSize());
    _border->setPosition(_background->getPosition());
}

////////////////////////////////////////////////////////////////////////////////////////////////

MessagesWindow::MessagesWindow(ResourceManager& resmgr, sf::RenderTarget& target)
    : ModalWindow(resmgr, target)
{
}

PollResult MessagesWindow::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
            break;

            case sf::Keyboard::Space:
            {
                _messages.pop_front();
                if (_messages.size() > 0)
                {
                    _text->setString(_messages.front());
                }
                else
                {
                    return { true, { ScreenActionType::CLOSE_MODAL, {} }};
                }
            }
            break;
            
            case sf::Keyboard::Escape:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, {} }};
            }
            break;
        }
    }

    return PollResult{};
}

////////////////////////////////////////////////////////////////////////////////////////////////

SelectionWindow::SelectionWindow(ResourceManager& resmgr, sf::RenderTarget& target)
    : ModalWindow(resmgr, target),
      _indicator("->", _font)
{   
    _indicator.setCharacterSize(25);
    _indicator.setFillColor(sf::Color::Yellow);
}

PollResult SelectionWindow::poll(const sf::Event& e)
{
    if (e.type == sf::Event::KeyPressed)
    {
        switch (e.key.code)
        {
            default:
                return ModalWindow::poll(e);
            
            case sf::Keyboard::Up:
            {
                prevSelection();
            }
            break;

            case sf::Keyboard::Down:
            {
                nextSelection();
            }
            break;

            case sf::Keyboard::Enter:
            case sf::Keyboard::Space:
            {
                return { true, { ScreenActionType::CLOSE_MODAL, _selection}};
            }
            break;
        }
    }

    return PollResult{};
}

void SelectionWindow::setText(const std::string& header)
{
    ModalWindow::setText(header);
    adjustLayout();
}

void SelectionWindow::addChoice(const std::string& choice)
{
    auto temptext = fmt::format("   {}", choice);
    auto temp = std::make_shared<sf::Text>(temptext, _font);
    temp->setCharacterSize(22);

    _choices.emplace_back(std::move(temp));

    addDrawable(_choices.back());
    adjustLayout();
}

void SelectionWindow::adjustLayout()
{
    setAlignment(_alignment);

    auto[xanchor, yanchor] = _background->getPosition();
    xanchor += _indicator.getGlobalBounds().width + 15.f;
    yanchor += 5.f;

    _text->setPosition(sf::Vector2f(xanchor, yanchor));
    yanchor += _text->getGlobalBounds().height + 20.f;

    for (auto& choice : _choices)
    {
        choice->setPosition(xanchor, yanchor);
        yanchor += choice->getGlobalBounds().height + 7.5f;
    }

    updateText();
}

void SelectionWindow::draw()
{
    ModalWindow::draw();
    _window.draw(_indicator);
}

void SelectionWindow::nextSelection()
{
    if (_selection == (_choices.size() - 1))
    {
        _selection = 0;
    }
    else
    {
        _selection++;
    }

    updateText();
}

void SelectionWindow::prevSelection()
{
    if (_selection == 0)
    {
        _selection = _choices.size() - 1;
    }
    else
    {
        _selection--;
    }

    updateText();
}

void SelectionWindow::updateText()
{
    if (_choices.size() == 0)
    {
        return;
    }

    std::for_each(_choices.begin(), _choices.end(),
        [](auto text)
        {
            text->setFillColor(sf::Color::White);
            text->setStyle(sf::Text::Regular);
        });

    _choices.at(_selection)->setFillColor(sf::Color::Yellow);
    _choices.at(_selection)->setStyle(sf::Text::Style::Bold);

    auto xpos = _text->getPosition().x;
    auto ypos = _choices.at(_selection)->getPosition().y;
    _indicator.setPosition(xpos, ypos);
}

////////////////////////////////////////////////////////////////////////////////////////////////

InventoryWindow::InventoryWindow(ResourceManager& resmgr, sf::RenderTarget& target, PlayerPtr player)
    : SelectionWindow(resmgr, target)
{
    setAlignment(ModalWindow::Alignment::CENTER);

    // count and name
    using InvAgg = std::tuple<std::uint32_t, std::string>;
    std::map<std::string, InvAgg> aggregate;

    for (const auto& item : player->getInventory())
    {
        if (aggregate.find(item->getID()) != aggregate.end())
        {
            std::get<0>(aggregate[item->getID()])++;
        }
        else
        {
            aggregate[item->getID()] = std::make_tuple(1, item->getName());
        }
    }

    setText("You are carrying...");

    if (aggregate.size() >= 10)
    {
        setHeight(height() + 350.f);
    }
    if (aggregate.size() >= 5)
    {
        setHeight(height() + 175.f);
    }

    for (const auto& item : aggregate)
    {
        const auto&[count, name] = item.second;
        addChoice(fmt::format("{} x {}", name, count));
    }
}

} // namespace tt