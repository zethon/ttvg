#pragma once

#include <nlohmann/json.hpp>

#include <SFML/Graphics.hpp>

#include "Path.hpp"
#include "Intersection.h"

namespace nl = nlohmann;

namespace tt
{

class PathFactory;
using PathFactoryPtr = std::shared_ptr<PathFactory>;

////////////////////////////////////////////////////////////
/// \brief RiboPath generator for traffic system
///
////////////////////////////////////////////////////////////
class PathFactory final
{
public:
    ////////////////////////////////////////////////////////////
    /// \brief Construct the RiboPath generator
    ///
    /// \param size   The size of the world in which the paths exist
    ///
    ////////////////////////////////////////////////////////////
    PathFactory(const sf::Vector2i& size);

    ////////////////////////////////////////////////////////////
    /// \brief Set the edges
    ///
    /// The edges are the starting points of each RiboPath. When
    /// a path is generated, one edge is selected a random. The 
    /// edges are copied and PathFactory stores its own copy
    ///
    /// \param edges    Container of edges
    ///
    ////////////////////////////////////////////////////////////
    void setEdges(const TurningPoints& edges) 
    { 
        _edges = edges; 
    }

    ////////////////////////////////////////////////////////////
    /// \brief Set the list turning points
    ///
    /// The set of turning points will be used when a RiboPath is
    /// generated. The list of turning points is copied.
    ///
    /// \param points   Container of turning points
    ///
    /// \see addTurn
    ///
    ////////////////////////////////////////////////////////////
    void setTurningPoints(const TurningPoints& points) 
    { 
        _turns = points; 
    }
    ////////////////////////////////////////////////////////////
    /// \brief Add a single turning point
    ///
    /// Helper function that adds a single turning point.
    ///
    /// \param tp   The turning point to be added
    ///
    /// \see setTurningPoints
    ///
    ////////////////////////////////////////////////////////////
    void addTurn(const TurningPoint& tp)
    {
        _turns.push_back(tp);
    }
    
    ////////////////////////////////////////////////////////////
    /// \brief Generate a new RiboPath
    ///
    /// This overload accept a Path object in which to generate
    /// the path. This should be used during gameplay to avoid
    /// a vector copy
    ///
    /// \param path         The path in which to generate the RiboPath
    ///
    ////////////////////////////////////////////////////////////
    void makeRiboPath(Path& path) const;

    ////////////////////////////////////////////////////////////
    /// \brief Make and return a new RiboPath
    ///
    /// This overload returns a generated RiboPath. Generally this
    /// method will require a vector copy by the caller.
    ///
    /// \return The generated RiboPath
    ///
    ////////////////////////////////////////////////////////////
    Path makeRiboPath() const;

private:

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    TurningPoints   _edges; //!< Starting points that are slightly off the map
    TurningPoints   _turns; //!< Turns generated from configured intersections
    sf::Vector2i    _size;  //!< X and Y tilesize of the map

};

} // namespace tt