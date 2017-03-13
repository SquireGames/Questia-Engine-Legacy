#include "QuestiaEng/EntityManager/Entity/Entity_Obj/Entity_Coll/Bounds.h"

Bounds::Bounds(Circ circ):
  //  area(circ)
     coll_shape(Shape::circ)
{

}

Bounds::Bounds(Rect rect):
   //area(rect)
     coll_shape(Shape::rect)
{

}

Bounds::Bounds(Poly poly):
  //  area(poly)
     coll_shape(Shape::poly)
{

}

Bounds::Bounds(Dot dot):
   // area(dot)
     coll_shape(Shape::dot)
{

}

Bounds::Bounds():
   // area(None())
     coll_shape(Shape::none)
{

}

Bounds::Shape Bounds::getShape()
{
    return coll_shape;
}
