#include "Math/Rectangle.hpp"



Rectangle::Rectangle() {
}
Rectangle::Rectangle(const Vector2& pos, const Vector2& size, float a) : 
	A(pos), 
	B(pos + Vector2(cosf(a) * size.x, sinf(a) * size.x)),
	D(pos + Vector2(-sinf(a) * size.y, cosf(a) * size.y)) {
}
Rectangle::Rectangle(const Vector2& A, const Vector2& B, const Vector2& D) : 
	A(A), 
	B(B), 
	D(D) {
}
Rectangle::~Rectangle() {
}

bool Rectangle::isInside(const Vector2 &P) {
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
	return{ getWidth(), getHeight() };
}
float Rectangle::getWidth() const {
	return (B - A).length();
}
float Rectangle::getHeight() const {
	return (D - A).length();
}
void Rectangle::setSize(const Vector2& size) {
	Vector2 AB = B - A;
	Vector2 AD = D - A;
	B = A + AB.normalize() * size.x;
	D = A + AD.normalize() * size.y;
}
const void Rectangle::draw(sf::RenderTarget& target, sf::Color color) {
	(B - A).render(target, A, color);
	(D - A).render(target, A, color);
	(B - A).render(target, D, color);
	(D - A).render(target, B, color);
}

bool Rectangle::operator==(const Rectangle &r) {
	return A == r.A && B == r.B && D == r.D;
}
