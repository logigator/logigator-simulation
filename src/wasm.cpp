#include <emscripten/bind.h>
#include "board.h"
#include "component.h"
#include "link.h"
#include "and.h"
#include "not.h"

int test() {
    Board* board = new Board();
    Link** links = new Link*[3]
    {
        new Link(board),
        new Link(board),
        new Link(board)
    };
    Component** components = new Component*[2]
    {
        new NOT(board, new Link*[1] { links[0] }, new Link*[1] { links[1] }),
        new AND(board, new Link*[2] { links[0], links[1] }, new Link*[1] { links[2] })
    };
    board->init(components, links, 2, 3, 1);
    board->start(100);
    return (int)links[0]->powered;
}

EMSCRIPTEN_BINDINGS(module)
{
	emscripten::function("test", &test);
}
