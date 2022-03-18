#ifndef CUSTOMSHAPE
#define CUSTOMSHAPE

#include <SFML/Graphics/ConvexShape.hpp>

class CustomShape : public sf::ConvexShape {
public:
    const bool contains(int x, int y) {
        sf::Vector2f tp(x, y);
		std::vector<sf::Vector2f> vertices = getVertices();
	    int pos = 0;
	    int neg = 0;
		// g_logger.log(Logger::INFO, "Contains");
		// g_logger.log(Logger::INFO, "Position:");
		// g_logger.log(Logger::INFO, getPosition());

	    for (unsigned int i = 0; i < vertices.size(); i++){
			if (vertices[i] == tp) {
				return true;
			}
			// Form a segment between two points
			sf::Vector2f point1 = vertices[i];
			sf::Vector2f point2 = vertices[(i+1)%vertices.size()];
			// Compute the cross product
			int cross = (tp.x - point1.x)*(point2.y - point1.y) - (tp.y - point1.y)*(point2.x - point1.x);
			if (cross > 0) pos++;
	        if (cross < 0) neg++;
			// If the sign changes, then no
			if (pos > 0 && neg > 0) {
				return false;
			}
	    }
		return true;
    }
    
protected:
    const std::vector<sf::Vector2f> getVertices() {
		sf::Transform t = getTransform();
		return {t.transformPoint(getPoint(0)),
			t.transformPoint(getPoint(1)),
			t.transformPoint(getPoint(2)),
			t.transformPoint(getPoint(3))};
	}
};

#endif