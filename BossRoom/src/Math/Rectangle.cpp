#include "Math/Rectangle.hpp"

const Rectangle Rectangle::ZERO = { 0, 0, 0, 0 };

Rectangle::Rectangle() {
}
Rectangle::Rectangle(const Rectangle& other) : Rectangle(other.pos, other.size) {
	
}
Rectangle::Rectangle(float x, float y, float w, float h) : Rectangle({ x, y }, { w, h }) {

}
Rectangle::Rectangle(const Vector2& pos, const Vector2& size, float a) :
	pos(pos),
	size(size),
	a(a){
}
Rectangle::Rectangle(const Vector2& A, const Vector2& B, const Vector2& D) : 
	pos(A), 
	size({ B.x - A.x, D.y - A.y }) {
}
Rectangle::~Rectangle() {
}

bool Rectangle::isInside(const Vector2 &P) {
	Vector2 A = pos;
	Vector2 B = { pos.x + size.x, pos.y };
	Vector2 D = { pos.x, pos.y + size.y };

	if(A.x == D.x && A.y == B.y)
		if(A.x < B.x && A.y < D.y)
			return A.x < P.x && P.x < B.x && A.y < P.y && P.y < D.y;
		else if(A.x > B.x && A.y < D.y)
			return A.x > P.x && P.x > B.x && A.y < P.y && P.y < D.y;
		else if(A.x < B.x && A.y > D.y)
			return A.x < P.x && P.x < B.x && A.y > P.y && P.y > D.y;
		else 
			return A.x > P.x && P.x > B.x && A.y > P.y && P.y > D.y;


	Vector2 AP = P - A;
	Vector2 AB = B - A;
	Vector2 AD = D - A;

	return (0 < AP * AB && AP * AB < AB * AB) && (0 < AP * AD && AP * AD < AD * AD);
}

Vector2 Rectangle::getSize() const {
	return{ w, h };
}
void Rectangle::setSize(const Vector2& size_) {
	size = size_;
}
const void Rectangle::draw(sf::RenderTarget& target, sf::Color color) {
	Vector2 A = pos;
	Vector2 B = { pos.x + size.x, pos.y };
	Vector2 D = { pos.x, pos.y + size.y };
	(B - A).render(target, A, color);
	(D - A).render(target, A, color);
	(B - A).render(target, D, color);
	(D - A).render(target, B, color);
}

bool Rectangle::operator==(const Rectangle &r) {
	return pos == r.pos && size == r.size && a == r.a;
}
Rectangle& Rectangle::operator=(const Rectangle& other) {
	pos = other.pos;
	size = other.size;
	return *this;
}

bool Rectangle::intersect(const Rectangle& rect) {
	if (a == 0.f && rect.a == 0.f) {
		return	  !(pos.x > rect.pos.x + rect.size.x ||
					pos.x + size.x < rect.pos.x ||
					pos.y > rect.pos.y + rect.size.y ||
					pos.y + size.y > rect.pos.y);
	}
	return false;
}
